#include "Movie.hpp"

Movie::Movie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre):
	Video{t_name, t_id, t_duration, t_genre, VideoType::MOVIE }{}
