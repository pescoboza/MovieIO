#ifndef SERIES_HPP
#define SERIES_HPP


#include "Video.hpp"
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Series;
class Season;
class Episode;

using SeriesPtr = std::unique_ptr<Series>;

using EpisodePtr = std::unique_ptr<Episode>;
using Episodes = std::map<unsigned, EpisodePtr>;

using SeasonPtr = std::unique_ptr<Season>;
using Seasons = std::map<unsigned, SeasonPtr>;

class Series {
	std::string m_name;
	Seasons m_seasons;
public:
	Series(const std::string& t_name);
	static SeriesPtr newSeries(const std::string& t_name);
	const std::string& getName() const;
	Series& addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, unsigned t_season, unsigned t_episodeNum);
	Episode* getEpisode(unsigned t_season, unsigned t_episodeNum);
	bool getEpisodeSeasonAndNum(const Episode& t_episode, unsigned& t_outSeason, unsigned& t_outEpNum) const;
	const Episode* getEpisode(unsigned t_season, unsigned t_episodeNum) const;
	std::vector<Season*>& getAllSeasons(std::vector<Season*>& t_outSeasons) const;
	std::vector<Episode*>& getAllEpisodes(std::vector<Episode*>& t_outEpisodes) const;
	std::vector<Video*>& getAllEpisodes(std::vector<Video*>& t_outEpisodes) const;
};

class Season {
	friend class Series;

	Series& m_series;
	Episodes m_episodes;
	unsigned m_seasonNum;

	Season(unsigned t_seasonNum, Series& t_series);
	static SeasonPtr newSeason(unsigned t_seasonNum, Series& t_series);

public:
	Series& getSeries();
	const Series& getSeries() const;
	Season& addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, unsigned t_episodeNum);
	unsigned getSeasonNum() const;
	
	std::vector<Episode*>& getAllEpisodes(std::vector<Episode*>& t_outEpisodes) const;
	
};


class Episode : public Video {
	friend class Season;
	Season& m_season;
	unsigned m_episodeNum;

	Episode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, unsigned t_episodeNum, Season& t_season);
	static EpisodePtr newEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, unsigned t_episodeNum, Season& t_season);
public:
	Season& getSeason();
	const Season& getSeason() const;
	const std::string& getSeriesName()const;
	unsigned getEpisodeNum() const;
	void print(std::ostream& t_out = std::cout) const;

	friend std::ostream& operator<<(std::ostream& t_out, const Episode& t_episode);
};

#endif // !SERIES_HPP
