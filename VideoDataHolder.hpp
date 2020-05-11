#ifndef VIDEO_DATA_HOLDER_HPP
#define VIDEO_DATA_HOLDER_HPP

#include "Video.hpp"
#include "Movie.hpp"
#include "Series.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

using Videos = std::unordered_map<std::string, Video*>; // non data-owning ptr
using MoviePtr = std::unique_ptr<Movie>;
using MoviesVec = std::vector<MoviePtr>;
using SeriesPtr = std::unique_ptr<Series>;
using SeriesVec = std::vector<SeriesPtr>;

class VideoDataHolder {
	
	Videos m_videos;
	MoviesVec m_movies;
	SeriesVec m_series;

public:
	VideoDataHolder();
	
	VideoDataHolder& addSeries(SeriesPtr t_series);
	VideoDataHolder& addMovie(MoviePtr t_movie);

	const Video& getVideo(const std::string& t_videoId) const;
	Video& getVideo(const std::string& t_videoId);

};

#endif // !VIDEO_DATA_HOLDER_HPP