#pragma once

#include "Pass.h"

class ClsInlinePass;

/// Abstract base class to perform an inlining
class Inliner
{
public:
    virtual void run() =0;
    virtual ~Inliner() { }
};

/// Abstract base class that describes an inlining
class InlineSpec
{
public:
    virtual Inliner* get_inliner() const =0;
    virtual ~InlineSpec() { }
};


class ClsInlinePass : public Pass
{
public:
    ClsInlinePass() : Pass("ClsInlinePass"), spec(nullptr) { }
    ~ClsInlinePass();
    
    virtual void configure_pass(const JsonWrapper& pc) override;


    virtual void eval_pass(DexStoresVector&, ConfigFiles&, PassManager&) override;
    virtual void run_pass(DexStoresVector&, ConfigFiles&, PassManager&) override;
	
private:
    void clear();
    
    InlineSpec* spec;
};


