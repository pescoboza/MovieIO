#include "Movie.hpp"
#include <memory>

Movie::Movie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre):
	Video{t_name, t_id, t_duration, t_genre, VideoType::MOVIE }{}

MoviePtr Movie::newMovie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre){
	return std::make_unique<Movie>(t_name, t_id, t_duration, t_genre);
}

void Movie::print(std::ostream& t_out) const{
	

}
