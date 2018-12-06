#include <unordered_set>

#include "DexUtil.h"
#include "PlastDevirtualizationPass.h"

#include "IRCode.h"
#include "Walkers.h"

#include "MethodDevirtualizationPass.h"
#include "MethodDevirtualizer.h"

//used for redex Devirtualizer
#include "Resolver.h"
#include "Mutators.h"


struct CallCounter {
  uint32_t virtuals{0};
  uint32_t supers{0};
  uint32_t directs{0};

  static CallCounter plus(CallCounter a, CallCounter b) {
    a.virtuals += b.virtuals;
    a.supers += b.supers;
    a.directs += b.directs;
    return a;
  }
};

//TODO (hal)
//check what happens if static versions are created
//if so, make sure only these are going through devirtualization
//this should fix the virtual zip bug for our run only
//.

void PlastDevirtualizationPass::run_pass(DexStoresVector& stores, ConfigFiles& cfg,
                                PassManager& manager) {

  /* devirtualise invokations written from a file*/
  /* also , fully devirtualise functions that are given from a separate file */
  /* filename should be declared at the config file*/

  std::cout << "PlastDevirtPass started..." << std::endl;

  std::string invokdevirtfile;
  std::string methoddevirtfile;

  const Json::Value& pass_args =
      cfg.get_json_config()["PlastDevirtualizationPass"];
  methoddevirtfile =
    pass_args.get("GlobalDevirtTargets", invok_method_devirt_file).asString();
  invokdevirtfile =
    pass_args.get("LocalDevirtTargets", invok_site_devirt_file).asString();
  only_devirtualize_invok_interface =
    pass_args.get("OnlyDevirtInvokInterface", false).asBool();
  zip_vv = pass_args.get("ZipVirtualVersion", false).asBool();

  std::cout << invokdevirtfile << std::endl;
  std::cout << methoddevirtfile << std::endl;
  std::map <std::string, ClassScopeInfo*> devirt_ins;
  std::vector <PlastMethodSpec*> devirt_ms;
  /*the struct above are the input files data */

  parse_instructions_i(&devirt_ins, invokdevirtfile);
  reset_metrics();
  parse_instructions_m(&devirt_ms, methoddevirtfile);
  //config for devirtualizing only vmethods

  Scope scope = build_class_scope(stores);
  gather_statistics(scope);


  devirt_methods(scope, devirt_ms);
  devirt_targets(scope, devirt_ins);


  //metrics names are not correlated with their corresponding
  //meaning, I just re-used the same structured
  std::cout << "done..." << std::endl;
  {  std::cout << "Devirtualized " << m_metrics.num_methods_not_using_this << " methods\nVirtual:"
    <<m_metrics.num_virtual_calls << "\nSuper:" << m_metrics.num_super_calls <<
    "\nDirect:" << m_metrics.num_direct_calls << std::endl;}
  //TODO clean up

  std::cout << "Invok V:" << l_metrics.num_invok_virtual << std::endl;
    std::cout << "TOTAL I:" << l_metrics.num_invok_interface << std::endl;
  std::cout << "New M:" << l_metrics.num_new_methods << std::endl;
  std::cout << "Extra OP:" << l_metrics.num_extra_opcodes << std::endl;
  std::cout << "TOTAL INVOK:" << l_metrics.num_total_invocations << std::endl;
  std::cout << "TOTAL METH:" << l_metrics.num_total_methods << std::endl;
  std::cout << "TOTAL INTER:" << l_metrics.num_total_interface << std::endl;
  return ;

}



void PlastDevirtualizationPass::parse_instructions_i(
  std::map <std::string, ClassScopeInfo*> *scope, std::string filename) {

  std::ifstream file;
  std::string line;
  file.open(filename);
  /*parse the file's contents */
  if (!file.good()) {
    std::cout << "Plast:File not found: " << filename << std::endl;
    return;
  }
  int lineId = -1;
  while (file.good()) {
    lineId++;
    std::getline(file, line, '\n');
    if (line.compare("") == 0)
      return;
    std::string rest;

    PlastInvocSite* is = new PlastInvocSite();
    PlastMethodSpec* method = new PlastMethodSpec();



    rest = PlastDoopParser::parse_invok_site(line, *is);
    rest = PlastDoopParser::parse_method_spec(rest, *method);

    is->to_dalvik();
    //std::cerr << is->gcls() << std::endl;
    method->to_dalvik();


    if (type_class( DexType::get_type(DexString::make_string(
      method->cls.c_str()))) == NULL) {
        delete is;
        delete method;
        continue;
    }


    scope_insert(scope, is, method);
  } // end - while()

}


