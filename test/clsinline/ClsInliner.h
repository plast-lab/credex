#pragma once

#include <vector>
#include <unordered_map>
#include <stdexcept>

#include "TypeSystem.h"
#include "ClsInlinePass.h"


TypeVector ancestors_of(DexClass* cls);


namespace clsinliner {

	
struct BasicInlineSpec : InlineSpec
{
	std::string host_name;
	std::string inlined_name;
	std::string inlined_attr;

	BasicInlineSpec(const JsonWrapper& jspec);

	virtual Inliner* get_inliner() const override;
};
	


class ConstraintViolation final : public std::runtime_error {
 public:
  explicit ConstraintViolation(const std::string& what_arg)
      : std::runtime_error(what_arg) {}
};

	
/**
   @brief Inline a class into another class in a basic way
   
   Assume that class I is to be inlined in class H.

   Let CS be the sequence of classes that are superclasses of both
   H and I  (this is always at least [java.lang.Object]

   Let IS be the sequence of classes that are superclasses of I
   but not of H. This always includes I at the end. 

   Let HS be the sequence of classes that are superclasses of H
   but not of I. This includes H at the end.

   IS(-) and HS(-)


   Constraints:
   - There is a private final field H.a which will be inlined
   - H.a is set in each constructor, by assignment from a 
     "new" object 
   - H.a is always set to nonnull
   - H.a is never changed in any method (since it is final!)
   - H.a is never changed (once assigned) in the constructor
     (since it is final!)
   - Classes in IS do not override methods inherited from classes in CS
   [Alternative: we could possibly live with a weaker restriction, if
     massive rewriting is allowed...]
   - no naming collisions are allowed! {Note: this may be a problem
     with private attributes inherited from base classes, so in general
     a mangling scheme could be used}

   The basic transformation is:
   - all fields of I are added to H. This includes fields inherited
     by superclasses of I that are not superclasses of H.
  
   - field H.a is removed from H

   - every method of I is added to H. This includes methods inherited
     by superclasses of I that are not superclasses of H.

   - Where a method of H passes H.a to a subcall or returns it, 'this'
     is returned instead  (i.e. the inlined object has identity equal to
     the host).
   
*/
struct BasicInliner : Inliner
{
	DexClass* host;
	DexClass* inlined;
	DexField* inlined_field;

	TypeVector common_superclasses;  //< CS
	TypeVector host_superclasses;    //< HS
	Scope inlined_superclasses;      //< IS


	/// For f a field  of I,  replaced_field[f] is a field of
	/// H constructed by this inliner
	std::unordered_map<DexFieldRef*,DexField*>  replaced_field;

	/// For m a method of I,  replaced_method[m] is a method of
	/// H constructed by this inliner, referencing the inlined
	/// fields
	std::unordered_map<DexMethodRef*,DexMethod*>  replaced_method;
	
	
	std::vector<DexField*> injected_fields;
	std::vector<DexMethod*> injected_vmethods;
    
	BasicInliner(DexClass* h, DexClass* i, DexField* f)
		: host(h), inlined(i), inlined_field(f) {
		
		always_assert(host != inlined);	    
	}

	/// @brief Compute the sequences of superclasses for
	/// H and I
	void resolve_superclasses();
	
	/// @brief Add attribute of inlined class to host
	/// @return the new field
	DexField* inject_attribute(DexField* fld);
    
	/// Add methods of inlined class to host
	void inject_methods();

	/// Add a new static method to construct inlined attributes
	void inject_inlined_constructor();

	/// Inject a method of inlined class to the host class
	/// Note that the method code contains a copy of the
	/// code of the original methd, and needs to be rewritten
	
	/// @return the new method
	DexMethod* inject_method(DexMethodRef* m);

	/// Rewrite the injected method to reference
	/// the injected fields/methods
	void rewrite_injected(DexMethod* inj_meth);
	
	/// Rewrite host constructors to inject calls to
	/// I's constructors
	void rewrite_host_constructors();
	
	/// Rewrite host methods to replace uses of inlined_field
	/// with appropriate code
	void rewrite_host_methods();


	void run() override;
};


}
