
#include <iostream>

#include <strstream>
#include "ClsInlinePass.h"
#include "Trace.h"

using std::cout;
using std::endl;

void ClsInlinePass::configure_pass(const JsonWrapper& pc)
{
	host_name = pc.get("host", std::string(""));
	inlined_name = pc.get("inlined", std::string(""));

	TRACE(MAIN, 1, "Class Inlining %s into %s\n",
	      host_name.c_str(), inlined_name.c_str());
	
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
	
}
