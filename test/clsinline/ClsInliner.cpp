
#include "DexClass.h"
#include "TypeSystem.h"
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


void BasicInliner::inject_attributes()
{
	// This will only process fields in the inlined class
	
	for(auto fld : inlined->get_ifields())
	{
		// Create a field ref , get DexFieldRef*
		auto newfldref = DexField::make_field(host->get_type(),
			fld->get_name(), fld->get_type());

		// Cast it to DexField*. This is done in the
		// loader code in libredex, so I am assuming it is
		// part of the API
		auto newfld = static_cast<DexField*>(newfldref);

		// record new field
		injected_fields.push_back(newfld);

		// Make field concrete
		newfld->make_concrete(fld->get_access());

		// Add to the host class
		host->add_field(newfld);
	}
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
	return nmeth;
}



void BasicInliner::inject_methods()
{
	for(auto meth : inlined->get_vmethods()) {
		DexMethod* nmeth = inject_method(meth);
		injected_vmethods.push_back(nmeth);

		TRACE(PM,1,"Added method %s\n",nmeth->c_str());
		
		IRTypeChecker checker(nmeth);
		checker.run();
		
		cout << "Type checker: " << checker.what() << endl;
		cout << checker << endl;
		
		always_assert(checker.good());
	}
}

void BasicInliner::rewrite_host_constructors()
{
	
}

void BasicInliner::rewrite_host_methods()
{
}



void BasicInliner::run()
{
	resolve_superclasses();
	inject_attributes();
	inject_methods();
	rewrite_host_constructors();
	rewrite_host_methods();
}
