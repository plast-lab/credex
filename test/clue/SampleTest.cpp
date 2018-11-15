/**
  @file
  @brief Sample test file.

  This is a file that you can copy in order to create your own test suite.

 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/process/environment.hpp>

#include "ClueTest.h"

/**
  @fn SampleTests_sample1_Test()
  @brief A sample test case for libredex.

  The purpose of this test function is to give a guide into writing your own test functions.
  In this example, `SampleTests` is the name of the test suite and `sample1` is the name of
  the test case.
  */
TEST(SampleTests, sample1) {

  // This test is a self-contained routine, that does nothing
  ASSERT_EQ(1,1);

  // We print the environment here
  namespace bp = boost::process;

  if(const char* val = std::getenv("PYTHON"))
    std::cout << "PYTHON=" << val << std::endl;
  else
    std::cout << "No PYTHON defined" << std::endl;
  if(const char* val = std::getenv("ANDROID_SDK_ROOT") )
    std::cout << "ANDROID_SDK_ROOT=" << val << std::endl;
  else
    std::cout << "No ANDROID_SDK_ROOT defined" << std::endl;

  int result = bp::system(std::getenv("PYTHON"),"-m","androidctl","avds");
  ASSERT_EQ(0, result);  
}


class SamplePass : public Pass
{
public:
  SamplePass() : Pass("SamplePass") { }

  std::string param;

  size_t configure_calls = 0;
  size_t eval_calls = 0;
  size_t run_calls = 0;

  virtual void configure_pass(const JsonWrapper& pc) override {
    pc.get("foo", "void", param);

    configure_calls++;
  }

  virtual void eval_pass(DexStoresVector&, ConfigFiles&, PassManager&) override {
    eval_calls++;
  }

  virtual void run_pass(DexStoresVector&, ConfigFiles&, PassManager&) override {
    run_calls++;
  }

};


/*
  The purpose of this test function is to give a guide into writing your own test functions.
  In this example, the test uses fixture @ref ClueTest  and `sample2` is the name of
  the test case. A trivial, user-defined pass is then executed.
*/
TEST_F(ClueTest, sample2) {

  using std::cout;
  using std::endl;

  // This test is a self-contained routine, that loads a simple .dex file
  // and checks what it loaded

  // initially the fixtures store is empty
  ASSERT_EQ(1, stores().size());
  EXPECT_EQ("classes", root_store().get_name());

  // Load the sample-classes.dex file by calling ClueTest::load_dex()
  load_dex("sample-classes.dex");

  // Make some simple checks
  ASSERT_EQ(1, root_store().get_dexen().size());
  const DexClasses& classes = root_store().get_dexen().back();
  ASSERT_EQ(1, classes.size());
  ASSERT_EQ("Lclue/testing/SampleClass;", classes[0]->get_name()->str());

  //--------------------
  // Execute a run with a single pass
  //--------------------

  auto sample_pass = new SamplePass();
  std::vector<Pass*> passes { sample_pass };
  run_passes(passes);

  // Note that configure_pass was not called.
  // This is because the PassManager was not configured, therefore it
  // just ran each pass in array `passes` unconfigured.
  EXPECT_EQ(0, sample_pass->configure_calls);

  EXPECT_EQ(1, sample_pass->eval_calls);
  EXPECT_EQ(1, sample_pass->run_calls);

}


//----------------------
// A test with multiple executions of SimplePass. This is excercising
// the PassManager feature that allows a pass to be invoked multiple times
// in the same run.
//------------------------
TEST_F(ClueTest, sample3) {

  using std::cout;
  using std::endl;

  // Load the sample-classes.dex file by calling ClueTest::load_dex()
  load_dex("sample-classes.dex");

  auto sample_pass = new SamplePass();
  std::vector<Pass*> passes { sample_pass };

  //
  // Use the config to execute the SamplePass pass twice.
  //
  CLUETEST_CONFIG(
  {
      "redex": {
        "passes": [
            "SamplePass#1",
            "SamplePass#2"
          ]
      },
      "SamplePass#1": { // Note: you can add a comment
        "foo": "bar"
      },
      "SamplePass#2": { // Note: another comment
        "foo": "baz",
        "foof": null
      }
  }
  );

  std::cerr << config << std::endl;

  run_passes(passes);

  EXPECT_EQ(2, sample_pass->configure_calls);
  EXPECT_EQ(2, sample_pass->eval_calls);
  EXPECT_EQ(2, sample_pass->run_calls);

  // Note that the param has taken the value provided at the last activation,
  // i.e., that corresponding to "SamplePass#2"
  EXPECT_EQ("baz", sample_pass->param);

}


//----------------------
// A test similar to sample3, but with the configuration file read from a disk file.
//------------------------
TEST_F(ClueTest, sample4) {

  // Load the sample-classes.dex file by calling ClueTest::load_dex()
  load_dex("sample-classes.dex");
  load_config("sample-config.json");

  auto sample_pass = new SamplePass();
  std::vector<Pass*> passes { sample_pass };

  run_passes(passes);

  EXPECT_EQ(2, sample_pass->configure_calls);
  EXPECT_EQ(2, sample_pass->eval_calls);
  EXPECT_EQ(2, sample_pass->run_calls);
  EXPECT_EQ("baz", sample_pass->param);
}




//----------------------
// A test with all passes. 
//------------------------
TEST_F(ClueTest, sample5) {

  using std::cout;
  using std::endl;

  // Load the sample-classes.dex file by calling ClueTest::load_dex()
  load_dex("sample-classes.dex");

  auto sample_pass = new SamplePass();
  //std::vector<Pass*> passes { sample_pass };
  std::vector<Pass*> passes ; 
  for(auto& pass : PassRegistry::get().get_passes())
  	passes.push_back(pass);

  //
  // Use a reasonable config
  //
  load_config("reasonable.config");


  std::cerr << config << std::endl;

  run_passes(passes);

  // EXPECT_EQ(0, sample_pass->configure_calls);
  // EXPECT_EQ(0, sample_pass->eval_calls);
  // EXPECT_EQ(0, sample_pass->run_calls);
  // EXPECT_EQ("", sample_pass->param);

  auto tmp_path = boost::filesystem::temp_directory_path()/boost::filesystem::unique_path("credex-test-%%%%-%%%%");
  ASSERT_TRUE(create_directory(tmp_path));

  cout << "Created unique directory: " << tmp_path << endl;
  write_dexen(tmp_path.native());
}

