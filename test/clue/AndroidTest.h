/**
   @file
   @brief Utility classes for running .dex test code on android devices
*/
#pragma once

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

namespace fs = boost::filesystem;
namespace proc = boost::process;

/**
   @brief A RAII-style object representing a temporary directory

   When a new instance of this class is copied, a new temporary
   directory is created.
 */
class TempDir
{
public:
	
	static const char* default_model;
	
	/**
	   @brief construct a new object and a new temp directory

	   When this object is destroyed, the new directory and all
	   its contents will be discarded.

	   The constructor uses create() to create the new directory.
	 */
	TempDir(const fs::path& temp,
		const fs::path& model = fs::path(default_model))
		: _temp(temp), _model(model), _keep(false)
		{
			p = create(temp, model);
		}

	inline TempDir()
		: TempDir(fs::temp_directory_path(), fs::path(default_model))
		{}
	
	/**
	   @brief Create a new temporary directory.

	   The first argument is the directory into which the new
	   temporary directory will be created. The default is
	   /tmp on unix systems.

	   The second argument is a pattern containing a number of
	   '%' characters. These characters will be replaced by hex
	   random digits in an attempt to construct a unique name.


	   @param temp  the directory inside which the new directory 
	                will be created
	   @param model the pattern model for the new directory
	   @return  the path name to the newly created directory

	   @throws runtime_error if a new directory cannot be constructed
	*/
	static fs::path create(const fs::path& temp = fs::path(),
			       const fs::path& model = fs::path());

#if 0
	TempDir(const TempDir& o) {
		_temp = o._temp;
		_model = o._model;
		p = create(_temp, _model);
		_keep = false;
	}
#else
	TempDir(const TempDir&) = delete;
#endif
	TempDir& operator=(const TempDir&)=delete;

	TempDir(TempDir&& o) = default;

	TempDir& operator=(TempDir&& o) {
		if(!_keep && !p.empty()) fs::remove_all(p);
		_temp = o._temp;
		_model = o._model;
		p = std::move(o.p);
		o.p = "";
		_keep = o._keep;
		return *this;
	}
	
	~TempDir() {
		if(!_keep && !p.empty()) fs::remove_all(p);
	}

	const fs::path& dir() const { return p; }

	operator fs::path () const { return p; }
	
	inline void keep() { _keep = true; }
	inline bool is_kept() const { return _keep; }
private:
	fs::path _temp;
	fs::path _model;
	fs::path p;
	bool _keep;
};





class AndroidTest
{
public:
	AndroidTest() { }

	std::string avd;
	std::string serial;
	std::vector<fs::path> dexen;
	bool verbose = true;
	
	fs::path get_python3() {
		auto py = std::getenv("PYTHON");
		if(py) return py;
		fs::path python = proc::search_path("python3");
		if(! python.empty()) return python;
		// This may yield python 2.*, but it's the best we can do at
		// this point...
		python = proc::search_path("python");
		if(! python.empty()) return python;
		throw std::runtime_error("Cannot locate python executable!");
	}
	
	int call(const std::string& cmd, const std::vector<std::string>& a);

	bool operator()(const std::string& jclass) {
		return call("test", {jclass})==0;
	}
};


