#include "Entry.hpp"

const float Entry::s_minRating{ 0.f };
const float Entry::s_maxRating{ 5.f };
Entry::Entry(const std::string& t_name, EntryType t_type, Genre t_genre) : m_name{ t_name }, m_type{ t_type }, m_genre{ t_genre }{}
const std::string& Entry::getName() const { return m_name; }
EntryType Entry::getType() const { return m_type; }
Genre Entry::getGenre() const { return m_genre; }
int Entry::clipRating(float t_rating) const {
	if (t_rating > s_maxRating) { return s_maxRating; }
	if (t_rating < s_minRating) { return s_minRating; }
	return t_rating;
}