void PlastDevirtualizationPass::parse_instructions_m(
  std::vector <PlastMethodSpec*>* devms, std::string filename) {

  std::ifstream file;
  std::string line;
  file.open(filename);
  /*parse the file's contents */
  if (!file.good()) {
    std::cout << "Plast:File not found: " << filename << std::endl;
    return;
  }
  int lineId = -1;
  while (file.good()) {
    lineId++;
    std::getline(file, line, '\n');
    if (line.compare("") == 0)
      return;
    std::string rest;
    PlastMethodSpec* method = new PlastMethodSpec();

    rest = PlastDoopParser::parse_method_spec(line, *method);
    method->to_dalvik();

    if (type_class( DexType::get_type(DexString::make_string(
      method->cls.c_str()))) == NULL) {
        delete method;
        continue;
    }

    devms->push_back(method);
  } // end - while()

}



void PlastDevirtualizationPass::scope_insert(
  std::map <std::string, ClassScopeInfo*>* scope, PlastInvocSite* is, PlastMethodSpec* target) {

  std::string cls = is->gcls();
  const PlastMethodSpec& method = is->gmethod();
  auto callee = is->gcallee();
  int n = is->gn();

  if (scope->find(cls) == scope->end())
    scope->operator[](cls) = new ClassScopeInfo;
  if (scope->operator[](cls)->info.find(method) == scope->operator[](cls)->info.end())
    scope->operator[](cls)->info.operator[](method) = new MethodScopeInfo;

  MethodScopeInfo* msi = scope->operator[](cls)->info.at(method);
  if (msi->info.find(callee) == msi->info.end())
    msi->info.operator[](callee) = new std::vector<std::pair<int, PlastMethodSpec*> >() ;
  msi->info.operator[](callee)->push_back(std::pair<int, PlastMethodSpec* >(n, target));


}

void PlastDevirtualizationPass::devirt_methods(std::vector<DexClass*>& scope,
  std::vector<PlastMethodSpec*> methods) {
  std::cout << "Starting devirtualizment " << std::endl;
  // create a vector to call the already built-in function
  std::unordered_set <DexMethod*> devirtualizable_methods;
  //here I need to get the DexMethod * from the class for each file entry
  //in order to devirtualise. To do this I have to look it up
  //in the vmethods vector , which is a bit inefficient , but
  //the only way that can be done given the currect version's API
  for (auto method: methods) {
    auto cls = type_class(DexType::get_type(method->cls.c_str()));
    if (cls == NULL) {
        std::cout << "Plast: Class not found!" << std::endl;
        continue;
    }
    std::vector<DexMethod*> vmeths= std::vector<DexMethod*>(cls->get_vmethods());
    for (auto const &m : vmeths) {
      if (method->compare(m)) {
        devirtualizable_methods.insert(m);
        delete method;
        break;
      }
      //again ignore naming, I just re-use an existing struct
    }
  }
  //TODO
  //should also work on the case of methods that dont require this...
  staticize_methods_using_this(scope, devirtualizable_methods);
}

void PlastDevirtualizationPass::devirt_targets(
	std::vector<DexClass*>& classes, std::map <std::string, ClassScopeInfo*>& devi_scope) {

  for (auto const &cls : classes) {
    std::string clsname = cls->get_name()->str();
    if (devi_scope.find(clsname) == devi_scope.end())
      continue;

    //std::cout << clsname << std::endl;
    auto ccls = devi_scope[clsname]->info;

    //have to check every type of method
    //have to create a copy of dmethds, since they might while iterations run
    std::vector<DexMethod*> dmeths= std::vector<DexMethod*>(cls->get_dmethods());

    for (auto const& dmeth: dmeths) {
      PlastMethodSpec spec(dmeth);
      devirt_targets(spec, ccls, dmeth);
    }


    std::vector<DexMethod*> vmeths= std::vector<DexMethod*>(cls->get_vmethods());

    for (auto const& vmeth: vmeths) {
      PlastMethodSpec spec(vmeth);
      devirt_targets(spec , ccls, vmeth);
    }
  }
}


