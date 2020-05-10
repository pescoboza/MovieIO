#include "CollectionEntry.hpp"

// Returns average of all ratings of all episodes
float CollectionEntry::getRating() const {
	int accRating{ 0 };
	int numEntries{ 0 };
	for (const auto& subEntry : m_subEntries) {
		accRating += subEntry->getRating;
		numEntries++; // TODO: Decide what formula will be used to calculate raing of CollectionEntry.
	}
	return;
}

unsigned CollectionEntry::getLength()const {
	unsigned accLength{ 0U };
	for (const auto& subEntry : m_subEntries) {
		accLength += subEntry->getLength();
	}
	return accLength;
}
