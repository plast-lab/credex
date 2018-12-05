
#include <iostream>
#include <memory>

#include "ClsInlinePass.h"
#include "ClsInliner.h"
#include "Trace.h"
#include "Resolver.h"

#include <gtest/gtest.h>

using std::cout;
using std::endl;


ClsInlinePass::~ClsInlinePass()
{
    clear();
}

void ClsInlinePass::clear()
{
    if(spec) {
	delete spec;
	spec = nullptr;
    }
}

void ClsInlinePass::configure_pass(const JsonWrapper& pc)
{
    clear();

    // 2DO:  This is trivial for now!
    spec = new clsinliner::BasicInlineSpec(pc);
}

void ClsInlinePass::eval_pass(DexStoresVector&, ConfigFiles&, PassManager&)
{

}



void ClsInlinePass::run_pass(DexStoresVector&, ConfigFiles&, PassManager&)
{
    auto inliner = std::unique_ptr<Inliner>( spec->get_inliner() );
    inliner->run();
}
