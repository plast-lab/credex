#include "IRCode.h"
#include "DexUtil.h"
#include "DexClass.h"

static bool compare_class_types(const DexMethod* meth1, const DexMethod* meth2){
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


class FileParser{

public:	

	static void to_dalvik_format(std::string&);

	static std::string transform_type(std::string);

	static void analyze_method(std::string, std::string&, std::string&, 
			std::string&, std::string&);

	static void find_def(const std::string&, DexMethod*&, DexStoresVector&);

	static std::string convert_method(const std::string&, const std::string&,
    	const std::string&, const std::string&);	

	static void parse_file(const std::string, MethodsToInline&, DexStoresVector&);

	static void analyze_invoc_id(const std::string, std::string&, std::string&, uint16_t&);

};

