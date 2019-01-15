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

struct localDevirtutalizerMetrics {
  uint32_t num_invok_virtual{0};
  uint32_t num_new_methods{0};
  uint32_t num_extra_opcodes{0};
  uint32_t num_total_invocations{0};
  uint32_t num_total_methods{0};
  uint32_t num_invok_interface{0};
  uint32_t num_total_interface{0};
};


class PlastDevirtualizationPass : public Pass {
  public:
    PlastDevirtualizationPass(): Pass("PlastDevirtualizationPass"),
    only_devirtualize_invok_interface(false),zip_vv(false){}

    virtual void run_pass(DexStoresVector&, ConfigFiles&, PassManager&) override;

    bool only_devirtualize_invok_interface;
    bool zip_vv;

  private:
    DevirtualizerMetrics m_metrics;
    localDevirtutalizerMetrics l_metrics;

    void parse_instructions_m(std::vector<PlastMethodSpec*>*, std::string);
    void parse_instructions_i(std::map <std::string, ClassScopeInfo*> *, std::string);
    void scope_insert(std::map <std::string, ClassScopeInfo*>*, PlastInvocSite*, PlastMethodSpec*);

    void devirt_methods(std::vector<DexClass*>&, std::vector<PlastMethodSpec*>);
    void devirt_targets(std::vector<DexClass*>&, std::map <std::string, ClassScopeInfo*>&);
    void devirt_targets(const PlastMethodSpec& , std::map<PlastMethodSpec, MethodScopeInfo*>&, DexMethod* const&);
    void devirtualize(IRInstruction*, PlastMethodSpec*);
    void add_cast(IRInstruction*, IRCode*, PlastMethodSpec *, IRList::iterator&);
    void create_static_copy(DexClass *, const PlastMethodSpec&, DexMethodRef*&,
      bool ii = true);

    void staticize_methods_using_this(const std::vector<DexClass*>&, const std::unordered_set<DexMethod*>&);
    void make_methods_static(const std::unordered_set<DexMethod*>&, bool);

    void zip_virtual_version(DexClass*, PlastMethodSpec*, DexMethodRef*);
    void gather_statistics(const std::vector<DexClass*>&);

    void reset_metrics() { m_metrics = DevirtualizerMetrics();}
};



static const char invok_site_devirt_file[] = "SingleInvocationTarget.csv";
static const char invok_method_devirt_file[] = "ReachableMethodOnlyUsedInSingleInvocationTarget.csv";
static const char statprefix[] = "static_";
