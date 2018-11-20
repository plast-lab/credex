
#include <stdexcept>
#include "AndroidTest.h"


const char* TempDir::default_model = "credex-test-%%%%-%%%%";


fs::path TempDir::create(const fs::path& temp, const fs::path& model)
{
	fs::path ttemp = temp;
	fs::path tmodel;

	if(model.is_absolute())
		throw std::invalid_argument("The model parameter cannot be an absolute path");
		
	
	if(temp.empty() && model.empty()) {
		tmodel = fs::temp_directory_path()/fs::path(default_model);
	} else {
		// normalize ttemp
		if(ttemp.empty())
			ttemp = fs::temp_directory_path();
	
		if(model.empty()) {
			tmodel = temp;
		} else {
			tmodel = temp/model;
		}
	}

	// We may need to try many times for a new directory.
	// Use twice the number of legal combinations, up to
	// a reasonable maximum

	// No of %      tries   combos
	//   0          1       1
	//   1          31      16
	//   2          511     256
	//   >=3        4096    >=4096
	auto mstr = model.string();
	size_t mno = std::count(mstr.begin(), mstr.end(), '%');
	mno = 4*mno+1; // 4 bits in each hex digit
	if(mno > 12) mno = 12; // max 4096 tries!
		
	fs::path p;
	size_t tries = 1 << mno;
	do {
		tries --;
		if(tries==0)
			throw std::runtime_error
	      ("Could not find a suitable name for the new directory");
		p = fs::unique_path(tmodel);
	} while(fs::exists(p));
	fs::create_directories(p);

	return p;
}


int AndroidTest::call(const std::string& cmd, const std::vector<std::string>& a)
{
	std::vector<std::string> args = {
		"-m", "androidctl"
	};
	if(! avd.empty()) {
		args.push_back("-a");
		args.push_back(avd);
	}
	if(! serial.empty()) {
		args.push_back("-s");
		args.push_back(serial);
	}
	for(auto& dex : dexen) {
		args.push_back("--dex");
		args.push_back(dex.string());
	}
	
	args.push_back(cmd);
	std::copy(a.begin(), a.end(), std::back_inserter(args));
	
	proc::child c(get_python3(),
		      proc::args = args);
	
	c.wait();
	return c.exit_code();
}
