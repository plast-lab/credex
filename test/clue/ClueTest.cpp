
#include <fstream>

#include "ClueTest.h"
#include "DexLoader.h"
#include "DexOutput.h"
#include "JarLoader.h"
#include "InstructionLowering.h"

#include <boost/exception/diagnostic_information.hpp>


static inline bool is_yes(const char* txt) {
  std::string value = txt;

  return value=="yes" || value=="true";
}


ClueTest::ClueTest()
	: config(Json::nullValue)
{
	g_redex = new RedexContext();
	DexStore root("classes");
	m_stores.emplace_back(std::move(root));
	if(std::getenv("CLUE_TEST_KEEP")!=nullptr &&
	   is_yes(std::getenv("CLUE_TEST_KEEP")))
	  outdir.keep();
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
	ConfigFiles cfg(config, outdir.dir().native());
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

static Json::CharReaderBuilder jbuilder;

inline static void parse_json(std::istream& sin, Json::Value* out)
{
	std::string err;
	if(! Json::parseFromStream(jbuilder, sin, out, &err)) {
		throw std::runtime_error(err.c_str());
	}
}

void ClueTest::parse_config(const char* js)
{
  std::istringstream jss(js);
  parse_json(jss, &config);
}

void ClueTest::load_config(const std::string& cfgfile)
{
  std::ifstream cfgin;
  cfgin.open(cfgfile.c_str(), std::ifstream::in);
  parse_json(cfgin, &config);
}


std::vector<std::string> ClueTest::write_dexen()
{
	ConfigFiles cfg(config, outdir.dir().native());
	const JsonWrapper& json_cfg = cfg.get_json_config();

	// Prepare code for output (ignore returned stats)
	(void) instruction_lowering::run(m_stores);
	
	// Possibly build locator index
	LocatorIndex* locator_index = nullptr;
	if(json_cfg.get("emit_locator_strings", false)) {
		locator_index = new LocatorIndex(make_locator_index(m_stores));
	}

	// Possibly build position mapper
	auto pos_output = cfg.metafile(json_cfg.get("line_number_map", std::string()));
	auto pos_output_v2 = cfg.metafile(json_cfg.get("line_number_map_v2", std::string()));

	std::unique_ptr<PositionMapper> pos_mapper(PositionMapper::make(pos_output, pos_output_v2));

	std::vector<std::string> generated_dexen;
	
	// Loop over the stores
	for(auto& store : stores() ) {
		
		// Loop over each store's dexen
		for(size_t i=0; i<store.get_dexen().size(); ++i) {

			// Compute a name for the dex file to be written
			std::ostringstream ss;
			ss << store.get_name();
			if(store.get_name().compare("classes")==0) {
			  // For the root store, the .dex names are
			  // classes, classes2, classes3, ...
				if(i>0) ss << (i+1);
			} else {
			  // For other stores, say named 'n', the
			  // .dex names are  n2, n3, ...
				ss << (i+2);
			}
			// add extension
			ss << ".dex";

			fs::path dexfname = outdir.dir()/ss.str();
			
			// ignore the stats returned by the following call
			write_classes_to_dex(
				dexfname.string(),
				&store.get_dexen()[i],
				locator_index,
				i,
				cfg,
				pos_mapper.get(),
				nullptr,
				nullptr
				);
			generated_dexen.push_back(dexfname.string());
		}

	}

	// There seems to be a bug in this code, re. constructors!
	// Since it is not necessary for our purposes, we comment it out!
	//pos_mapper->write_map();

	return generated_dexen;
}


