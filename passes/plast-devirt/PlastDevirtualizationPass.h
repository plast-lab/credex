#pragma once

#include <map>

#include "Pass.h"
#include "IRList.h"


#include "PlastDVParser.h"
#include "MethodDevirtualizer.h"

struct MethodScopeInfo {
  std::map<std::pair<std::string,std::string>, std::vector< std::pair<int, PlastMethodSpec*> >* > info;
};

struct ClassScopeInfo {
  std::map <PlastMethodSpec, MethodScopeInfo*> info;
};

class PlastDevirtualizationPass : public Pass {
  public:
    PlastDevirtualizationPass() : Pass("PlastDevirtualizationPass"){}

    virtual void run_pass(DexStoresVector&, ConfigFiles&, PassManager&) override;


  private:
    DevirtualizerMetrics m_metrics;


    void parse_instructions_m(std::vector<PlastMethodSpec*>*, std::string);
    void parse_instructions_i(std::map <std::string, ClassScopeInfo*> *, std::string);
    void scope_insert(std::map <std::string, ClassScopeInfo*>*, PlastInvocSite*, PlastMethodSpec*);

    void devirt_methods(std::vector<DexClass*>&, std::vector<PlastMethodSpec*>);
    void devirt_targets(std::vector<DexClass*>&, std::map <std::string, ClassScopeInfo*>&);
    void devirt_targets(const PlastMethodSpec& , std::map<PlastMethodSpec, MethodScopeInfo*>&, DexMethod* const&);
    void devirtualize(IRInstruction*, PlastMethodSpec*);
    void add_cast(IRInstruction*, IRCode*, PlastMethodSpec *, IRList::iterator&);
    void create_static_copy(DexClass *, const PlastMethodSpec&, DexMethodRef*&);

    void staticize_methods_using_this(const std::vector<DexClass*>&, const std::unordered_set<DexMethod*>&);
    void make_methods_static(const std::unordered_set<DexMethod*>&, bool);

    void zip_virtual_version(DexClass*, PlastMethodSpec*, DexMethodRef*);

    void reset_metrics() { m_metrics = DevirtualizerMetrics();}
};



static const char invok_site_devirt_file[] = "SingleInvocationTarget.csv";
static const char invok_method_devirt_file[] = "ReachableMethodOnlyUsedInSingleInvocationTarget.csv";
static const char statprefix[] = "static_";
