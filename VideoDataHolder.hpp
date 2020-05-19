#ifndef VIDEO_DATA_HOLDER_HPP
#define VIDEO_DATA_HOLDER_HPP

#include "Video.hpp"
#include "Movie.hpp"
#include "Series.hpp"
#include <algorithm>
#include <map>
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

class Action;
enum class SortVideosBy;
enum class Actions;
enum class ActionBindings;

using VideoPtr  = std::unique_ptr<Video>;
using VideosMap = std::unordered_map<std::string, Video*>; // non data-owning ptr
using VideosVec = std::vector<Video*>; 
using MoviePtr = std::unique_ptr<Movie>;
using MoviesVec = std::vector<MoviePtr>;
using SeriesPtr = std::unique_ptr<Series>;
using SeriesMap = std::unordered_map<std::string,SeriesPtr>;
using ActionStrMap = std::multimap<std::string, ActionBindings>;
using ActionPtr = std::unique_ptr<Action>;
using ActionMap = std::unordered_map<ActionBindings, ActionPtr>;

enum class SearchCategories {
	NAME,
	ID,
	RATING,
	LENGTH,
	SERIES
};

enum class ActionBindings {
	SEARCH,
	FILTER,
	RATE,
	SORT,
	CLEAR,
	HELP,
	QUIT
};

class Action {

	ActionBindings m_boundAction;
	std::string m_desc;
	std::string m_usage;

public:
	Action(ActionBindings t_boundAction, const std::string& t_desc, const std::string& t_usage);

	const std::string& getDesc() const;
	const std::string& getUsage() const;
	
};

enum class SortVideosBy {
	NAME,
	ID,
	RATING,
	LENGTH
};


class VideoDataHolder {
	
	VideosMap m_videosById;
	VideosVec m_videosVec;

	MoviesVec m_movies;
	SeriesMap m_series;

	ActionStrMap m_actions;
	ActionMap m_actionBindings;

	static const std::string s_startScreen;
	static const std::string s_unkownCmdErrMsg;
	static const std::string s_helpMsg;

public:
	VideoDataHolder();
	void parseInfoFromFile(const std::string& t_filename);
	void start(std::ostream& t_out = std::cout, std::istream& t_in = std::cin);


	const Video* getVideoById(const std::string& t_videoId) const; // Returns nullptr if video is not found
	Video* getVideoById(const std::string& t_videoId); // Returns nullptr if video is not found

	const Series* getSeriesByName(const std::string& t_name) const; // Returns nullptr if video is not found
	Series* getSeriesByName(const std::string& t_name); // Returns nullptr if video is not found
	
	std::vector<Movie*>& getMovies(std::vector<Movie*>& t_outMovies);


	// Returns all the video entries that match the filter function
	template <typename Functor>
	static VideosVec& filter(Functor t_filter, const VideosVec& t_inVideos, VideosVec& t_outVideos);
	
	static void printVideos(const VideosVec& t_videos, unsigned t_numEntries, bool t_printHeader = true, std::ostream& t_out = std::cout);

	VideosVec& filterVideos(VideosVec& t_outVideos, const std::string& t_name = "",
		const std::string& t_genre = "",
		const std::string& t_series = "",
		const std::pair<float, float>& t_rating = {Video::s_minRating, Video::s_maxRating}) const;

	
	static VideosVec& sortVideosBy(const VideosVec& t_inVideos, VideosVec& t_outVideos, SortVideosBy t_criteria, bool t_ascending = true);

private:
	static const std::string s_initMsg;

	void registerVideo(Video* t_video);

	VideoDataHolder& addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, const std::string& t_series, unsigned t_season, unsigned t_episodeNum, const Ratings& t_ratings = {});
	VideoDataHolder& addMovie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, const Ratings& t_ratings = {});

	static std::string input(std::istream& t_in = std::cin);
	std::pair<ActionBindings, bool> strToActionBinding(const std::string& t_input) const;
};

template<typename Functor>
inline VideosVec& VideoDataHolder::filter(Functor t_filter, const VideosVec& t_inVideos, VideosVec& t_outVideos){
	for (const auto vidPtr : t_inVideos) {
		const auto& constVid = *vidPtr;
		if (t_filter(constVid)) {
			t_outVideos.push_back(vidPtr);
		}
	}
	return t_outVideos;
}


#endif // !VIDEO_DATA_HOLDER_HPP