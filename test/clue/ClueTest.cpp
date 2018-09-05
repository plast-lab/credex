
#include <fstream>

#include "ClueTest.h"
#include "DexLoader.h"
#include "JarLoader.h"

#include <boost/exception/diagnostic_information.hpp>


ClueTest::ClueTest()
	: config(Json::nullValue)
{
	DexStore root("classes");
	m_stores.emplace_back(std::move(root));
}

ClueTest::~ClueTest()
{ }


void ClueTest::load_dex(const std::string& dexfile)
{
	DexClasses classes;
	try {
	 	classes = load_classes_from_dex(dexfile.c_str());
	}
	catch(...) {
		// prints diagnostic and rethrows
	    std::cerr <<
	        "Unexpected exception, diagnostic information follows:\n" <<
    	    boost::current_exception_diagnostic_information();
		throw;
	}
	root_store().add_classes(std::move(classes));
}


void ClueTest::load_jar(const std::string& jarfile)
{
	if(!load_jar_file(jarfile.c_str(), &m_external_classes)) {
		std::cerr << "Loading of jar file '" << jarfile << "' failed!" << std::endl;
		throw std::runtime_error(__FUNCTION__);
	}
}

void ClueTest::run_passes(const std::vector<Pass*>& passes,
              const redex::ProguardConfiguration& pg_config,
              bool verify_none_mode, bool is_art_build)
{
	ConfigFiles cfg(config);
	// N.B. Here, we would call apply_deobfuscated_names when ProGuard map files are
	// provided!

	PassManager manager(passes, pg_config, config, verify_none_mode, is_art_build);
	// This will run passes that check the manager.no_proguard_rules() guard.
	// If not called, passes that require proguard "keep rules" would be skipped
	manager.set_testing_mode();
	manager.run_passes(m_stores, cfg);
}


void ClueTest::parse_config(const char* js)
{
  std::string jss(js);
  Json::Reader reader;
  reader.parse(jss, config);
  if(! reader.good()) {
    std::string what = "Json_parse errors: " + reader.getFormattedErrorMessages();
    throw std::runtime_error(what.c_str());
  }
}

void ClueTest::load_config(const std::string& cfgfile)
{
  std::ifstream cfgin;
  cfgin.open(cfgfile.c_str(), std::ifstream::in);
    Json::Reader reader;
  reader.parse(cfgin, config);
  if(! reader.good()) {
    std::string what = "Json_parse errors: " + reader.getFormattedErrorMessages();
    throw std::runtime_error(what.c_str());
  }
}
