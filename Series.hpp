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
};

class Season {
	Series& m_series;
	Episodes m_episodes;

public:
	Season(Series& t_series);
	Series& getSeries();
	const Series& getSeries() const;
};


class Episode : public Video {
	
	Season& m_season;

public:
	Episode(const std::string& m_name, const std::string& t_id, unsigned t_duration, Genre t_genre, Season& t_season);
	Season& getSeason();
	const Season& getSeason() const;
};

#endif // !SERIES_HPP
