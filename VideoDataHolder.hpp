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

	// Returns all the video entries that match the filter function
	template <typename Functor>
	std::vector<Video*>& filter(Functor t_filter , std::vector<Video*>& t_outVideos);

	std::vector<Video*>& getVideosOfGenre(Genre t_genre, std::vector<Video*>& t_outVideos);
	std::vector<Video*>& getVideosOfRating(float t_min, float t_max, std::vector<Video*>& t_outVideos);
	
};

template<typename Functor>
inline std::vector<Video*>& VideoDataHolder::filter(Functor t_filter, std::vector<Video*>& t_outVideos){
	for (const auto& vidPair : m_videos) {
		if (t_filter(*vidPair.second)) {
			t_outVideos.push_back(vidPair.second);
		}
	}
	return t_outVideos;
}

#endif // !VIDEO_DATA_HOLDER_HPP