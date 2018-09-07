#pragma once

#include <map>

#include "Pass.h"
#include "IRList.h"


#include "PlastDVParser.h"

struct MethodScopeInfo {
  std::map<std::string, std::vector< std::pair<int, PlastMethodSpec*> >* > info;
};

struct ClassScopeInfo {
  std::map <PlastMethodSpec, MethodScopeInfo*> info;
};

class PlastDevirtualizationPass : public Pass {
  public:
    PlastDevirtualizationPass() : Pass("PlastDevirtualizationPass"){}

  virtual void run_pass(DexStoresVector&, ConfigFiles&, PassManager&) override;

  
  private:
  void parse_instructions(std::map <std::string, ClassScopeInfo*> *, std::string);
  void scope_insert(std::map <std::string, ClassScopeInfo*>*, PlastInvocSite*, PlastMethodSpec*);

  void devirt_targets(std::vector<DexClass*>&, std::map <std::string, ClassScopeInfo*>&);
  void devirt_targets(const PlastMethodSpec& , std::map<PlastMethodSpec, MethodScopeInfo*>&, DexMethod* const&);
  void devirtualize(IRInstruction*, PlastMethodSpec*);
  void add_cast(IRInstruction*, IRCode*, PlastMethodSpec *, IRList::iterator&);
  void create_static_copy(DexClass *, const PlastMethodSpec&, DexMethodRef*&); 



};


static const char statprefix[] = "stat_";