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
	m_seasons.emplace(max, Season::newSeason(*this));
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
	m_seasons.emplace(t_number, Season::newSeason(*this));
	return *this;
}

Season::Season(Series& t_series) : m_series{ t_series } {}

SeasonPtr Season::newSeason(Series& t_series){	
	Season s{t_series};
	return std::make_unique<Season>(std::move(s));
}

Series& Season::getSeries() { return m_series; }

const Series& Season::getSeries() const { return m_series; }

Season& Season::addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre){
	m_episodes.emplace_back(Episode::newEpisode(t_name, t_id, t_duration, t_genre, *this));
	return *this;
}

Episode::Episode(const std::string& m_name, const std::string& t_id, unsigned t_duration, Genre t_genre, Season& t_season) :
	Video{ m_name, t_id, t_duration, t_genre, VideoType::SERIES_EPISODE}, m_season{t_season}{}

EpisodePtr Episode::newEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, Season& t_season){
	Episode ep{ t_name, t_id, t_duration, t_genre, t_season };
	return std::make_unique<Episode>(std::move(ep));
}

Season& Episode::getSeason() { return m_season; }

const Season& Episode::getSeason() const { return m_season; }
