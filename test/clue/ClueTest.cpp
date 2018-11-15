
#include <fstream>

#include "ClueTest.h"
#include "DexLoader.h"
#include "DexOutput.h"
#include "JarLoader.h"

#include <boost/exception/diagnostic_information.hpp>

ClueTest::ClueTest()
	: config(Json::nullValue)
{
	g_redex = new RedexContext();
	DexStore root("classes");
	m_stores.emplace_back(std::move(root));
}

ClueTest::~ClueTest()
{
	delete g_redex;
}


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
	// Note: this is commented out, since it generates broken dexes!!!!
	// This must be investigated!
	//manager.set_testing_mode();
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


void ClueTest::write_dexen(const std::string& out_dir)
{
	ConfigFiles cfg(config, out_dir);
	const JsonWrapper& json_cfg = cfg.get_json_config();

	// Possibly build locator index
	LocatorIndex* locator_index = nullptr;
	if(json_cfg.get("emit_locator_strings", false)) {
		locator_index = new LocatorIndex(make_locator_index(m_stores));
	}

	auto pos_output = cfg.metafile(json_cfg.get("line_number_map", std::string()));
	auto pos_output_v2 = cfg.metafile(json_cfg.get("line_number_map_v2", std::string()));

	std::unique_ptr<PositionMapper> pos_mapper(PositionMapper::make(pos_output, pos_output_v2));

	for(auto& store : stores() ) {
		for(size_t i=0; i<store.get_dexen().size(); ++i) {
			std::ostringstream ss;
			ss << out_dir << "/" << store.get_name();
			if(store.get_name().compare("classes")==0) {
				if(i>0) ss << (i+1);
			} else {
				ss << (i+2);
			}
			ss << ".dex";

			auto this_dex_stats = write_classes_to_dex(
					ss.str(),
					&store.get_dexen()[i],
					locator_index,
					i,
					cfg,
					pos_mapper.get(),
					nullptr,
					nullptr
				);
		}

	}
	pos_mapper->write_map();
}

