#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <memory>
#include <string>
#include <vector>

class Entry;
class SeriesEpisode;

using Ratings = std::vector<int>;
using EntryPtr = std::unique_ptr<Entry>;
using EpisodePtr = std::unique_ptr<SeriesEpisode>;
using Episodes = std::vector<EpisodePtr>;

enum class EntryType {
	VIDEO,
	MOVIE,
	SEIRES_EPISODE
};

class Entry {
protected:
	static int s_minRating;
	static int s_maxRating;

	std::string m_name;
	EntryType m_type;

	Entry(const std::string& t_name, int t_rating);
	

	int getRating() const;
	const std::string& getName() const;
	
	void rate(int t_rating);
};

class Movie {

};

class SeriesEpisode {

};

class Series {

};

#endif // !ENTRY_HPP