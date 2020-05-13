#ifndef MOVIE_HPP
#define MOVIE_HPP

#include "Video.hpp"
#include <memory>

class Movie;

using MoviePtr = std::unique_ptr<Movie>;

class Movie : public Video{
public:
	Movie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre);
	static MoviePtr newMovie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre);
	void print(std::ostream& t_out = std::cout) const;
};

#endif // !MOVIE_HPP