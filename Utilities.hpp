#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace utl {

	std::string& ltrim(std::string& t_outStr);

	std::string& rtrim(std::string& t_outStr);

	std::string& trim(std::string& t_outStr);

	std::vector<std::string> getWords(const std::string& t_str, bool t_respectQuoted = true);

	bool getNWordsAfterKeyword(const std::vector<std::string>& t_words,
		std::vector<const std::string*>& t_outWords,
		const std::string& t_keyword,
		unsigned t_n);

	bool lexCompStrs(const std::string& t_a, const std::string& t_b);

	const std::string emptyStr{ "" };
};
#endif // !UTILITIES_HPP