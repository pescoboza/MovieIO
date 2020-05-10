#ifndef SERIES_HPP
#define SERIES_HPP

#include <memory>
#include <vector>
#include "CollectionEntry.hpp"
#include "Episode.hpp"

class Season;

using EpisodePtr = std::unique_ptr<Episode>;
using Episodes = std::vector<EpisodePtr>;

using SeasonPtr = std::unique_ptr<Season>;
using Seasons = std::vector<SeasonPtr>;

class Season : public CollectionEntry {
	Episodes m_episodes;
};

class Series : public CollectionEntry {
	Seasons m_seasons;
};

#endif // !SERIES_HPP