#ifndef SERIES_HPP
#define SERIES_HPP


#include "Video.hpp"
#include <memory>
#include <string>
#include <vector>

class Episode;
class Season;

using EpisodePtr = std::unique_ptr<Episode>;
using Episodes = std::vector<EpisodePtr>;

using SeasonPtr = std::unique_ptr<Season>;
using Seasons = std::vector<SeasonPtr>;

class Series {
	std::string m_name;
	Seasons m_seasons;
};

class Season {
	Series& m_series;
	unsigned m_number;
	Episodes m_episodes;
};


class Episode : public Video {
	Season& m_season;
};

#endif // !SERIES_HPP
