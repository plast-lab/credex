#pragma once

#include "Pass.h"
#include "IRList.h"
#include "DexClass.h"
#include "FileParser.h"

class ExplicitInlinePass : public Pass {
public:
  ExplicitInlinePass() : Pass("ExplicitInlinePass") {}

  virtual void run_pass(DexStoresVector&, ConfigFiles&, PassManager&) override;
  virtual void eval_pass(DexStoresVector&, ConfigFiles&, PassManager&) override;

private:
  void create_guards(IRList::iterator&, IRList::iterator&, IRList::iterator&, 
  		IRList::iterator&, bool, const uint16_t, IRCode*&);

  void create_instanceof(IRList::iterator&, IRList::iterator&, IRCode*&,
      	DexType*&, const uint16_t, const uint16_t);

public:
  struct InlineStats
  {
  	uint64_t callsites{0};
    uint64_t total_meths{0};
    uint64_t need_guards{0};
    uint64_t invoke_super{0};
    uint64_t invoke_static{0};
    uint64_t invoke_direct{0};
    uint64_t invoke_virtual{0};
    uint64_t devirtualizable{0};
    uint64_t invoke_interface{0};  	
  };

  InlineStats inline_stats;

private:
	Inlinables imethods;
	std::vector<DexMethod*> sorted_callers;
};
