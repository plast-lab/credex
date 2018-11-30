
#include <iostream>
#include "ClsInlinePass.h"
#include "Trace.h"
#include "Resolver.h"

#include <gtest/gtest.h>

using std::cout;
using std::endl;

void ClsInlinePass::configure_pass(const JsonWrapper& pc)
{
	host_name = pc.get("host", std::string(""));
	inlined_name = pc.get("inlined", std::string(""));
	inlined_attr = pc.get("inlined_attr", std::string(""));

	TRACE(MAIN, 1, "Class Inlining %s into %s attr %s\n",
	      inlined_name.c_str(), host_name.c_str(), inlined_attr.c_str());

}

void ClsInlinePass::eval_pass(DexStoresVector&, ConfigFiles&, PassManager&)
{
	// Locate host and inlined class
	DexType* host = DexType::get_type(host_name);
	DexClass* hostcls = type_class(host);

	for(auto meth : hostcls->get_dmethods()) {
		cout << "dmethod " << meth->get_fully_deobfuscated_name() << endl;
	}

	for(auto meth : hostcls->get_vmethods()) {
		cout << "vmethod " << meth->get_fully_deobfuscated_name() << endl;
	}

}

void ClsInlinePass::run_pass(DexStoresVector&, ConfigFiles&, PassManager&)
{
	// Get classes
	DexClass* host_class = type_class(DexType::get_type(host_name));
	ASSERT_NE(nullptr, host_class);
	DexClass* inlined_class = type_class(DexType::get_type(inlined_name));
	ASSERT_NE(nullptr, inlined_class);

	// (a) find inlined attribute
	DexField* inlined_field = host_class->find_field(inlined_attr.c_str(), inlined_class->get_type());
	ASSERT_NE(nullptr, inlined_field);

	// (b) add attributes of inlined class
	for(auto fld : inlined_class->get_ifields()) {
		auto newfldref = DexField::make_field(host_class->get_type(),
			fld->get_name(), fld->get_type());
		auto newfld = static_cast<DexField*>(newfldref);
		ASSERT_NE(nullptr, newfld);
		newfld->make_concrete(fld->get_access());
		host_class->add_field(newfld);
	}

   // (c) add methods from inlined into host


}
