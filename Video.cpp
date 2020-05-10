#include "Video.hpp"

const std::string& Video::getName() const { return m_name; }

float Video::getRating() const {
	float accRating{ 0.f };
	int size{ 0 };
	for (const auto& rating : m_ratings) {
		accRating += rating;
		size++;
	}
	return accRating / size;
}

VideoType Video::getType() const{ return m_type;}

unsigned Video::getDuration() const {return m_duration;}

void Video::rate(float t_rating) { m_ratings.emplace_back(t_rating); }
