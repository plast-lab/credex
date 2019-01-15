
#include "DexClass.h"
#include "TypeSystem.h"
#include "IRCode.h"
#include "IRTypeChecker.h"

#include "ClsInliner.h"

using namespace clsinliner;

using std::cout;
using std::endl;

BasicInlineSpec::BasicInlineSpec(const JsonWrapper& jspec)
{
    host_name = jspec.get("host", std::string(""));
    inlined_name = jspec.get("inlined", std::string(""));
    inlined_attr = jspec.get("inlined_attr", std::string(""));
    
    TRACE(MAIN, 1, "Class Inlining %s into %s attr %s\n",
	  inlined_name.c_str(), host_name.c_str(), inlined_attr.c_str());
}

Inliner* BasicInlineSpec::get_inliner() const
{
	// Get classes
	DexClass* host_class = type_class(DexType::get_type(host_name));
	DexClass* inlined_class = type_class(DexType::get_type(inlined_name));

	// find inlined attribute
	DexField* inlined_field = host_class->find_field(inlined_attr.c_str(), inlined_class->get_type());

	return new BasicInliner { host_class, inlined_class, inlined_field } ;
}


TypeVector ancestors_of(DexClass* cls)
{
	TypeVector anc;
	if(cls == nullptr) return anc;
	anc.push_back(cls->get_type());
	while(cls != nullptr) {
		DexType* t = cls->get_super_class();
		if(t != nullptr) {
			cls = type_class(t);
			anc.push_back(t);
		} else {
			break;
		}
	}
	std::reverse(anc.begin(), anc.end());
	return anc;
}

void BasicInliner::resolve_superclasses()
{      
	// build superclass list
	TypeVector hanc = ancestors_of(host);
	TypeVector ianc = ancestors_of(inlined);

	// find longest common prefix
	auto hi = hanc.begin();
	auto ii = ianc.begin();
	while(1) {
		if(hi==hanc.end()) break;
		if(ii==ianc.end()) break;

		if(*hi != *ii) break;

		++hi;
		++ii;
	}
	if(hi==hanc.end())
		throw ConstraintViolation("Host is a superclass of inlined");
	if(ii==ianc.end())
		throw ConstraintViolation("Inlined is a superclass of host");

	std::copy(hanc.begin(), hi, std::back_inserter(common_superclasses));
	hanc.erase(hanc.begin(), hi);
	host_superclasses = std::move(hanc);

	ianc.erase(ianc.begin(), ii);
	for(auto t : ianc) {
		DexClass* c = type_class(t);
		always_assert(c != nullptr);
		inlined_superclasses.push_back(c);
	}
}


DexField* BasicInliner::inject_attribute(DexField* fld)
{
	// TODO: add assertion that fld belongs to injected
	
	// Create a field ref , get DexFieldRef*
	auto newfldref = DexField::make_field(host->get_type(),
					      fld->get_name(),
					      fld->get_type());

	// Cast it to DexField*. This is done in the
	// loader code in libredex, so I am assuming it is
	// part of the API
	auto newfld = static_cast<DexField*>(newfldref);
	
	// record new field
	replaced_field[fld] = newfld;
	injected_fields.push_back(newfld);
	
	// Make field concrete
	newfld->make_concrete(fld->get_access());
	
	// Add to the host class
	host->add_field(newfld);
	
	return newfld;
}


DexMethod* BasicInliner::inject_method(DexMethodRef* m)
{
	if(! m->is_def())
		throw ConstraintViolation("Injected class has undefined method");
	
	DexMethod* meth = static_cast<DexMethod*>(m);
	DexMethod* nmeth = DexMethod::make_method_from(meth,
						       host->get_type(),
						       m->get_name());
	host->add_method(nmeth);
	replaced_method[meth] = nmeth;	
	injected_vmethods.push_back(nmeth);
	
	return nmeth;
}


void BasicInliner::inject_inlined_constructor()
{

}


void BasicInliner::rewrite_host_constructors()
{
	for(auto cons : host->get_dmethods()) {
		if(is_constructor(cons) && cons->get_name()->str()=="<init>") {
			// algorithm to rewrite a constructor:
			// (a) locate the v<n> = new Inlined  insn

			
			
			// (b) locate the foo = v<m> insn

			//     check that m==n !!!
			// (c) remove the "new" invocation
			// (d) remove the foo = invocation
			// (e) find call to Inlined.<init>(v<n>, ...) and replace with
			//     call to $inlined$init(this, ...)
			
		}	      
	}
}



void BasicInliner::rewrite_injected(DexMethod* inj_meth)
{
	auto inj_code = inj_meth->get_code();

	// rewrite code to replace all references to replaced fields and methods
	for(auto mie : InstructionIterable(inj_code)) {
		auto insn = mie.insn;

		if(insn->has_field()) {
			auto fiter = replaced_field.find(insn->get_field());
			if(fiter!=replaced_field.end()) {
				insn->set_field(fiter->second);
			}
		}

		if(insn->has_method()) {
			auto miter = replaced_method.find(insn->get_method());
			if(miter != replaced_method.end()) {
				insn->set_method(miter->second);
			}
		}
	}

	IRTypeChecker checker(inj_meth);
	checker.run();
	always_assert(checker.good());	
}

void BasicInliner::rewrite_host_methods()
{
	
}



void BasicInliner::run()
{
	//
	// These steps comprise the class inlining transformation
	//

        // Precomupte superclass info
	resolve_superclasses();

	// Add attributes to host
	for(auto fld : inlined->get_ifields()) {
		inject_attribute(fld);
	}

	// Add methods to host
	for(auto meth : inlined->get_vmethods()) {
		DexMethod* nmeth = inject_method(meth);
		TRACE(PM,1,"Added method %s\n",nmeth->c_str());	       
	}


	// Add static method to host that constructs the inlined attrs
	inject_inlined_constructor();

	// Rewrite injected methods: each reference to a replaced field or
	// method is replaced.
	for(auto inj_meth : injected_vmethods)
		rewrite_injected(inj_meth);
	 
	// Also, rewrite  injected constructors
	
	// Rewrite host constructors to call the inlined constructor
	rewrite_host_constructors();

	// Rewrite host methods to replace operations on inlined_attr
	rewrite_host_methods();
}
