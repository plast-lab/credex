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

void FileParser::find_def(const std::string& mstr, DexMethod*& method, 
    DexStoresVector& stores)
{
  // std::cout << "method = " << method << std::endl;
  method = nullptr;
  auto scope = build_class_scope(stores);
  walk::methods(scope, [&](DexMethod* m) {
    // std::cout << "m = " << m->get_deobfuscated_name() << std::endl;
    if(mstr == m->get_deobfuscated_name()){
      std::cout << "found " << mstr << std::endl;
      method = m;  
      return;
    }
  });
}

void FileParser::parse_file(const std::string filename, MethodsToInline& imethods,
    DexStoresVector& stores)
{
  std::ifstream file(filename);
  
  if(file.is_open()){
    while(!file.eof()){
      std::string method, caller, line, invocation_id, callee_name;
      uint16_t offset;
      std::getline(file, line);
      
      //if line is empty keep reading
      while(line.length() == 0) {
        std::getline(file, line);
      }
      
      std::stringstream ss(line);
      std::getline (ss, invocation_id, '\t');
      std::getline (ss, method, '\n');      
      to_dalvik_format(method);

      // caller += std::to_string(offset);
      if(imethods.find(invocation_id) == imethods.end()){
        MethodImpls method_impls;
        imethods.insert(std::make_pair(invocation_id, method_impls));
      }
      DexMethod* callee; 
      find_def(method, callee, stores);
      imethods[invocation_id].insert(callee);
    }
  }
  else{ 
    std::cout << "Could not open file " << filename << std::endl;
  }
}