void PlastDevirtualizationPass::devirt_targets(
	const PlastMethodSpec& spec, std::map<PlastMethodSpec, MethodScopeInfo*>& ccls, DexMethod* const& method) {

    //std::cout << spec.name << spec.rtype  <<  std::endl;

  if  (ccls.find(spec) == ccls.end()) {
    return;
  }
  auto lfm = ccls[spec]->info;
  auto irc = method->get_code();
  auto ircit = InstructionIterable(irc);
  for (auto it1 = ircit.begin(); it1 != ircit.end(); it1++) {
    IRInstruction* insn = it1->insn;
    if (!insn->has_method()) {
      continue;
    }
    DexMethodRef *method = insn->get_method();
    std::pair<std::string,std::string> p(method->get_class()->get_name()->str(),
        method->get_name()->str());
    if (lfm.find(p) != lfm.end()) {
      auto  methodvector = lfm[p];
      for (size_t i=0; i < methodvector->size(); i++) {

      	if (methodvector->at(i).first == 0) {
      	  //devirtualize the invocation
      	  devirtualize(insn, methodvector->at(i).second);
      	  //add casts if required
      	  //auto it2 = irc->iterator_to(*it1);
      	  //add_cast(insn ,irc, methodvector->at(i).second, it2);

      	  //clean up
      	  delete methodvector->at(i).second;
      	  methodvector->erase(methodvector->begin()+i);
      	  i--;
      	}
        else
        	methodvector->at(i).first = methodvector->at(i).first - 1;
      }
    }
  }
}

void PlastDevirtualizationPass::create_static_copy(DexClass *cls,
  const PlastMethodSpec &mp, DexMethodRef*& d, bool invok_interface) {
  //creates a static version of an existing function to
  // allow devirtualization

  DexMethod* method = NULL;
  d = NULL;

  //look if the method already exists
  auto newname = DexString::make_string((statprefix+ mp.name).c_str());
  std::vector<std::string> *margs = new std::vector<std::string>(*(mp.args));
  if (margs->size() > 0) {
    margs->push_back(std::string(margs->back()));
    for (int i=margs->size()-2; i; i--)
      margs->at(i) = margs->at(i-1);
    margs->at(0) = mp.cls;
  }
  else
    margs->push_back(mp.cls);
  PlastMethodSpec stmp(std::string(newname->c_str()), margs, mp.rtype);
  stmp.cls = mp.cls;


  for (auto meth :cls->get_dmethods()) {
    if (stmp.compare(meth)) {
      method = meth;
        if (!is_static(method))
          std::cerr << "CStaticCopy: Something went a bit wrong" << std::endl;
        else {
          d = method;
        }
      return;
    }
  }
  // if we only seek to devirtualize invok interface , stop here
  if (!invok_interface && only_devirtualize_invok_interface)
    return;

  for (auto meth :cls->get_vmethods()) {
    if (mp.compare(meth)) {
      method = meth;
      break;
    }
  }
  if (method == NULL) {
    d = NULL;
    return;
  }



  auto clstype = method->get_class();
  method = DexMethod::make_method_from(method, clstype, newname);
  auto proto = method->get_proto();
  auto params = proto->get_args()->get_type_list();
  params.push_front(clstype);
  auto new_args = DexTypeList::make_type_list(std::move(params));
  auto new_proto = DexProto::make_proto(proto->get_rtype(), new_args);
  DexMethodSpec spec;
  spec.proto = new_proto;


  //auto tm = g_redex->make_method(
  //  clstype , DexString::make_string(("_stat_"+ mp.name).c_str()) ,new_proto);
  method->change(spec, false);
  auto code = method->get_code();

  // If the debug info param count doesn't match the param count in the
  // method signature, ART will not parse any of the debug info for the
  // method. Note that this shows up as a runtime error and not a
  // verification error. To avoid that, we insert a nullptr here.
  if (code) {
    auto debug = code->get_debug_item();
    if (debug) {
      auto& param_names = debug->get_param_names();
      param_names.insert(param_names.begin(), nullptr);
    }
  }

  method->set_access(method->get_access() | ACC_STATIC);


  method->set_virtual(false);
  cls->add_method(method);
  l_metrics.num_new_methods++;
  l_metrics.num_extra_opcodes += code->sum_opcode_sizes();
  d = method;
}


