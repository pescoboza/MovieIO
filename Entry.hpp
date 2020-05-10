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
using SubEntryPtr = std::unique_ptr<AtomicEntry>;
using SubEntries = std::vector<SubEntryPtr>;

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
	static const int s_minRating;
	static const int s_maxRating;

	std::string m_name;
	EntryType m_type;
	Genre m_genre;

	Entry(const std::string& t_name, EntryType t_type, Genre t_genre);
	

	virtual void rate(int t_rating) const = 0;
	virtual int getRating() const = 0;
	virtual unsigned getLength() const = 0;
	
	int clipRating(int t_rating) const;

public: 
	const std::string& getName() const;
	EntryType getType() const;
	Genre getGenre() const;
};
const int Entry::s_minRating{0};
const int Entry::s_maxRating{5};
Entry::Entry(const std::string& t_name, EntryType t_type, Genre t_genre) : m_name{ t_name }, m_type{ t_type }, m_genre{t_genre}{}
const std::string& Entry::getName() const { return m_name; }
EntryType Entry::getType() const { return m_type; }
Genre Entry::getGenre() const { return m_genre; }
int Entry::clipRating(int t_rating) const {
	if (t_rating > s_maxRating) { return s_maxRating; }
	if (t_rating < s_minRating) { return s_minRating; }
	return t_rating;
}


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
	
	SubEntries m_subEntries;

public:
	int getRating() const;
	unsigned getLength() const;
};

// Returns average of all ratings of all episodes
int CollectionEntry::getRating() const {
	int accRating{ 0 };
	int numEntries{ 0 };
	for (const auto& subEntry : m_subEntries) {
		accRating += subEntry->getRating;
		numEntries++; // TODO: Decide what formula will be used to calculate raing of CollectionEntry.
	}
	return ;
}

unsigned CollectionEntry::getLength()const {
	unsigned accLength{ 0U };
	for (const auto& subEntry : m_subEntries) {
		accLength += subEntry->getLength();
	}
	return accLength;
}




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