#ifndef ATOMIC_ENTRY_HPP
#define ATOMIC_ENTRY_HPP

#include "Entry.hpp"

// Class for single elements such a move or video, in opposition to series which are collections
class AtomicEntry : public Entry {
protected:
	float m_rating;
	unsigned m_length;

public:
	float getRating() const;
	unsigned getLength() const;
};

#endif // !ATOMIC_ENTRY_HPP
