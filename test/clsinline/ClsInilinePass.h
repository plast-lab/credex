#pragma once

#include "Pass.h"

class ClsInlinePass : public Pass
{
public:
  SamplePass() : Pass("ClsInlinePass") { }

  virtual void configure_pass(const JsonWrapper& pc) override {
  }

  virtual void eval_pass(DexStoresVector&, ConfigFiles&, PassManager&) override {
  }

  virtual void run_pass(DexStoresVector&, ConfigFiles&, PassManager&) override {
  }

};

