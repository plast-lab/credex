#include "Inliner.h"
#include "Creators.h"
#include "VirtualScope.h"
#include "ClassHierarchy.h"
#include "ExplicitInlinePass.h"

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
                                      uint16_t idx, 
                                      const uint16_t offset, 
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
  if(idx == offset){
    idx++;
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
  std::cout << "file parsing ended " << imethods.size() << std::endl;
  // for(auto caller : sorted_callers){
  //   for(auto insn : imethods[caller]){
  //     for(auto callee : insn.second){
  //       std::cout << "callee_code = " << show(callee->get_code()) << std::endl;
  //     }
  //   }
  // }
}

void ExplicitInlinePass::run_pass(DexStoresVector& stores, 
                                  ConfigFiles& cfg, 
                                  PassManager& mgr) 
{
  MethodsToInline mti;
  uint16_t offset, idx;
  IRList::iterator it;  
  MethodImpls method_impls;

  for(auto caller : sorted_callers){

    mti = imethods[caller];
    auto caller_code = caller->get_code();
    auto caller_mc = new MethodCreator(caller);

    for(auto map_it = mti.begin(); map_it != mti.end(); map_it++){
      idx = 0;
      offset = 0;
      inline_stats.callsites++;

      for(it = caller_code->begin(); it != caller_code->end(); it++){
        idx++;
        offset++;

        if (it->type != MFLOW_OPCODE) continue;
        if(!is_invoke(it->insn->opcode())){
          continue;
        }

        if(*(map_it->first.insn) == *(it->insn)){
          if(it->insn->opcode() == OPCODE_INVOKE_VIRTUAL)
            inline_stats.invoke_virtual++;
          if(it->insn->opcode() == OPCODE_INVOKE_INTERFACE)
            inline_stats.invoke_interface++;
          break;
        } 
        
      }

      //the invocation is already inlined 
      //or removed by previous optimizations
      if(it == caller_code->end()) continue;
      
      method_impls = map_it->second;
      if(method_impls.size() > 1){
        inline_stats.need_guards++;

        DexType* type;
        uint16_t this_reg;
        IRInstruction *insn;
        IRList::iterator true_block_it, main_block_it, 
            false_block_it, instanceof_it;

        true_block_it = it;
        insn = it->insn;
        this_reg = map_it->first.insn->src(0);
        main_block_it = std::next(it);

        // find the move-result after the invoke, if any
        auto move_res_it = it;
        while (move_res_it++ != caller_code->end() && move_res_it->type != MFLOW_OPCODE);
        if (is_move_result(move_res_it->insn->opcode())) {
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
          inline_stats.total_meths++;
          
          if(count < method_impls.size()){
            type = callee->get_class();
            create_instanceof(true_block_it, instanceof_it, caller_code,
              type, this_reg, dst_location);
            create_guards(main_block_it, true_block_it, false_block_it, 
                instanceof_it, idx, offset, dst_location, caller_code);
          }
          else{
            false_block_it = true_block_it;
          }
   
          //create new invoke instruction to add to the false block
          //in order to inline the callee later
          auto invoke_insn = new IRInstruction(insn->opcode());
          invoke_insn->set_method(callee)->set_arg_word_count(insn->arg_word_count());

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
            auto move_res_insn = new IRInstruction(move_res_it->insn->opcode());
            move_res_insn->set_dest(move_res_it->insn->dest());
            caller_code->insert_after(false_block_it, move_res_insn);
          }

          inliner::inline_method(caller->get_code(),
                                 callee->get_code(),
                                 false_block_it);
          
          change_visibility(callee);
        }
        caller_mc->create();
        // erases the iterator and deletes the MethodItemEntry
        caller_code->erase_and_dispose(it); 
        if(move_res_it != caller_code->end()){
          caller_code->erase_and_dispose(move_res_it);      
        }  
      }
      else if(method_impls.size() == 1){
        inline_stats.total_meths++;
        auto callee = *method_impls.begin();
        inliner::inline_method(caller->get_code(), callee->get_code(), it);
        change_visibility(callee);
      }
    }
  }           
  std::cout << "Total inlined methods : " << inline_stats.total_meths << std::endl;
  std::cout << "Total invocation sites : " << inline_stats.callsites << std::endl;
  std::cout << "Total virtual invocations : " << inline_stats.invoke_virtual << std::endl;
  std::cout << "Total interface invocations : " << inline_stats.invoke_interface << std::endl;
  std::cout << "Total virtual methods inlined with guarding : " << inline_stats.need_guards;
  std::cout << std::endl << std::endl;
  std::cout << "Explicit inline pass done." << std::endl;
}

static ExplicitInlinePass s_pass;