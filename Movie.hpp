#ifndef MOVIE_HPP
#define MOVIE_HPP

#include "Video.hpp"

class Movie : public Video{
public:
	Movie(const std::string& m_name, const std::string& t_id, unsigned t_duration, Genre t_genre);
};

#endif // !MOVIE_HPP