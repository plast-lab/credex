#pragma once

#include "Pass.h"
#include "DexClass.h"

class RemoverPass : public Pass {
 public:
  RemoverPass() : Pass("RemoverPass") {}

  virtual void run_pass(DexStoresVector&, ConfigFiles&, PassManager&) override;
};
