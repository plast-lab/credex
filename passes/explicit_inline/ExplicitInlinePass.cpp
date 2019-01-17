#include "Inliner.h"
#include "Creators.h"
#include "VirtualScope.h"
#include "ClassHierarchy.h"
#include "ExplicitInlinePass.h"

#include "../simpleinline/Deleter.h"

void ExplicitInlinePass::create_instanceof(IRList::iterator& true_block_it,
                                          IRList::iterator& instanceof_it,
                                          IRCode*& caller_code, 
                                          DexType*& type,
                                          const uint16_t this_reg, 
                                          const uint16_t dst_location)
{
  auto instance_of_insn = new IRInstruction(OPCODE_INSTANCE_OF);
  auto move_res = new IRInstruction(IOPCODE_MOVE_RESULT_PSEUDO);
  instance_of_insn->set_src(0, this_reg);
  instance_of_insn->set_type(type);
  move_res->set_dest(dst_location);

  instanceof_it = caller_code->insert_after(true_block_it, instance_of_insn);
  instanceof_it = caller_code->insert_after(instanceof_it, move_res);
}

void ExplicitInlinePass::create_guards(IRList::iterator& main_block_it, 
                                      IRList::iterator& true_block_it, 
                                      IRList::iterator& false_block_it,
                                      IRList::iterator& instanceof_it, 
                                      bool first_time, 
                                      const uint16_t dst_location, 
                                      IRCode*& caller_code)
{
  //if block
  auto if_insn = new IRInstruction(OPCODE_IF_EQZ);
  if_insn->set_src(0, dst_location);
  auto if_entry  = new MethodItemEntry(if_insn);
  false_block_it = caller_code->insert_after(instanceof_it, *if_entry);

  auto goto_insn  = new IRInstruction(OPCODE_GOTO);
  auto goto_entry = new MethodItemEntry(goto_insn);
  IRList::iterator goto_it;
  if(first_time){
    first_time = false;
    goto_it = caller_code->insert_before(main_block_it, *goto_entry);
  }
  else{
    goto_it = caller_code->insert_after(main_block_it, *goto_entry);
  }

  // main block
  auto main_bt  = new BranchTarget(goto_entry);
  auto mb_entry = new MethodItemEntry(main_bt);
  main_block_it = caller_code->insert_after(goto_it, *mb_entry);


  // else block
  auto else_bt  = new BranchTarget(if_entry);
  auto eb_entry = new MethodItemEntry(else_bt);
  true_block_it = caller_code->insert_after(goto_it, *eb_entry);  
}

void ExplicitInlinePass::eval_pass(DexStoresVector& stores,
                                   ConfigFiles& cfg,
                                   PassManager& mgr)
{
  FileParser::parse_file("imethods.txt", imethods, sorted_callers);
}

