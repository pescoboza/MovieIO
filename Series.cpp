#include "Series.hpp"
#include <stdexcept>

Series::Series(const std::string& t_name) : m_name{t_name} {}

const std::string& Series::getName() const { return m_name; }

Series& Series::addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, unsigned t_season, unsigned t_episodeNum) {
	auto it{ m_seasons.find(t_season) };
	Season* season{ nullptr };
	if (it != m_seasons.end()) {
		season = it->second.get();
	}
	else {
		season = m_seasons.emplace(t_season, Season::newSeason(*this)).first->second.get();
	}

	season->addEpisode(t_name, t_id, t_duration, t_genre, t_episodeNum);
	return *this;
}

Episode* Series::getEpisode(unsigned t_season, unsigned t_episodeNum){
	auto ssn_it{ m_seasons.find(t_season) };
	if (ssn_it == m_seasons.end()) { return nullptr; }
	auto& episodes{ ssn_it->second->m_episodes };
	auto ep_it{ episodes.find(t_episodeNum)};
	if (ep_it == episodes.end()) { return nullptr; }
	return ep_it->second.get();
}

const Episode* Series::getEpisode(unsigned t_season, unsigned t_episodeNum) const {
	auto ssn_it{ m_seasons.find(t_season) };
	if (ssn_it == m_seasons.cend()) { return nullptr; }
	auto& episodes{ ssn_it->second->m_episodes };
	auto ep_it{ episodes.find(t_episodeNum) };
	if (ep_it == episodes.cend()) { return nullptr; }
	return ep_it->second.get();
}

std::vector<Season*>& Series::getAllSeasons(std::vector<Season*>& t_outSeasons) const{
	t_outSeasons.reserve(t_outSeasons.size() + m_seasons.size());
	for (const auto& ssnPair : m_seasons) {
		t_outSeasons.push_back(ssnPair.second.get());
	}
	return t_outSeasons;
}

std::vector<Episode*>& Series::getAllEpisodes(std::vector<Episode*>& t_outEpisodes) const{
	for (const auto& ssnPair : m_seasons) {
		ssnPair.second->getAllEpisodes(t_outEpisodes);
	}
	return t_outEpisodes;
}

std::vector<Video*>& Series::getAllEpisodes(std::vector<Video*>& t_outEpisodes) const {
	for (const auto& ssnPair : m_seasons) {
		ssnPair.second->getAllEpisodes(t_outEpisodes);
	}
	return t_outEpisodes;
}

Season::Season(Series& t_series) : m_series{ t_series } {}

SeasonPtr Season::newSeason(Series& t_series){	
	Season s{t_series};
	return std::make_unique<Season>(std::move(s));
}

Series& Season::getSeries() { return m_series; }

const Series& Season::getSeries() const { return m_series; }

Season& Season::addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, unsigned t_episodeNum){
	m_episodes.emplace(Episode::newEpisode(t_name, t_id, t_duration, t_genre, *this));
	return *this;
}

std::vector<Episode*>& Season::getAllEpisodes(std::vector<Episode*>& t_outEpisodes) const {
	t_outEpisodes.reserve(t_outEpisodes.size() + m_episodes.size());
	for (const auto& p : m_episodes) {
		t_outEpisodes.push_back(p.second.get());
	}
	return t_outEpisodes;
}

Episode::Episode(const std::string& m_name, const std::string& t_id, unsigned t_duration, Genre t_genre, Season& t_season) :
	Video{ m_name, t_id, t_duration, t_genre, VideoType::SERIES_EPISODE}, m_season{t_season}{}

EpisodePtr Episode::newEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, Season& t_season){
	Episode ep{ t_name, t_id, t_duration, t_genre, t_season };
	return std::make_unique<Episode>(std::move(ep));
}

Season& Episode::getSeason() { return m_season; }

const Season& Episode::getSeason() const { return m_season; }
