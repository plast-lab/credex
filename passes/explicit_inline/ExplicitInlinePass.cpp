    #include "IRCode.h"
    #include "DexUtil.h"
    #include "Walkers.h"
    #include "Inliner.h"
    #include "DexClass.h"
    #include "Creators.h"
    #include "VirtualScope.h"
    #include "ClassHierarchy.h"
    #include "ExplicitInlinePass.h"

bool compare_class_types(const DexMethod* meth1, const DexMethod* meth2){
  auto cls_type1 = type_class(meth1->get_class());
  auto cls_type2 = type_class(meth2->get_class());
  auto cls1 = cls_type1->get_super_class();
  auto cls2 = cls_type2->get_type();
  auto obj_type = get_object_type();
  
  while(cls1 != obj_type && cls1 != nullptr){
    if(cls1->c_str() == cls2->c_str()){
      return true;
    }
    cls1 = type_class(cls1)->get_super_class();
  }
  return false;
}

struct class_type_comparator {
  bool operator()(const DexMethod* meth1, const DexMethod* meth2) const {
    return compare_class_types(meth1, meth2);
  }
};

using MethodImpls = std::set<DexMethod*, class_type_comparator>;
using MethodsToInline = std::unordered_map<std::string, MethodImpls>; 

void parse_file(std::string filename, MethodsToInline& imethods){
  std::ifstream file(filename);

  if(file.is_open()){
    while(!file.eof()){
      std::string method, callsite;
      std::getline (file, method, '\t');
      std::getline (file, callsite, '\n');

      if(!method.empty() && !callsite.empty()){
        if(imethods.find(callsite) == imethods.end()){
          MethodImpls method_impls;
          imethods.insert(std::make_pair(callsite, method_impls));
        }
        imethods[callsite].insert((DexMethod*) DexMethod::get_method(method));
      }
    }
  }
  else{ 
    std::cout << "Could not open file " << filename << std::endl;
  }
}

std::string get_caller_str(std::string caller_name){    
  char last_char = caller_name.c_str()[caller_name.size()-1];
  while(isdigit(last_char) || last_char == '/'){
    caller_name = caller_name.substr(0, caller_name.size()-1);
    last_char = caller_name.c_str()[caller_name.size()-1];
  }

  return caller_name;
}

uint16_t get_insn_index(std::string callsite){
  size_t last_index = callsite.find_last_not_of("0123456789");
  std::string insn_index = callsite.substr(last_index + 1);

  return atoi(insn_index.c_str());
}

void create_instanceof(IRList::iterator& true_block_it,
                      IRList::iterator& instanceof_it, 
                      IRCode* caller_code,
                      DexType*& type,
                      int this_reg,
                      int dst_location)
{
  auto instance_of_insn = new IRInstruction(OPCODE_INSTANCE_OF);
  auto move_res = new IRInstruction(IOPCODE_MOVE_RESULT_PSEUDO);
  instance_of_insn->set_src(0, this_reg);
  instance_of_insn->set_type(type);
  move_res->set_dest(dst_location);
  instanceof_it = caller_code->insert_after(true_block_it, instance_of_insn);
  instanceof_it = caller_code->insert_after(instanceof_it, move_res);
}

void create_guards(IRList::iterator& main_block_it, 
                   IRList::iterator& true_block_it,
                   IRList::iterator& false_block_it,
                   IRList::iterator& instanceof_it, 
                   int idx, 
                   int insn_index, 
                   int dst_location,
                   IRCode* caller_code)
{
  //if block
  auto if_insn = new IRInstruction(OPCODE_IF_EQZ);
  if_insn->set_src(0, dst_location);
  auto if_entry  = new MethodItemEntry(if_insn);
  false_block_it = caller_code->insert_after(instanceof_it, *if_entry);

  auto goto_insn  = new IRInstruction(OPCODE_GOTO);
  auto goto_entry = new MethodItemEntry(goto_insn);
  IRList::iterator goto_it;
  if(idx == insn_index){
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
                                  PassManager& mgr) {

  MethodsToInline imethods;
  DexMethod *caller, *callee;
  parse_file("imethods.txt", imethods);

  for(auto map_entry : imethods){
    std::string caller_str = get_caller_str(map_entry.first);
    uint16_t insn_index    = get_insn_index(map_entry.first);
    caller = (DexMethod*) DexMethod::get_method(caller_str);

    IRInstruction *insn;
    uint16_t idx = 0, this_reg;
    IRList::iterator it, erase_it, true_block_it, 
        main_block_it, false_block_it, instanceof_it;
   
    auto caller_code = caller->get_code();
    auto caller_mc = new MethodCreator(caller);
    auto ii = InstructionIterable(caller_code);
    for(it = ii.begin().unwrap(); it != ii.end().unwrap(); it++, idx++){
      if(idx == insn_index){ 
        erase_it = it;
        true_block_it = it;
        insn = (*it).insn;
        this_reg = insn->src(0);
        main_block_it = ++it;
        break;
      }
    }

    MethodImpls method_impls = map_entry.second;
    if(method_impls.size() > 1){
      for(auto callee : method_impls){
        DexType* type = callee->get_class();
        
        //get next free register to use as destination register
        //for instance_of instruction
        auto dst_location = caller_code->get_registers_size();
        caller_code->set_registers_size(dst_location+1);

        create_instanceof(true_block_it, instanceof_it, caller_code,
                          type, this_reg, dst_location);
        create_guards(main_block_it, true_block_it, false_block_it, 
            instanceof_it, idx, insn_index, dst_location, caller_code);

        //create new invoke instruction to add to the false block
        //in order to inline the callee later
        auto invoke_insn = new IRInstruction(OPCODE_INVOKE_VIRTUAL);
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

      caller_code->erase(erase_it);
      caller_mc->create();   
    
    }
    else{
      callee = *method_impls.begin();
      inliner::inline_method(caller->get_code(), callee->get_code(), it);
      
    }
    // caller_code->build_cfg();
    // std::cout << SHOW(caller_code->cfg()) << std::endl;
  }           
  // std::cout << SHOW(caller->get_code()) << std::endl;
  std::cout << "Explicit inline pass done." << std::endl;
}

static ExplicitInlinePass s_pass;