void ExplicitInlinePass::run_pass(DexStoresVector& stores, 
                                  ConfigFiles& cfg, 
                                  PassManager& mgr) 
{
  MethodsToInline mti;
  IRList::iterator it;  
  MethodImpls method_impls;

  std::unordered_set<DexMethod*> inlined;

  for(auto caller : sorted_callers){
    mti = imethods[caller];
    auto caller_code = caller->get_code();
    auto caller_mc = new MethodCreator(caller);

    for(auto map_it = mti.begin(); map_it != mti.end(); map_it++){
      bool first_time = true;
      inline_stats.callsites++;

      for(it = caller_code->begin(); it != caller_code->end(); it++){
        if (it->type != MFLOW_OPCODE) continue;
        if(!is_invoke(it->insn->opcode())) continue;
        if(map_it->first == (unsigned long)&*it)  break;
      }

      //the invocation is already inlined 
      //or removed by previous optimizations
      if(it == caller_code->end())  continue;

      //gather statistics
      if(it->insn->opcode() == OPCODE_INVOKE_DIRECT)
        inline_stats.invoke_direct++;
      if(it->insn->opcode() == OPCODE_INVOKE_STATIC)
        inline_stats.invoke_static++;
      if(it->insn->opcode() == OPCODE_INVOKE_SUPER)
        inline_stats.invoke_super++;
      if(it->insn->opcode() == OPCODE_INVOKE_VIRTUAL)
        inline_stats.invoke_virtual++;
      if(it->insn->opcode() == OPCODE_INVOKE_INTERFACE)
        inline_stats.invoke_interface++;

      method_impls = map_it->second;
      if(method_impls.size() > 1){
        DexType* type;
        uint16_t this_reg;
        IRInstruction *insn, *invoke_insn, *move_res_insn;
        IRList::iterator true_block_it, main_block_it, 
            false_block_it, instanceof_it;

        true_block_it = it;
        insn = it->insn;
        this_reg = insn->src(0);
        main_block_it = std::next(it);

        // find the move-result after the invoke, if any
        auto move_res_it = it;
        while(move_res_it++ != caller_code->end()){
          if(move_res_it->type == MFLOW_OPCODE) break;
        }
        if(is_move_result(move_res_it->insn->opcode())) {
          true_block_it = move_res_it; 
          main_block_it = std::next(move_res_it);
        }
        else{
          move_res_it = caller_code->end();
        }

        //get next free register to use as destination register
        //for the instance_of instructions and the new move_result 
        auto dst_location = caller_code->get_registers_size();
        caller_code->set_registers_size(dst_location+1);
                
        uint16_t count = 0;
        for(auto callee : method_impls){
          count++;
          inline_stats.need_guards++;
          inline_stats.total_meths++;
          
          type = callee->get_class();
          create_instanceof(true_block_it, instanceof_it, caller_code,
            type, this_reg, dst_location);
          create_guards(main_block_it, true_block_it, false_block_it, 
              instanceof_it, first_time, dst_location, caller_code);

   
          //create new invoke instruction to add to the false block
          //in order to inline the callee later
          uint16_t arg_count = insn->arg_word_count();
          invoke_insn = new IRInstruction(insn->opcode());
          invoke_insn->set_method(callee)->set_arg_word_count(arg_count);
          for (uint16_t i = 0; i < insn->srcs().size(); i++) {
            invoke_insn->set_src(i, insn->src(i));
          }

          if (false_block_it == caller_code->end()) {
            caller_code->push_back(invoke_insn);
            false_block_it = std::prev(caller_code->end());
          } else {
            false_block_it = caller_code->insert_after(false_block_it, invoke_insn); 
          }
          if(move_res_it != caller_code->end()){
            auto opcode = move_res_it->insn->opcode();
            move_res_insn = new IRInstruction(opcode);
            move_res_insn->set_dest(move_res_it->insn->dest());
            caller_code->insert_after(false_block_it, move_res_insn);
          }

          inliner::inline_method(caller_code, callee->get_code(), false_block_it);          
          change_visibility(callee);
          inlined.insert(callee);
        }
        //add new invoke instruction in case all guards fail
        invoke_insn->set_method(insn->get_method());
        true_block_it = caller_code->insert_after(true_block_it, invoke_insn);
        //add move result instruction, if any
        if(move_res_it != caller_code->end()){
          caller_code->insert_after(true_block_it, move_res_insn);
        }

        caller_mc->create();
        caller_code->erase_and_dispose(it); 
        if(move_res_it != caller_code->end()){
          caller_code->erase_and_dispose(move_res_it);      
        } 
      }
      else if(method_impls.size() == 1){
        inline_stats.total_meths++;
        if(it->insn->opcode() == OPCODE_INVOKE_VIRTUAL ||
            it->insn->opcode() == OPCODE_INVOKE_INTERFACE){
          inline_stats.devirtualizable++;
        }
    
        auto callee = *method_impls.begin();
        inliner::inline_method(caller_code, callee->get_code(), it);
        change_visibility(callee);
        inlined.insert(callee);
      }
    }
  }

  //statistics from parsing
  std::cout << "Total methods parsed : " << FileParser::total_meths_parsed;
  std::cout << "\nTotal unknown methods to Redex : " << FileParser::unknown_methods;
  std::cout << "\nTotal methods with no code : " << FileParser::meths_with_no_code;
  std::cout << "\nTotal methods with recursive calls : " << FileParser::recursive_calls;

  //statistics from the pass
  std::cout << "\nTotal inlined methods : " << inline_stats.total_meths;
  std::cout << "\nTotal invocation sites : " << inline_stats.callsites;
  std::cout << "\nTotal super invocations : " << inline_stats.invoke_super;
  std::cout << "\nTotal static invocations : " << inline_stats.invoke_static;
  std::cout << "\nTotal direct invocations : " << inline_stats.invoke_direct;
  std::cout << "\nTotal virtual invocations : " << inline_stats.invoke_virtual;
  std::cout << "\nTotal interface invocations : " << inline_stats.invoke_interface;
  std::cout << "\nTotal virtual methods inlined with guards : " << inline_stats.need_guards;
  std::cout << "\nTotal virtual methods inlined without guards : " << inline_stats.devirtualizable;
  
  std::cout << std::endl << std::endl;
  std::cout << "Explicit inline pass done." << std::endl;
}

static ExplicitInlinePass s_pass;