void PlastDevirtualizationPass::devirtualize(IRInstruction *insn, PlastMethodSpec *spec) {
  DexMethodRef *ref;
  DexClass *cls = type_class( DexType::get_type(DexString::make_string(
    spec->cls.c_str())));
  if (cls == NULL) {
    std::cout << "Plast: Class not found!" << std::endl;
      return ;
  }
  // we don't want to devirtualize invocations that aren't
  // invoke-interface

  create_static_copy(cls, *spec, ref, insn->opcode() == OPCODE_INVOKE_INTERFACE);

  if (ref == NULL)
    return;

  if (is_invoke_virtual(insn->opcode()))
    l_metrics.num_invok_virtual++;
  if (insn->opcode() == OPCODE_INVOKE_INTERFACE)
    l_metrics.num_invok_interface++;

  insn->set_opcode(OPCODE_INVOKE_STATIC)->set_method(ref);
  if (this->zip_vv)
    zip_virtual_version(cls, spec, ref);
}




void PlastDevirtualizationPass::add_cast(
	IRInstruction *insn, IRCode *irc,  PlastMethodSpec *spec, IRList::iterator& it) {
  IRInstruction* check_cast = new IRInstruction(OPCODE_CHECK_CAST);
  check_cast->set_type(
  DexType::get_type(DexString::make_string(
  spec->cls.c_str())))->set_src(0, insn->src(0));

  MethodItemEntry *newinstr = new MethodItemEntry(check_cast);


  irc->insert_before(it, *newinstr);
   irc->insert_before(it,
   *(new MethodItemEntry((new IRInstruction
    (IOPCODE_MOVE_RESULT_PSEUDO_OBJECT))->set_dest(insn->src(0)))));


}

//Clearly not functional yet.
//Will crash on fblite, not on uber
//Works fine on some small examples
//TODO debug, solve the recursive functions problem here
void PlastDevirtualizationPass::zip_virtual_version(DexClass* cls,
  PlastMethodSpec* mp, DexMethodRef* newmethod) {
  //change the virtual method to contain only 1 call
  // on the static version
  std::cerr << "Dangerous function is called" << std::endl;
  DexMethod* method = NULL;
  for (auto meth :cls->get_vmethods()) {
    if (mp->compare(meth)) {
      method = meth;
      break;
    }
  }
  if (method == NULL) {
    std::cerr << "VirtualZip: Something went wrong" << std::endl;
    return;
  }

  IRCode* code = method->get_code();
  auto irc = InstructionIterable(code);


  method->set_dex_code(nullptr);
  method->set_code(nullptr);
  method->set_code(std::make_unique<IRCode>(method, 0));

  //TODO make sure the method returns the result , if
  //the method isnt void
  IRInstruction* invok_static = new IRInstruction(OPCODE_INVOKE_STATIC);
  invok_static->set_method(newmethod);
  invok_static->set_arg_word_count(mp->args->size()+1);
  for (int i=0; i < mp->args->size()+1; i++)
    invok_static->set_src(i, i);
  IRInstruction* rv = new IRInstruction(OPCODE_RETURN_VOID);


  code = method->get_code();
  code->clear_cfg();
  code->push_back(invok_static);
  code->push_back(rv);
  code->build_cfg();

}

//reusing code found in MethodDevirtualizizer.cpp


void patch_call_site(DexMethod* callee,
                     IRInstruction* method_inst,
                     CallCounter& counter) {
  auto op = method_inst->opcode();
  if (is_invoke_virtual(op)) {
    method_inst->set_opcode(OPCODE_INVOKE_STATIC);
    counter.virtuals++;
  } else if (is_invoke_super(op)) {
    method_inst->set_opcode(OPCODE_INVOKE_STATIC);
    counter.supers++;
  } else if (is_invoke_direct(op)) {
    method_inst->set_opcode(OPCODE_INVOKE_STATIC);
    counter.directs++;
  } else {
    always_assert_log(false, SHOW(op));
  }

  method_inst->set_method(callee);
}


