#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <memory>
#include <string>
#include <vector>

class Entry;
class SeriesEpisode;
class AtomicEntry;
class CollectionEntry;

using Ratings = std::vector<int>;
using EntryPtr = std::unique_ptr<Entry>;


enum class EntryType {
	VIDEO,
	MOVIE,
	SERIES,
	SERIES_EPISODE
};

enum class Genre {
	DRAMA,
	ACTION,
	MYSTERY
};


class Entry {
protected:
	static const float s_minRating;
	static const float s_maxRating;

	std::string m_name;
	EntryType m_type;
	Genre m_genre;

	Entry(const std::string& t_name, EntryType t_type, Genre t_genre);
	

	virtual void rate(int t_rating) const = 0;
	virtual float getRating() const = 0;
	virtual unsigned getLength() const = 0;
	
	int clipRating(float t_rating) const;

public: 
	const std::string& getName() const;
	EntryType getType() const;
	Genre getGenre() const;
};






#endif // !ENTRY_HPP