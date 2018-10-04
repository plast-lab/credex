#include "Walkers.h"

#include "Inliner.h"
#include "Creators.h"
#include "FileParser.h"
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


void ExplicitInlinePass::run_pass(DexStoresVector& stores, 
                                  ConfigFiles& cfg, 
                                  PassManager& mgr) 
{
  IRList::iterator it;  
  uint16_t offset, idx;
  MethodImpls method_impls;
  MethodsToInline imethods;
  DexMethod *caller, *callee;
  std::string caller_str, callee_name;

  FileParser::parse_file("imethods.txt", imethods, stores);

  for(auto map_entry : imethods){

    FileParser::analyze_invoc_id(map_entry.first, caller_str, callee_name, offset);
    FileParser::to_dalvik_format(caller_str);
    FileParser::find_def(caller_str, caller, stores);
    // std::cout << "caller_str = " << caller_str << std::endl;
    // std::cout << "offset = " << offset << std::endl;
    // std::cout << "caller = " << caller << std::endl;
    // std::cout << "callee_name = " << callee_name << std::endl;

    // std::cout << SHOW(caller->get_code()) << std::endl;

    idx = 0;    
    auto caller_code = caller->get_code();
    auto caller_mc = new MethodCreator(caller);
    auto ii = InstructionIterable(caller_code);

    for(it = ii.begin().unwrap(); it != ii.end().unwrap(); it++){
      if(it->insn->opcode() == OPCODE_INVOKE_VIRTUAL ||
         it->insn->opcode() == OPCODE_INVOKE_INTERFACE){
        std::string meth, name;
        name = it->insn->get_method()->c_str();
        meth = it->insn->get_method()->get_class()->get_name()->str();
        meth = JavaNameUtil::internal_to_external(meth) + "." + name;

        if(callee_name == meth && idx++ == offset)  break;
      }
      
    }

    method_impls = map_entry.second;
    if(method_impls.size() > 1){

      DexType* type;
      uint16_t this_reg;
      IRInstruction *insn;
      IRList::iterator erase_it, true_block_it, 
        main_block_it, false_block_it, instanceof_it;

      erase_it = it;  //iterator to erase the invoke instruction after it's inlined
      true_block_it = it;
      insn = it->insn;
      this_reg = insn->src(0);
      main_block_it = ++it;
      
      for(auto callee : method_impls){
        type = callee->get_class();

        //get next free register to use as destination register
        //for instance_of instruction
        auto dst_location = caller_code->get_registers_size();
        caller_code->set_registers_size(dst_location+1);

        create_instanceof(true_block_it, instanceof_it, caller_code,
                          type, this_reg, dst_location);
        create_guards(main_block_it, true_block_it, false_block_it, 
            instanceof_it, idx, offset, dst_location, caller_code);

        //create new invoke instruction to add to the false block
        //in order to inline the callee later
        auto invoke_insn = new IRInstruction(insn->opcode());
        invoke_insn->set_method(callee)->set_arg_word_count(insn->arg_word_count());

        for (uint16_t i = 0; i < insn->srcs().size(); i++) {
          invoke_insn->set_src(i, insn->srcs().at(i));
        }
        if (false_block_it == caller_code->end()) {
          caller_code->push_back(invoke_insn);
          std::prev(caller_code->end());
        } else {
          false_block_it = caller_code->insert_after(false_block_it, invoke_insn);
        }

        inliner::inline_method(caller->get_code(),
                               callee->get_code(),
                               false_block_it);      
      }

      // if the callee returns anything other than void,
      //find the move-result after the invoke
      auto move_res = erase_it;
      while (move_res++ != caller_code->end()){
        if (move_res->type == MFLOW_OPCODE && 
          is_move_result(move_res->insn->opcode())) break;
      }
      //erases the iterator and deletes the MethodItemEntry
      caller_code->erase_and_dispose(erase_it);
      caller_code->erase_and_dispose(move_res);    
    }
    else if(method_impls.size()){
      std::cout << *method_impls.begin() << std::endl;
      callee = *method_impls.begin();
      inliner::inline_method(caller->get_code(), callee->get_code(), it);
    }
    caller_mc->create(); 
    // caller_code->build_cfg();
    // std::cout << SHOW(caller_code->cfg()) << std::endl;
  }           
  std::cout << SHOW(caller->get_code()) << std::endl;
  std::cout << "Explicit inline pass done." << std::endl;
}

static ExplicitInlinePass s_pass;