void fix_call_sites(const std::vector<DexClass*>& scope,
                    const std::unordered_set<DexMethod*>& target_methods,
                    DevirtualizerMetrics& metrics,
                    bool drop_this = false) {
  const auto fixer = [&target_methods, drop_this](DexMethod* m) -> CallCounter {
    CallCounter call_counter;
    IRCode* code = m->get_code();
    if (code == nullptr) {
      return call_counter;
    }

    for (const MethodItemEntry& mie : InstructionIterable(code)) {
      IRInstruction* insn = mie.insn;
      if (!insn->has_method()) {
        continue;
      }

      MethodSearch type = drop_this ? MethodSearch::Any : MethodSearch::Virtual;
      auto method = resolve_method(insn->get_method(), type);
      if (method == nullptr || !target_methods.count(method)) {
        continue;
      }

      always_assert(drop_this || !is_invoke_static(insn->opcode()));
      patch_call_site(method, insn, call_counter);

      if (drop_this) {
        auto nargs = insn->arg_word_count();
        for (uint16_t i = 0; i < nargs - 1; i++) {
          insn->set_src(i, insn->src(i + 1));
        }
        insn->set_arg_word_count(nargs - 1);
      }
    }

    return call_counter;
  };

  CallCounter call_counter =
      walk::parallel::reduce_methods<CallCounter, Scope>(
          scope,
          fixer,
          [](CallCounter a, CallCounter b) -> CallCounter {
            a.virtuals += b.virtuals;
            a.supers += b.supers;
            a.directs += b.directs;
            return a;
          });

  metrics.num_virtual_calls += call_counter.virtuals;
  metrics.num_super_calls += call_counter.supers;
  metrics.num_direct_calls += call_counter.directs;
}

void PlastDevirtualizationPass::make_methods_static(const std::unordered_set<DexMethod*>& methods,
                         bool keep_this) {
for (auto method : methods) {
    TRACE(VIRT,
          2,
          "Staticized method: %s, keep this: %d\n",
          SHOW(method),
          keep_this);
  mutators::make_static(
      method, keep_this ? mutators::KeepThis::Yes : mutators::KeepThis::No);
  }
}

void PlastDevirtualizationPass::staticize_methods_using_this(
    const std::vector<DexClass*>& scope,
    const std::unordered_set<DexMethod*>& methods) {
  fix_call_sites(scope, methods, m_metrics, false /* drop_this */);
  make_methods_static(methods, true);
  TRACE(VIRT, 1, "Staticized %lu methods using this\n", methods.size());
  m_metrics.num_methods_not_using_this += methods.size();
}

void PlastDevirtualizationPass::gather_statistics(const std::vector<DexClass*>& scope) {

    for (auto const &cls : scope) {

      std::vector<DexMethod*>& dmeths= cls->get_dmethods();
      for (auto const &dmeth: dmeths) {
        l_metrics.num_total_methods++;
        auto irc = dmeth->get_code();
        if (!irc)
          continue;
        auto ircit = InstructionIterable(irc);
        for (auto it1 = ircit.begin(); it1 != ircit.end(); it1++) {
          IRInstruction* insn = it1->insn;
          if (insn->has_method()&& is_invoke_virtual(insn->opcode())) {
            l_metrics.num_total_invocations++;
          }
        }
      }


      std::vector<DexMethod*>& vmeths= cls->get_vmethods();
      for (auto const &vmeth: vmeths) {
        l_metrics.num_total_methods++;
        auto irc = vmeth->get_code();
        if (!irc)
          continue;
        auto ircit = InstructionIterable(irc);
        for (auto it1 = ircit.begin(); it1 != ircit.end(); it1++) {
          IRInstruction* insn = it1->insn;
          if (insn->has_method() && is_invoke_virtual(insn->opcode())) {
            l_metrics.num_total_invocations++;
          }
          if (insn->has_method() && insn->opcode() == OPCODE_INVOKE_INTERFACE) {
            l_metrics.num_total_interface++;
          }
      }
    }
  }
}


static PlastDevirtualizationPass s_pass;
