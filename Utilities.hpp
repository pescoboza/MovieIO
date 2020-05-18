#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <algorithm>
#include <string>

namespace util {

	std::string& ltrim(std::string& t_outStr) {
		t_outStr.erase(t_outStr.begin(),
			std::find_if(t_outStr.begin(), t_outStr.end(),
				[](int t_ch) {return !std::isspace(t_ch); }));
		return t_outStr;
	}

	std::string& rtrim(std::string& t_outStr) {
		t_outStr.erase(std::find_if(t_outStr.rbegin(), t_outStr.rend(),
			[](int t_ch) {return !std::isspace(t_ch); }).base(), t_outStr.end());
		return t_outStr;
	}

	std::string& trim(std::string& t_outStr) {
		return ltrim(rtrim(t_outStr));
	}
};

#endif // !UTILITIES_HPP