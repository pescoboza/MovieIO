#include "Series.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>

Series::Series(const std::string& t_name) : m_name{t_name} {}

SeriesPtr Series::newSeries(const std::string& t_name){
	return std::make_unique<Series>(t_name);
}

const std::string& Series::getName() const { return m_name; }

Series& Series::addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, unsigned t_seasonNum, unsigned t_episodeNum) {
	auto &season{ *m_seasons.emplace(t_seasonNum, std::move(Season::newSeason(t_seasonNum, *this))).first->second.get() };
	season.addEpisode(t_name, t_id, t_duration, t_genre, t_episodeNum);
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
		std::vector<Video*> videos;
		auto &episodes{ ssnPair.second->m_episodes};
		for (auto& p : episodes) {
			t_outEpisodes.push_back(static_cast<Video*>(p.second.get()));
		}
	}
	return t_outEpisodes;
}

Season::Season(unsigned t_seasonNum, Series& t_series) : m_seasonNum{t_seasonNum}, m_series { t_series }{}

SeasonPtr Season::newSeason(unsigned t_seasonNum, Series& t_series){
	Season s{t_seasonNum,t_series};
	return std::make_unique<Season>(s);
}

Series& Season::getSeries() { return m_series; }

const Series& Season::getSeries() const { return m_series; }


Season& Season::addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, unsigned t_episodeNum){
	m_episodes.emplace(t_episodeNum, EpisodesMap{});
	m_episodes.emplace(t_episodeNum, std::move(Episode::newEpisode(t_name, t_id, t_duration, t_genre, t_episodeNum, *this)));
	return *this;
}

unsigned Season::getSeasonNum() const {	return m_seasonNum;}


std::vector<Episode*>& Season::getAllEpisodes(std::vector<Episode*>& t_outEpisodes) const {
	t_outEpisodes.reserve(t_outEpisodes.size() + m_episodes.size());
	for (const auto& p : m_episodes) {
		t_outEpisodes.push_back(p.second.get());
	}
	return t_outEpisodes;
}

Episode::Episode(const std::string& m_name, const std::string& t_id, unsigned t_duration, Genre t_genre, unsigned t_episodeNum, Season& t_season) :
	Video{ m_name, t_id, t_duration, t_genre, VideoType::SERIES_EPISODE }, m_episodeNum{t_episodeNum}, m_season{ t_season }{}

EpisodePtr Episode::newEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, unsigned t_episodeNum, Season& t_season){
	Episode ep{ t_name, t_id, t_duration, t_genre, t_episodeNum, t_season };
	return std::make_unique<Episode>(std::move(ep));
}

Season& Episode::getSeason() { return m_season; }

const Season& Episode::getSeason() const { return m_season; }

const std::string& Episode::getSeriesName() const{
	return m_season.getSeries().getName();
}

unsigned Episode::getEpisodeNum() const { return m_episodeNum;}

void Episode::print(std::ostream& t_out) const{
	const auto& sep{ s_tbl.m_separator };
	const auto& t{ s_tbl };
	std::string seriesNameSep{ " - " };
	
	unsigned ssnNum{m_season.getSeasonNum()};

	std::string ssnNumStr{std::to_string(ssnNum)};
	if (ssnNumStr.size() == 1U) { ssnNumStr = '0' + ssnNumStr; }
	ssnNumStr = 'S' + ssnNumStr;

	std::string m_episodeNumStr{ std::to_string(m_episodeNum) };
	if (m_episodeNumStr.size() == 1U) { m_episodeNumStr = '0' + m_episodeNumStr; }
	m_episodeNumStr = 'E' + m_episodeNumStr;

	std::string enrichedName { ssnNumStr + ' ' +  m_episodeNumStr + seriesNameSep + getSeriesName() +  seriesNameSep + m_name};

	t_out << sep << ' ' <<
		std::left << std::setw(t.m_id) << m_id << sep <<
		std::left << std::setw(t.m_name) << enrichedName << sep <<
		std::left << std::setw(t.m_duration) << formattedDuration() << sep <<
		std::left << std::setw(t.m_genre) << getStrFromGenre(m_genre) << sep <<
		std::left << std::setw(t.m_rating) << std::fixed << std::setprecision(1) << getRating() << sep <<
		std::left << std::setw(t.m_type) << getStrFromVideoType(m_type) << sep;
}

std::ostream& operator<<(std::ostream& t_out, const Episode& t_episode){
	t_episode.print(t_out);
	return t_out;
}
