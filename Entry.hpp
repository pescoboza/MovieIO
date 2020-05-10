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
	SERIES,
	SERIES_EPISODE
};



class Entry {
protected:
	static const int s_minRating;
	static const int s_maxRating;

	std::string m_name;
	EntryType m_type;

	Entry(const std::string& t_name, int t_rating);
	

	virtual void rate(int t_rating) const = 0;
	virtual int getRating() const = 0;
	virtual unsigned getLength() const = 0;


public: 
	const std::string& getName() const;
	EntryType getType() const;
};
const int Entry::s_minRating{0};
const int Entry::s_maxRating{5};
const std::string& Entry::getName() const { return m_name; }
EntryType Entry::getType() const { return m_type; }

// Class for single elements such a move or video, in opposition to series which are collections
class AtomicEntry : public Entry{
protected:
	int m_rating;
	unsigned m_length;

public:
	int getRating() const;
	unsigned getLength() const;
};

int AtomicEntry::getRating() const { return m_rating; }
unsigned AtomicEntry::getLength() const { return m_length; }

class CollectionEntry : public Entry {
	
	int getRating() const = 0;
	unsigned getLength() const = 0;
};





class Movie :  public AtomicEntry {
};

class SeriesEpisode :  public AtomicEntry {

};

class Vide :  public AtomicEntry {

};

class Series : public CollectionEntry{
	Episodes m_episodes;
};

#endif // !ENTRY_HPP