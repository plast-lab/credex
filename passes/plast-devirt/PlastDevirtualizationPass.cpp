#include "DexUtil.h"
#include "PlastDevirtualizationPass.h"

#include "IRCode.h"
#include "Walkers.h"






void PlastDevirtualizationPass::run_pass(DexStoresVector& stores, ConfigFiles& ConfigFiles,
                                PassManager& manager) {

  /* devirtualise invokations written from a file*/
  /* filename should be declared at the config file*/

  std::cout << "PlastDevirtPass started..." << std::endl;
  //TODO file shouldn't be hard coded ?
  std::string filename("inputfuns.csv");
  std::map <std::string, ClassScopeInfo*> devirt_ins;
  /*the struct above are the input files data */

  parse_instructions(&devirt_ins, filename);
  Scope scope = build_class_scope(stores);
  devirt_targets(scope, devirt_ins);
  //TODO clean up
  return ;

}



void PlastDevirtualizationPass::parse_instructions(
  std::map <std::string, ClassScopeInfo*> *scope, std::string filename) {
  
  std::ifstream file;
  std::string line;
  file.open(filename);
  /*parse the files contents */
  if (!file.good()) {
    std::cout << "Plast:File not found: " << filename << std::endl;
    return;
  }
  int lineId = -1;
  while (file.good()) {
    lineId++;
    std::getline(file, line, '\n');
    std::string rest;

    PlastInvocSite* is = new PlastInvocSite();
    PlastMethodSpec* method = new PlastMethodSpec();

    rest = PlastDoopParser::parse_invok_site(line, *is);
    rest = PlastDoopParser::parse_method_spec(rest, *method);

    scope_insert(scope, is, method);
  } // end - while()

}


void PlastDevirtualizationPass::scope_insert(
  std::map <std::string, ClassScopeInfo*>* scope, PlastInvocSite* is, PlastMethodSpec* target) {
  
  std::string cls = is->gcls();
  const PlastMethodSpec& method = is->gmethod();
  std::string callee = is->gcallee();
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


void PlastDevirtualizationPass::devirt_targets(
	std::vector<DexClass*>& classes, std::map <std::string, ClassScopeInfo*>& devi_scope) {
  
  std::cout << "Starting PASS:" << std::endl;
  for (auto const &cls : classes) {
    std::string clsname = cls->get_name()->str();
    if (devi_scope.find(clsname) == devi_scope.end())
      continue;

    auto ccls = devi_scope[clsname]->info;

    //have to check every type of method
    std::vector<DexMethod*>& dmeths= cls->get_dmethods();
    for (auto const& dmeth: dmeths) {
      PlastMethodSpec spec(dmeth);
      devirt_targets(spec, ccls, dmeth);
    }


    std::vector<DexMethod*>& vmeths= cls->get_vmethods();
    for (auto const& vmeth: vmeths) {
      PlastMethodSpec spec(vmeth);
      devirt_targets(spec , ccls, vmeth);
    }
  }
}


void PlastDevirtualizationPass::devirt_targets(
	const PlastMethodSpec& spec, std::map<PlastMethodSpec, MethodScopeInfo*>& ccls, DexMethod* const& method) {
  if (ccls.find(spec) == ccls.end())
    return;

  auto lfm = ccls[spec]->info;
  auto irc = method->get_code();
  auto ircit = InstructionIterable(irc);
  for (auto it1 = ircit.begin(); it1 != ircit.end(); it1++) {
    IRInstruction* insn = it1->insn;
    if (!insn->has_method()) {
      continue;
    }
    DexMethodRef *method = insn->get_method();
    if (lfm.find(method->get_name()->str()) != lfm.end()) {
      auto  methodvector = lfm[(method->get_name()->str())];
      for (size_t i=0; i < methodvector->size(); i++) {
      	if (methodvector->at(i).first == 0) {
      	  //devirtualize the invocation
      	  devirtualize(insn, methodvector->at(i).second);
      	  //add casts if required
      	  auto it2 = irc->iterator_to(*it1);
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

void PlastDevirtualizationPass::create_static_copy(
	DexClass *cls, const PlastMethodSpec &mp, DexMethodRef*& d) {
  //creates a static version of an existing function to
  // allow devirtualization

  DexMethod* method = NULL;

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
          std::cerr << "CStaticCopy: Something went wrong" << std::endl;
        else
          d = method;
      return;
    }
  }

  for (auto meth :cls->get_vmethods()) {
    if (mp.compare(meth)) {
      method = meth;
      break;
    }
  }
  if (method == NULL) {
    std::cerr << "CStaticCopy: Something went wrong" << std::endl;
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

 
  //auto cls = type_class(clstype);
  method->set_virtual(false);
  cls->add_method(method);

  d = method;
}


void PlastDevirtualizationPass::devirtualize(IRInstruction *insn, PlastMethodSpec *spec) {
  DexMethodRef *ref;
  create_static_copy(type_class( DexType::get_type(DexString::make_string(
    spec->cls.c_str()))), *spec, ref);
  insn->set_opcode(OPCODE_INVOKE_STATIC)->set_method(ref);

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



static PlastDevirtualizationPass s_pass;