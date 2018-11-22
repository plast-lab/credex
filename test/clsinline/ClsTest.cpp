
#include "ClueTest.h"
#include "ClsInlinePass.h"

ClsInlinePass cls_inline_pass;

TEST_F(ClueTest, prerequisites)
{
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

