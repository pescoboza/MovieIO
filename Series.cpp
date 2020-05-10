#include "Series.hpp"
#include <stdexcept>

Series::Series(const std::string& t_name) : m_name{t_name} {}

Series& Series::addSeason() { 
	unsigned max{ 1U };
	for (const auto& p : m_seasons) {
		if (p.first > max) {
			max = p.first;
		}
	}
	m_seasons.emplace(max, *this);
	return *this;
}

Series& Series::addSeason(unsigned t_number){
	if (t_number == 0U) {
		throw std::invalid_argument{"Season number cannot be 0."};
	}
	auto it{ m_seasons.find(t_number) };
	if (it != m_seasons.end()) {
		m_seasons.erase(it);
	}
	m_seasons.emplace(t_number, *this);
	return *this;
}

Series& Season::getSeries() { return m_series; }

const Series& Season::getSeries() const { return m_series; }
