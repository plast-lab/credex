#pragma once

#include "Pass.h"
#include "DexClass.h"

class ExplicitInlinePass : public Pass {
 public:
  ExplicitInlinePass() : Pass("ExplicitInlinePass") {}

  virtual void run_pass(DexStoresVector&, ConfigFiles&, PassManager&) override;

  void create_guards(IRList::iterator&, IRList::iterator&, IRList::iterator&, 
  		IRList::iterator&, uint16_t, const uint16_t, const uint16_t, IRCode*&);

  void create_instanceof(IRList::iterator&, IRList::iterator&, IRCode*&,
      DexType*&, const uint16_t, const uint16_t);
};
