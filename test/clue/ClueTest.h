/**
   @file
   @brief Utility classes for clue-redex tests.
*/

#pragma once

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "DexStore.h"
#include "RedexContext.h"
#include "Pass.h"
#include "ProguardConfiguration.h"


/**
  @brief Test fixture class for clue-redex unit tests.

  This fixture class initializes the @ref RedexContext, so that a run of redex-all
  can be simulated. The fixture can load .dex files and external jars, accept
  configuration options and execute a number of passes.

  Current limitations:
  * only one DexStore is supported

*/
class ClueTest : public testing::Test
{
public:

  /// Json object containing configuration options for the passes
  Json::Value config;

  /// Return the DexStore for this run
  inline DexStore& root_store() { return m_stores[0]; }

  /// Return an internal vector of DexStores containing only the root store at [0]
  inline DexStoresVector& stores() { return m_stores; }

  /**
  	@brief Load classes from a .dex file

  	The method will load classes found in the `dexfile`.
  	The dexfile will be added to the stores.

  	In a multidex scenario, dex files should be loaded in the order that they
  	would be named in the .apk file, i.e., first the "classes" file, then the
  	"classes2" file, etc.

    All .dex classes are loaded into root store.

    @param dexfile a .dex filename to load
    */
  void load_dex(const std::string& dexfile);

  /**
    @brief Load a library jar file.

    Load a library jar file as a collection of external classes.
    */
  void load_jar(const std::string& jarfile);


  /**
    @brief Generate .dex files from the stores.

    Generate a set of .dex files corresponding to the current 
    contents of the ReDex context, i.e., reflecting the result
    of whatever passes or other transformations have occurred.

    The code is very similar to the code executed by credex-all at the
    end of a run (essentially, only some statistics reporting is omitted).

    The given argument designates the path name to a directory where the
    generated files will be created. Note that it must not be the empty
    string!

    @param out_dir a path name where the files will be stored.
    @return A vector of strings containing the file names of the generated files
    */
  std::vector<std::string> write_dexen(const std::string& out_dir);


  /**
    @brief Set the config field to JSON from parsing a string

    The value of the `config` field is set to the Json::Value obtained by
    parsing the argument.

    @param json a string used to set the `config` field
    @throws std::runtime_error if the parsing failed
    @see CLUETEST_CONFIG
    */
  void parse_config(const char* json);

  /**
    @brief Set the config field to the contents of a .json file.

    The value of the `config` field is set to the Json::Value obtained by
    parsing the contents of file `configfile`.

    @param configfile the name of a file containing the json for config
    @throws std::runtime_error if the parsing failed
    */
  void load_config(const std::string& configfile);

  /**
    @brief Run a sequence of passes.

    This call will create a PassManager to run a sequence of passes.
    The PassManager will be created with the given sequence of registered
    passes, a ProGuard configuration, and the `verify_none_mode` and `is_art_build` flags.

    The behaviour of the pass manager will be controlled by the `config` field. See the
    documentation of the PassManager for details.

    @param passes A vector of Pass pointers to register to the PassManager.
    @param pg_config a ProGuard configuration object
    @param verify_none_mode Flags non-verify code for execution (see PassManager::init() )
    @param is_art_build  Flags an ART build (see PassManager::init() )
  */
  void run_passes(const std::vector<Pass*>& passes,
              const redex::ProguardConfiguration& pg_config = redex::ProguardConfiguration(),
              bool verify_none_mode = false,
              bool is_art_build = false);


  /// Constructs the fixture
  ClueTest();

  /// Destroys the fixture
  ~ClueTest();

private:
  /// List of DexStore stores for the test context
  std::vector<DexStore> m_stores;

  /// Vector of external classes
  Scope m_external_classes;
};





/**
  @brief Convenience macro for calling ClueTest::parse_config()

  With this macro, one can create easily JSON code in an editor-friendly way.

  Use it from within a test as follows:

  CLUETEST_CONFIG({
    "foo": {
      "bar": 1,
      "baz": [1,2,3]
    }
  });

 */
#define CLUETEST_CONFIG(...) parse_config(#__VA_ARGS__)
