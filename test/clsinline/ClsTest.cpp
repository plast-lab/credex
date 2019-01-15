
#include "ClueTest.h"
#include "ClsInlinePass.h"
#include "ClsInliner.h"


void check_ancestors(DexClass* cls, const std::vector<std::string>& ancs)
{
    TypeVector anc = ancestors_of(cls);

    ASSERT_EQ(ancs.size(), anc.size());
    for(size_t i=0;i<ancs.size();i++)
	ASSERT_EQ(ancs[i], anc[i]->str());
}


TEST_F(ClueTest, utilities)
{
    load_dex("trivial.dex");
    load_config("trivial-test.config");

    auto host_name = config["ClsInlinePass"]["host"].asString();
    auto inlined_name = config["ClsInlinePass"]["inlined"].asString();
    auto inlined_attr = config["ClsInlinePass"]["inlined_attr"].asString();

    DexClass* host_class = type_class(DexType::get_type(host_name));
    ASSERT_NE(nullptr, host_class);
    
    DexClass* inlined_class = type_class(DexType::get_type(inlined_name));
    ASSERT_NE(nullptr, inlined_class);

    DexField* inlined_field = host_class->find_field(inlined_attr.c_str(), inlined_class->get_type());
    ASSERT_NE(nullptr, inlined_field);
    
    check_ancestors(host_class, {
	    std::string("Ljava/lang/Object;"),
		"Lclsinline/trivial/HostClass;"
		});

    check_ancestors(inlined_class, {
	    std::string("Ljava/lang/Object;"),
		inlined_name
		});

    
}


ClsInlinePass cls_inline_pass;

TEST_F(ClueTest, prerequisites)
{
	outdir = TempDir(".");
	outdir.keep();
	
	load_dex("trivial.dex");
	load_config("trivial-test.config");


	// Collect all registered passes
	std::vector<Pass*> passes ;
	for(auto& pass : PassRegistry::get().get_passes())
		passes.push_back(pass);
	
	passes.push_back(&cls_inline_pass);
	
	run_passes(passes);
	write_dexen();
	
	AndroidTest atst;
	atst.dexen.push_back(outdir);
	atst.dexen.push_back("trivial-test.dex");
	EXPECT_TRUE(atst("clsinline.trivial.TrivialTest"));  
}

