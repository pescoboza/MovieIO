#ifndef VIDEO_DATA_HOLDER_HPP
#define VIDEO_DATA_HOLDER_HPP

#include "Video.hpp"
#include "Movie.hpp"
#include "Series.hpp"
#include <algorithm>
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

class Action;
enum class SortCriteria;
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
using NumArgs = unsigned;
using CmdParamsMap = std::unordered_map<std::string, NumArgs>;
using CmdsMap = std::unordered_map<ActionBindings, CmdParamsMap>;
using PtrToConstStrVec = std::vector<const std::string*>;
using Parameters = std::vector<std::pair<const std::string*, PtrToConstStrVec>>;
using SortMemo = std::vector<std::pair<SortCriteria, bool>>; // true -> descending, false -> descending
using StrVec = std::vector<std::string>;

enum class SearchCategories {
	NAME,
	ID,
	RATING,
	DURATION,
	SERIES
};

enum class ActionBindings {
	SEARCH,
	RATE,
	SORT,
	CLEAR,
	HELP,
	QUIT
};

enum class SortCriteria {
	NAME,
	ID,
	RATING,
	DURATION
};


class VideoDataHolder {

	VideosMap m_videosById;
	VideosVec m_videosVec;
	VideosVec m_buffer;

	MoviesVec m_movies;
	SeriesMap m_series;

	ActionStrMap m_actions;
	CmdsMap m_cmds;

	std::ostream& m_out;
	std::istream& m_in;
	
	static const std::string s_msg_loading;
	static const std::string s_msg_startScreen;
	static const std::string s_msg_unknownCmdErr;
	static const std::string s_msg_help;
	static const std::string s_msg_notFoundErr;
	static const std::string s_msg_enterCmd;

public:
	VideoDataHolder(std::ostream& t_out = std::cout , std::istream& t_in = std::cin);
	void parseInfoFromFile(const std::string& t_filename);
	std::pair<ActionBindings,bool> structureCommand(const StrVec& t_words, Parameters& t_outParams);
	void executeAction(ActionBindings t_action, const Parameters& t_params);
	void start();


	const Video* getVideoById(const std::string& t_videoId) const; // Returns nullptr if video is not found
	Video* getVideoById(const std::string& t_videoId); // Returns nullptr if video is not found

	const Series* getSeriesByName(const std::string& t_name) const; // Returns nullptr if video is not found
	Series* getSeriesByName(const std::string& t_name); // Returns nullptr if video is not found

	std::vector<Movie*>& getMovies(std::vector<Movie*>& t_outMovies);


	// Returns all the video entries that match the filter function
	template <typename Functor>
	static VideosVec& filter(Functor t_filter, const VideosVec& t_inVideos, VideosVec& t_outVideos);

	static void printVideos(const VideosVec& t_videos, unsigned t_numEntries, bool t_printHeader = true, std::ostream& t_out = std::cout);

	VideosVec& filterVideos(
		const VideosVec& t_inVideos,
		VideosVec& t_outVideos,
		const std::string& t_name = "",
		const std::string& t_id = "",
		const std::string& t_genre = "",
		const std::string& t_series = "",
		const std::pair<float, float>& t_rating = { Video::s_minRating, Video::s_maxRating },
		const std::pair<int, int>& t_duration = { Video::s_minDuration, Video::s_maxDuration }
	) const;


	static VideosVec& sortVideosBy(const VideosVec& t_inVideos, VideosVec& t_outVideos, const SortMemo& t_criteria);

private:
	static const std::string s_initMsg;

	void registerVideo(Video* t_video);

	VideoDataHolder& addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, const std::string& t_series, unsigned t_season, unsigned t_episodeNum, const Ratings& t_ratings = {});
	VideoDataHolder& addMovie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, const Ratings& t_ratings = {});

	std::string input();
	std::pair<ActionBindings, bool> strToActionBinding(const std::string& t_input) const;

public:
	 
	struct ParametersSearch {
		std::string m_name;
		std::string m_id;
		std::string m_genre;
		std::string m_minrating;
		std::string m_maxrating;
		std::string m_minduration;
		std::string m_maxduration;
		std::string m_series;
	};
	static const ParametersSearch s_params_search;

	 struct ParametersSort {
		std::string m_name;
		std::string m_id;
		std::string m_rating;
		std::string m_duration;
		std::unordered_set<std::string> m_ascending;
		std::unordered_set<std::string> m_descending;
	 };
	 static const ParametersSort s_params_sort;

	

	 struct ParametersRate {
		 std::string m_id;
		 std::string m_rating;
		 std::string m_err_rating;
	 };
	 static const ParametersRate  s_params_rate;

private:
	static bool isLesser(SortCriteria t_criterion, const Video& t_a, const Video& t_b);
	static bool isEqual(SortCriteria t_criterion, const Video& t_a, const Video& t_b);
	
	void action_search(const Parameters& t_params);
	void action_rate(const Parameters& t_params);
	void action_sort(const Parameters& t_params);
	void action_clear(const Parameters& t_params = {});
	void action_help(const Parameters& t_params = {});
	void action_quit(const Parameters& t_params = {});
};

template<typename Functor>
inline VideosVec& VideoDataHolder::filter(Functor t_filter, const VideosVec& t_inVideos, VideosVec& t_outVideos){

	for (Video* vidPtr : t_inVideos) {
		if (!vidPtr) {
			throw std::runtime_error{ "Invalid video pointer.\n" };
		}

		if (t_filter(*vidPtr)) {
			t_outVideos.push_back(vidPtr);
		}
	}

	return t_outVideos;
}


#endif // !VIDEO_DATA_HOLDER_HPP