#include <cstdint>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <gtest/gtest.h>

#include "DexAsm.h"
#include "DexClass.h"
#include "Creators.h"
#include "ClueTest.h"
#include "ExplicitInlinePass.h"

void write_file(std::string invocation_site, 
                std::string method_desc,
                std::ios_base::openmode mode){
  std::ofstream imethods;
  imethods.open ("../../imethods.txt", mode);
  imethods << invocation_site << "\t" << method_desc;
  imethods.close();
}


// <android.icu.impl.ICUResourceBundle$4: android.icu.impl.ICUResourceBundle load()>/java.lang.StringBuilder.<init>/1  <java.lang.StringBuilder: void <init>()>


// TEST_F(ClueTest, simpleTest) {

//   using std::cout;
//   using std::endl;
//   using namespace dex_asm;

//   write_file();
//   // load_config("test_config.json");

//   auto cls_type = DexType::make_type(DexString::make_string("LtestClass;"));
//   ClassCreator creator(cls_type);
//   creator.set_super(get_object_type());
//   auto cls = creator.create();

//   auto callee = static_cast<DexMethod*>(DexMethod::make_method(
//       "LtestClass;", "testCallee", "I", {"I", "I"}));
//   callee->make_concrete(ACC_PUBLIC, true);
//   callee->set_code(std::make_unique<IRCode>(callee, 0));

//   auto callee_code = callee->get_code();
//   callee_code->push_back(dasm(OPCODE_ADD_INT, {1_v, 1_v, 2_v}));
//   callee_code->push_back(dasm(OPCODE_RETURN, {1_v}));

//   cls->add_method(callee);

//   //ensure everything is as supposed to in the callee code
//   auto it = InstructionIterable(callee_code).begin();
//   EXPECT_EQ(*it->insn, *dasm(IOPCODE_LOAD_PARAM_OBJECT, {0_v}));
//   ++it;
//   EXPECT_EQ(*it->insn, *dasm(IOPCODE_LOAD_PARAM, {1_v}));
//   ++it;
//   EXPECT_EQ(*it->insn, *dasm(IOPCODE_LOAD_PARAM, {2_v}));
//   ++it;
//   EXPECT_EQ(*it->insn, *dasm(OPCODE_ADD_INT, {1_v, 1_v, 2_v}));  
//   ++it;
//   EXPECT_EQ(*it->insn, *dasm(OPCODE_RETURN, {1_v}));
//   EXPECT_EQ(callee_code->get_registers_size(), 3);


//   auto caller = static_cast<DexMethod*>(
//       DexMethod::make_method("LtestClass;", "testCaller", "V", {}));
//   caller->make_concrete(ACC_PUBLIC | ACC_STATIC, false);
//   caller->set_code(std::make_unique<IRCode>(caller, 3));

//   auto caller_code = caller->get_code();
//   caller_code->push_back(dasm(OPCODE_NEW_INSTANCE, DexType::make_type("LtestClass;")));
//   caller_code->push_back(dasm(IOPCODE_MOVE_RESULT_PSEUDO_OBJECT, {0_v}));
//   caller_code->push_back(dasm(OPCODE_CONST, {1_v, 1_L}));
//   caller_code->push_back(dasm(OPCODE_CONST, {2_v, 2_L}));
//   caller_code->push_back(dasm(OPCODE_INVOKE_VIRTUAL, callee, {0_v, 1_v, 2_v}));
//   auto invoke_it = std::prev(caller_code->end());
//   caller_code->push_back(dasm(OPCODE_RETURN_VOID));

//   //ensure everything is as supposed to in the caller code
//   it = InstructionIterable(caller_code).begin();
//   EXPECT_EQ(*it->insn, *dasm(OPCODE_NEW_INSTANCE, DexType::make_type("LtestClass;")));
//   ++it;
//   EXPECT_EQ(*it->insn, *dasm(IOPCODE_MOVE_RESULT_PSEUDO_OBJECT, {0_v}));
//   ++it;
//   EXPECT_EQ(*it->insn, *dasm(OPCODE_CONST, {1_v, 1_L}));
//   ++it;
//   EXPECT_EQ(*it->insn, *dasm(OPCODE_CONST, {2_v, 2_L}));  
//   ++it;
//   EXPECT_EQ(*it->insn, *dasm(OPCODE_INVOKE_VIRTUAL, callee, {0_v, 1_v, 2_v}));
//   ++it;
//   EXPECT_EQ(*it->insn, *dasm(OPCODE_RETURN_VOID));
//   EXPECT_EQ(callee_code->get_registers_size(), 3);



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
