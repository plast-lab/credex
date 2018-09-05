/* Reads a list of method names from a file and removes them from
   their containing classes. The input file is assumed to contain one
   JVM-style method descriptor per line (as in names contained in
   .smali files). Compared to opt/simpleinline/Deleter, Remover does
   not run checks (like is_concrete() or can_delete()). */

#include <fstream>
#include "Remover.h"
#include "DexUtil.h"

typedef std::set<std::pair<DexClass*, DexMethod*> > CMethods;
typedef std::set<std::pair<std::string, std::string>> CMethodStrs;

std::string f_name_rmethods;
std::string f_name_amethods;

/* Given a DexMethod, generates its JVM descriptor string. */
std::string gen_JVM_descriptor(DexMethod* m) {
  std::stringstream ss;
  ss << m->get_name()->c_str();
  ss << show(m->get_proto());
  return ss.str();
}

std::string gen_method_desc(const std::string clsQName, DexMethod* m) {
  std::stringstream ss;
  ss << clsQName;
  ss << gen_JVM_descriptor(m);
  return ss.str();
}

std::string gen_method_desc(const std::pair<DexClass*, DexMethod*>& cm) {
  return gen_method_desc(cm.first->get_name()->c_str(), cm.second);
}

void check_method(DexClass* cls, std::string& clsQName, DexMethod* method,
                  const CMethodStrs& cMethodStrs, CMethods& methods,
                  int64_t& counter) {
  std::string descriptor = gen_JVM_descriptor(method);
  auto const m = std::make_pair(clsQName, descriptor);

  if (cMethodStrs.find(m) != cMethodStrs.end()) {
    auto const p = std::make_pair(cls, method);
    std::string meth_desc = gen_method_desc(clsQName, method);
    if (methods.find(p) == methods.end()) {
      methods.insert(p);
      counter++;
    }
  }
}

void mark_methods(std::vector<DexClass*>& classes, const CMethodStrs& cMethodStrs,
                  CMethods& methods, int64_t& counter) {
  for (auto const &cls : classes) {
    std::string clsQName = cls->get_name()->c_str();
    for (auto const& dm : cls->get_dmethods()) {
      check_method(cls, clsQName, dm, cMethodStrs, methods, counter);
    }
    for (auto const& vm : cls->get_vmethods()) {
      check_method(cls, clsQName, vm, cMethodStrs, methods, counter);
    }
  }
}

/* Given a list of class/method pairs, finds all candidates for removal,
   then proceeds to delete them. */
void remove_methods(std::vector<DexClass*>& classes, CMethodStrs& cMethodStrs) {
  CMethods methods;
  std::cout << "Marking methods to remove..." << std::endl;
  int64_t markedForRemoval = 0;
  mark_methods(classes, cMethodStrs, methods, markedForRemoval);

  for (auto const& cm : methods) {
    cm.first->remove_method(cm.second);
    std::cout << "Removing " << gen_method_desc(cm) << std::endl;
  }

  std::cout << "Removed " << methods.size() << " methods (from " << markedForRemoval << " marked)." << std::endl;
}

/* Given a list of class/method pairs, finds all non-abstract
   candidates to be made abstract, then mutates them.
 */
void make_methods_abstract(std::vector<DexClass*>& classes, CMethodStrs cMethodStrs) {
  CMethods methods;
  std::cout << "Marking methods to make abstract..." << std::endl;
  int64_t markedForAbstraction = 0;
  mark_methods(classes, cMethodStrs, methods, markedForAbstraction);

  int64_t madeAbstract = 0;
  for (auto const& cm : methods) {
    DexMethod* m = cm.second;
    std::string descriptor = gen_method_desc(cm);
    if (is_abstract(m)) {
      std::cout << descriptor << " is already abstract" << std::endl;
    } else {
      DexAccessFlags original_access = m->get_access();
      DexMethodSpec ref(m->get_class(), m->get_name(), m->get_proto());
      m->change(ref, false);
      if (m->is_def()) {
        std::cout << "Making " << descriptor << " abstract" << std::endl;
        m->set_access(original_access | ACC_ABSTRACT);
      } else {
        std::cout << descriptor << " has false is_def()" << std::endl;
      }
      madeAbstract++;
    }
  }

  std::cout << madeAbstract << " methods made abstract (from " << markedForAbstraction << " marked)." << std::endl;
}

void read_methods(std::string f_name, CMethodStrs& methods) {
  std::ifstream file;

  file.open(f_name);
  if (!file.good()) {
    std::cout << "File not found: " << f_name << std::endl;
    return;
  }

  while (file.good()) {
    std::string cName, mName;
    std::getline(file, cName, ':');
    std::getline(file, mName, '\n');
    if (!cName.empty() && !mName.empty()) {
      // If more (tab-separated) columns exist, keep first one.
      size_t tabPos = mName.find('\t');
      if (tabPos != std::string::npos) {
        mName = mName.substr(0, tabPos);
      }
      methods.insert(std::make_pair(cName, mName));
    } else if (cName.empty() && mName.empty()) {
      // Ignore empty lines.
    } else {
      std::cerr << "Bad method entry: (" << cName << ", " << mName << ")" << std::endl;
    }
  }
  file.close();

  std::cout << "Read " << methods.size() << " methods from " << f_name << std::endl;
}

void RemoverPass::run_pass(DexStoresVector& dexen, ConfigFiles& cfg, PassManager& mgr) {
  CMethodStrs rMethodStrs;
  CMethodStrs aMethodStrs;
  std::ifstream aFile;

  // TODO: (vsam) These should be put in the PassConfig section!
#if 0
  f_name_rmethods = cfg.get_rmethods();
  if (f_name_rmethods.empty()) {
      f_name_rmethods = "methods_to_remove.csv";
      std::cout << "Using default file " << f_name_rmethods << std::endl;
  } else
      std::cout << "Using custom file " << f_name_rmethods << std::endl;

  f_name_amethods = cfg.get_amethods();
  if (f_name_amethods.empty()) {
      f_name_amethods = "methods_to_make_abstract.csv";
      std::cout << "Using default file " << f_name_amethods << std::endl;
  } else
      std::cout << "Using custom file " << f_name_amethods << std::endl;
#endif

cfg.get_json_config().get("rmethods", "methods_to_remove.csv", f_name_rmethods);
cfg.get_json_config().get("amethods", "methods_to_make_abstract.csv", f_name_amethods);

  std::cout << "Reading list of methods to remove from " << f_name_rmethods << "..." << std::endl;
  read_methods(f_name_rmethods, rMethodStrs);

  std::cout << "Reading list of methods to make abstract from " << f_name_amethods << "..." << std::endl;
  read_methods(f_name_amethods, aMethodStrs);

  Scope scope = build_class_scope(dexen);
  std::cout << "Method removal pass" << std::endl;
  remove_methods(scope, rMethodStrs);

  std::cout << "Method abstraction optimization" << std::endl;
  make_methods_abstract(scope, aMethodStrs);

  std::cout << "Method removal pass done." << std::endl;
}

static RemoverPass s_pass;
