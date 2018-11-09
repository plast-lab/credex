#include "IRCode.h"
#include "DexUtil.h"
#include "DexClass.h"
#include "Resolver.h"

struct class_type_comparator {
  bool operator()(const DexMethod* meth1, const DexMethod* meth2) const {
    auto cls_type1 = type_class(meth1->get_class());
	  auto cls_type2 = type_class(meth2->get_class());
	  auto cls1 = cls_type1->get_super_class();
	  auto cls2 = cls_type2->get_type();
	  auto obj_type = get_object_type();

	  while(cls1 != obj_type && cls1 != nullptr){
	  	if(cls1->c_str() == cls2->c_str()){
	      return true;
	    }
	    cls_type1 = type_class(cls1);
	    if(cls_type1 == nullptr)	return false;
	    cls1 = cls_type1->get_super_class();
	  }
	  return false;
	}
};

struct mie_equal
{
	bool operator()(const MethodItemEntry& mie1, const MethodItemEntry& mie2) const
	{
		return mie1.insn == mie2.insn;
	}

};

struct mie_hasher
{

  std::uint64_t operator()(const MethodItemEntry& mie) const
  {
  	auto insn = mie.insn;
  	std::vector<uint64_t> bits;
	  bits.push_back(insn->opcode());

	  for (size_t i = 0; i < insn->srcs_size(); i++) {
	    bits.push_back(insn->src(i));
	  }

	  if (insn->dests_size() > 0) {
	    bits.push_back(insn->dest());
	  }

	  if (insn->has_data()) {
	    size_t size = insn->get_data()->data_size();
	    const auto& data = insn->get_data()->data();
	    for (size_t i = 0; i < size; i++) {
	      bits.push_back(data[i]);
	    }
	  }

	  if (insn->has_type()) {
	    bits.push_back(reinterpret_cast<uint64_t>(insn->get_type()));
	  }
	  if (insn->has_field()) {
	    bits.push_back(reinterpret_cast<uint64_t>(insn->get_field()));
	  }
	  if (insn->has_method()) {
	    bits.push_back(reinterpret_cast<uint64_t>(insn->get_method()));
	  }
	  if (insn->has_string()) {
	    bits.push_back(reinterpret_cast<uint64_t>(insn->get_string()));
	  }
	  if (insn->has_literal()) {
	    bits.push_back(insn->get_literal());
	  }

	  uint64_t result = 0;
	  for (uint64_t elem : bits) {
	    result ^= elem;
	  }
	    return result;
  }


};

using MethodImpls = std::set<DexMethod*, class_type_comparator>;
using MethodsToInline = std::unordered_map<MethodItemEntry, MethodImpls, mie_hasher, mie_equal>;
using Inlinables = std::unordered_map<DexMethod*, MethodsToInline>;
using ctc = std::unordered_map<DexMethod*, std::unordered_set<DexMethod*>>;

class FileParser{  	 	

	static void to_dalvik_format(std::string&);

	static std::string transform_type(std::string);

	static void find_def(const std::string&, DexMethod*&);

	static void analyze_method(std::string, std::string&, std::string&, 
			std::string&, std::string&);

	static std::string convert_method(const std::string&, const std::string&,
    	const std::string&, const std::string&);	

	static void analyze_invoc_id(const std::string, std::string&, std::string&, uint16_t&);

	static void sort_callers(const ctc&, const ctc&, std::vector<DexMethod*>&);

  static void sort_callers(DexMethod*, const ctc&, const std::unordered_set<DexMethod*>&,
   		std::unordered_set<DexMethod*>&, std::vector<DexMethod*>&);

public:

	static void parse_file(const std::string, Inlinables&, std::vector<DexMethod*>&);

};

