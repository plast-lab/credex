#include "Walkers.h"
#include "FileParser.h"

std::string FileParser::transform_type(std::string java_type){

  //remove any whitespace
  java_type.erase(remove_if(java_type.begin(), 
      java_type.end(), isspace), java_type.end());

  std::stringstream ss(java_type);
  std::string t, dalvik_type = "";
  size_t n, pos; 

  while(ss.good()){
    std::getline (ss, t, ',');
    if(!t.compare("")) {
      return dalvik_type;
    }

    n = std::count(t.begin(), t.end(), '[');
    if(n){
      pos = t.find_first_of("[");
      t = t.substr(0, pos);
      while(n--){
        dalvik_type += "[";
      }
    }
    
    if(!t.compare("byte")) {
      dalvik_type += "B";
    }
    else if(!t.compare("char")){
      dalvik_type += "C";   
    }
    else if(!t.compare("double")){
      dalvik_type += "D"; 
    }
    else if(!t.compare("float")){
      dalvik_type += "F";  
    }  
    else if(!t.compare("int")){
      dalvik_type += "I";  
    }
    else if(!t.compare("long")){
      dalvik_type += "J";  
    }
    else if(!t.compare("short")){
      dalvik_type += "S";  
    }
    else if(!t.compare("boolean")){
      dalvik_type += "Z";  
    }  
    else if(!t.compare("void")){
      dalvik_type += "V";  
    }
    else{
      dalvik_type += JavaNameUtil::external_to_internal(t);
    }       
  }

  return dalvik_type;
}

void FileParser::analyze_method(std::string method, std::string& cls, 
    std::string& name, std::string& args, std::string& ret_type)
{
  std::stringstream ss(method);

  std::getline (ss, cls, ':');
  std::getline (ss, ret_type, ' ');
  std::getline (ss, ret_type, ' ');
  std::getline (ss, name, '(');
  std::getline (ss, args, ')');  

  cls = JavaNameUtil::external_to_internal(cls);
}

std::string FileParser::convert_method(const std::string &cls, const std::string &name,
    const std::string &args, const std::string &ret_type)
{
  return cls + "." + name + ":(" + args + ")" + ret_type;
}


void FileParser::to_dalvik_format(std::string& method){
  
  size_t pos;
  std::stringstream ss(method);
  std::string cls, name, args, ret_type, 
      dalvik_args, dalvik_rtype;

  pos = method.find_last_not_of(" \t");
  method = method.substr(1, pos-1);
  analyze_method(method, cls, name, args, ret_type);
    
  dalvik_args  = transform_type(args);
  dalvik_rtype = transform_type(ret_type);

  method = convert_method(cls, name, dalvik_args, dalvik_rtype);  
}

void FileParser::analyze_invoc_id(const std::string invocation_id, 
    std::string& callsite, std::string& callee_name, uint16_t& offset)
{
  std::string off;
  std::stringstream ss(invocation_id);
  std::getline(ss, callsite, '/');
  std::getline(ss, callee_name, '/');
  std::getline(ss, off, '\n');

  offset = atoi(off.c_str());
}

void FileParser::find_def(const std::string& mstr, DexMethod*& method)
{
  method = nullptr;
  auto ref = DexMethod::get_method(mstr);
 
  if(ref) method = resolve_method(ref, MethodSearch::Any);   
}

//sort callers to perform bottom-up inlining

void FileParser::sort_callers(const ctc& callee_to_callers,
                              const ctc& caller_to_callees,
                              std::vector<DexMethod*>& sorted_callers) 
{
  for (auto it : caller_to_callees) {
    auto caller = it.first;
    //first fnd top level callers
    if (callee_to_callers.find(caller) != callee_to_callers.end()) continue;

    std::unordered_set<DexMethod*> visited;
    visited.insert(caller);
    sort_callers(caller, caller_to_callees, it.second, visited, sorted_callers);
    auto vec_it = std::find(sorted_callers.begin(), sorted_callers.end(), caller);
    if (vec_it == sorted_callers.end()) {
      sorted_callers.push_back(caller);
    }
  }
}

void FileParser::sort_callers(DexMethod* caller,
                              const ctc& caller_to_callees,
                              const std::unordered_set<DexMethod*>& callees,
                              std::unordered_set<DexMethod*>& visited,
                              std::vector<DexMethod*>& sorted_callers) 
{
  for (auto callee : callees) {
    // if the call chain hits a call loop, ignore and keep going
    if (visited.count(callee) > 0) {
      // std::cout << "cycle detected " << callee->get_deobfuscated_name() <<std::endl; 
      continue;
    }

    auto ctc_it = caller_to_callees.find(callee);
    if (ctc_it != caller_to_callees.end()) {
      visited.insert(callee);
      sort_callers(callee, caller_to_callees, ctc_it->second, visited, sorted_callers);
      visited.erase(callee);
      auto vec_it = std::find(sorted_callers.begin(), sorted_callers.end(), callee);
      if (vec_it == sorted_callers.end()) {
        sorted_callers.push_back(callee);
      }
    }
  }
}

void FileParser::parse_file(const std::string filename, Inlinables& imethods,
    std::vector<DexMethod*>& sorted_callers)
{
  std::ifstream file(filename);
  ctc callee_to_callers, caller_to_callees;

  if(file.is_open()){
    while(!file.eof()){
      uint16_t offset;
      std::string method, line, invocation_id, 
          callee_name, caller_str;

      std::getline(file, line);
      //ignore empty lines
      if(line.length() > 0) {
        
        DexMethod* callee, *caller; 
        std::stringstream ss(line);
        std::getline (ss, invocation_id, '\t');
        std::getline (ss, method, '\n'); 
        IRList::iterator it;

        analyze_invoc_id(invocation_id, caller_str, callee_name, offset);
        to_dalvik_format(method);
        to_dalvik_format(caller_str);
        find_def(method, callee);
        find_def(caller_str, caller);   

        /**
         * if the caller or callee method reference does not map 
         * to a known definition ignore it and don't inline.
         */
        if(callee ==  nullptr || caller == nullptr) continue;

        /*ignore recursive calls*/
        // if(caller->get_deobfuscated_name() == callee->get_deobfuscated_name()){
        //   std::cout << "recursive call = " << callee->get_deobfuscated_name() << std::endl;
        //   continue; 
        // }

        if(!callee->get_code()){
          // std::cout << "callee with no code = " << callee->get_deobfuscated_name() << std::endl;
          continue;
        }
      
        uint16_t idx = 0;
        auto caller_code = caller->get_code();
        auto ii = InstructionIterable(caller_code);

        for(auto ii_it = ii.begin(); ii_it != ii.end(); ii_it++){
          if(is_invoke(ii_it->insn->opcode())){
            
            std::string meth, name;
            name = ii_it->insn->get_method()->c_str();
            meth = ii_it->insn->get_method()->get_class()->get_name()->str();
            meth = JavaNameUtil::internal_to_external(meth) + "." + name;

            if(callee->c_str() == name && idx++ == offset){
              // std::cout << "mie = " << *ii_it << std::endl;
              // imethods[caller][*(ii_it->insn)].insert(callee);

              imethods[caller][(*ii_it)].insert(callee);
              // std::cout << "ii_it = " << *ii_it << std::endl;
              callee_to_callers[callee].insert(caller);  
              caller_to_callees[caller].insert(callee);
              break;
            }
          }
        }          
      }
    }
    sort_callers(callee_to_callers, caller_to_callees, sorted_callers);
  }
  else{ 
    std::cout << "Could not open file " << filename << std::endl;
  }
}
