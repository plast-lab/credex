#pragma once

#include "Pass.h"
#include "DexClass.h"

class ExplicitInlinePass : public Pass {
 public:
  ExplicitInlinePass() : Pass("ExplicitInlinePass") {}

  virtual void run_pass(DexStoresVector&, ConfigFiles&, PassManager&) override;
};
