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
	
	Videos m_videosById;
	Videos m_videosByName;

	MoviesVec m_movies;
	SeriesMap m_series;

public:
	VideoDataHolder();

	void parseInfoFromFile(const std::string& t_filename);
	
	VideoDataHolder& addSeries(SeriesPtr t_series);
	VideoDataHolder& addMovie(MoviePtr t_movie);

	const Video* getVideoById(const std::string& t_videoId) const; // Returns nullptr if video is not found
	Video* getVideoById(const std::string& t_videoId); // Returns nullptr if video is not found

	const Series* getSeriesByName(const std::string& t_name) const; // Returns nullptr if video is not found
	Series* getSeriesByName(const std::string& t_name); // Returns nullptr if video is not found
	
	std::vector<Movie*>& getMovies(std::vector<Movie*>& t_outMovies);

	// Returns all the video entries that match the filter function
	template <typename Functor>
	static std::vector<Video*>& filter(Functor t_filter, std::vector<Video*> t_inVideos, std::vector<Video*>& t_outVideos);

	static std::vector<Video*>& getVideosOfGenre(Genre t_genre, std::vector<Video*>& t_inVideos, std::vector<Video*>& t_outVideos);
	static std::vector<Video*>& getVideosOfRating(float t_min, float t_max, std::vector<Video*>& t_inVideos, std::vector<Video*>& t_outVideos);
	
private:
	VideoDataHolder& addVideo(Video* t_video);
	VideoDataHolder& addVideos(const std::vector<Video*>& t_videos);
};

template<typename Functor>
inline std::vector<Video*>& VideoDataHolder::filter(Functor t_filter, std::vector<Video*> t_inVideos, std::vector<Video*>& t_outVideos){
	for (auto vidPtr : t_inVideos) {
		if (t_filter(*vidPtr)) {
			t_outVideos.push_back(vidPtr);
		}
	}
	return t_outVideos;
}

#endif // !VIDEO_DATA_HOLDER_HPP