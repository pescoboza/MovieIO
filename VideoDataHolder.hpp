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
using SeriesMap = std::unordered_map<std::string,SeriesPtr>;

class VideoDataHolder {
	
	Videos m_videos;
	MoviesVec m_movies;
	SeriesMap m_series;

public:
	VideoDataHolder();
	
	VideoDataHolder& addSeries(SeriesPtr t_series);
	VideoDataHolder& addMovie(MoviePtr t_movie);

	const Video* getVideoById(const std::string& t_videoId) const; // Returns nullptr if video is not found
	Video* getVideoById(const std::string& t_videoId); // Returns nullptr if video is not found

	const Series* getSeriesByName(const std::string& t_name) const; // Returns nullptr if video is not found
	Series* getSeriesByName(const std::string& t_name); // Returns nullptr if video is not found

	std::vector<Movie*>& getMovies(std::vector<Movie*>& t_outMovies);


};

#endif // !VIDEO_DATA_HOLDER_HPP