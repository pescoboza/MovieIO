#ifndef VIDEO_DATA_HOLDER_HPP
#define VIDEO_DATA_HOLDER_HPP

#include "Video.hpp"
#include "Movie.hpp"
#include "Series.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

using VideosMap = std::unordered_map<std::string, Video*>; // non data-owning ptr
using VideosVec = std::vector<Video*>; 
using MoviePtr = std::unique_ptr<Movie>;
using MoviesVec = std::vector<MoviePtr>;
using SeriesPtr = std::unique_ptr<Series>;
using SeriesMap = std::unordered_map<std::string,SeriesPtr>;

class VideoDataHolder {
	
	VideosMap m_videosById;
	VideosMap m_videosByName;
	VideosVec m_videosVec;

	MoviesVec m_movies;
	SeriesMap m_series;

public:
	VideoDataHolder();
	void parseInfoFromFile(const std::string& t_filename);
	void start();


	const Video* getVideoById(const std::string& t_videoId) const; // Returns nullptr if video is not found
	Video* getVideoById(const std::string& t_videoId); // Returns nullptr if video is not found

	const Series* getSeriesByName(const std::string& t_name) const; // Returns nullptr if video is not found
	Series* getSeriesByName(const std::string& t_name); // Returns nullptr if video is not found
	
	std::vector<Movie*>& getMovies(std::vector<Movie*>& t_outMovies);


	// Returns all the video entries that match the filter function
	template <typename Functor>
	static VideosVec& filter(Functor t_filter, VideosVec t_inVideos, VideosVec& t_outVideos);
	static VideosVec& getVideosOfGenre(Genre t_genre, VideosVec& t_inVideos, VideosVec& t_outVideos);
	static VideosVec& getVideosOfRating(float t_min, float t_max, VideosVec& t_inVideos, VideosVec& t_outVideos);
	
	static void printVideos(const VideosVec& t_videos);

	void getVideos(const std::string& t_name = "",
		const std::string& t_genre = "",
		const std::string& t_series = "",
		const std::pair<float, float>& t_rating = {Video::s_minRating, Video::s_maxRating}) const;


private:
	static const std::string s_initMsg;


	VideoDataHolder& registerVideo(Video* t_video);
	VideoDataHolder& addVideos(const VideosVec& t_videos);

	VideoDataHolder& addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, const std::string& t_series, unsigned t_season, unsigned t_episodeNum);
	VideoDataHolder& addMovie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre);

	static std::string input(std::istream& t_in = std::cin);
};

template<typename Functor>
inline VideosVec& VideoDataHolder::filter(Functor t_filter, VideosVec t_inVideos, VideosVec& t_outVideos){
	for (auto vidPtr : t_inVideos) {
		if (t_filter(*vidPtr)) {
			t_outVideos.push_back(vidPtr);
		}
	}
	return t_outVideos;
}

#endif // !VIDEO_DATA_HOLDER_HPP