#ifndef COLLECTION_ENTRY_HPP
#define COLLECTION_ENTRY_HPP

#include "Entry.hpp"

using SubEntryPtr = std::unique_ptr<Entry>;
using SubEntries = std::vector<SubEntryPtr>;

class CollectionEntry : public Entry {

	SubEntries m_subEntries;

public:
	float getRating() const;
	unsigned getLength() const;
};


#endif // !COLLECTION_ENTRY_HPP