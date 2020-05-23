#include "Utilities.hpp"
#include <algorithm>

namespace utl {

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

	std::vector<std::string> getWords(const std::string& t_str, bool t_respectQuoted) {
		std::stringstream sstr{ t_str };
		std::string buff;
		std::vector<std::string> words;

		if (t_respectQuoted) {
			while (sstr >> std::quoted(buff)) {
				words.emplace_back(std::move(buff));
			}		
		}
		else {
			while (sstr >> buff) {
				words.emplace_back(std::move(buff));
			}
		}
		return std::move(words);
	}

	bool getNWordsAfterKeyword(const std::vector<std::string>& t_words, 
								std::vector<const std::string*>& t_outWords, 
								const std::string& t_keyword, 
								unsigned t_n) {

		unsigned i{ 0U };
		auto size{ t_words.size() };
		bool found{ false };
		for (; i < size; i++) {
			if (t_words[i] == t_keyword) {
				found = true;
				break;
			}
		}

		if (!found) { return false; }

		unsigned npos{ i + t_n };
		for (; i < size && i < npos; i++) {
			t_outWords.push_back(&t_words[i]);
		}

		return true;
	}
	bool lexCompStrs(const std::string& t_a, const std::string& t_b){
		return std::lexicographical_compare(t_a.cbegin(), t_a.cend(), t_b.cbegin(), t_b.cend());
	}
};