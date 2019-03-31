#include <cstdint>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <gtest/gtest.h>

#include "DexAsm.h"
#include "DexClass.h"
#include "Creators.h"
#include "ClueTest.h"
#include "PlastDevirtualizationPass.h"

/*
void write_file(std::string invocation_site,
                std::string method_desc,
                std::ios_base::openmode mode){
  std::ofstream imethods;
  imethods.open ("../../imethods.txt", mode);
  imethods << invocation_site << "\t" << method_desc;
  imethods.close();
}




TEST_F(ClueTest, simpleTest){

  std::string invocation_site, method_desc;

  invocation_site = "<clue.testing.SimpleTest: void caller()>/clue.testing.SimpleTest.callee/0";
  method_desc = "<clue.testing.SimpleTest: int callee(int, int)>";

  load_dex("simpleTest.dex");
  write_file(invocation_site, method_desc, std::ofstream::trunc);

  auto explicit_inline_pass = new ExplicitInlinePass();
  std::vector<Pass*> passes { explicit_inline_pass };

  run_passes(passes);

}

TEST_F(ClueTest, recursionTest){

  std::string invocation_site, method_desc;

  load_dex("recursionTest.dex");

  invocation_site = "<clue.testing.RecursionTest: void A()>/clue.testing.RecursionTest.A/0";
  method_desc = "<clue.testing.RecursionTest: void A()>";
  write_file(invocation_site, method_desc, std::ofstream::trunc);

  auto explicit_inline_pass = new ExplicitInlinePass();
  std::vector<Pass*> passes { explicit_inline_pass };

  run_passes(passes);

}

TEST_F(ClueTest, guardTest){

  std::string invocation_site, method_desc;

  load_dex("simpleTest.dex");
  load_dex("guardTest.dex");

  invocation_site = "<clue.testing.GuardTest: void caller()>/clue.testing.GuardTest.callee/0";
  method_desc = "<clue.testing.GuardTest: int callee(int, int)>";
  write_file(invocation_site, method_desc, std::ofstream::trunc);
  invocation_site = "\n<clue.testing.GuardTest: void caller()>/clue.testing.GuardTest.callee/0";
  method_desc = "<clue.testing.SimpleTest: int callee(int, int)>";
  write_file(invocation_site, method_desc, std::ofstream::app);

  auto explicit_inline_pass = new ExplicitInlinePass();
  std::vector<Pass*> passes { explicit_inline_pass };

  run_passes(passes);

}
*/



TEST_F(ClueTest, simpleTest){

  EXPECT_EQ(2, 2);
  load_dex("simpleTest.dex");

  auto plast_devirt_pass = new PlastDevirtualizationPass();
  std::vector<Pass*> passes { plast_devirt_pass };

  run_passes(passes);

}
