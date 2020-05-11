#ifndef SERIES_HPP
#define SERIES_HPP


#include "Video.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

class Episode;
class Season;

using EpisodePtr = std::unique_ptr<Episode>;
using Episodes = std::vector<EpisodePtr>;

using SeasonPtr = std::unique_ptr<Season>;
using Seasons = std::map<unsigned, SeasonPtr>;

class Series {
	std::string m_name;
	Seasons m_seasons;
public:
	Series(const std::string& t_name);
	Series& addSeason();
	Series& addSeason(unsigned t_number);
	std::vector<Season*>& getAllSeasons(std::vector<Season*>& t_outSeasons) const;
	std::vector<Episode*>& getAllEpisodes(std::vector<Episode*>& t_outEpisodes) const;
};

class Season {
	friend class Series;

	Series& m_series;
	Episodes m_episodes;

	Season(Series& t_series);
	static SeasonPtr newSeason(Series& t_series);

public:
	Series& getSeries();
	const Series& getSeries() const;
	Season& addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre);
	std::vector<Episode*>& getAllEpisodes(std::vector<Episode*>& t_outEpisodes) const;
	
};


class Episode : public Video {
	friend class Season;
	Season& m_season;

	Episode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, Season& t_season);
	static EpisodePtr newEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, Season& t_season);
public:
	Season& getSeason();
	const Season& getSeason() const;
};

#endif // !SERIES_HPP
