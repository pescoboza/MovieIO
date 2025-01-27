#include "VideoDataHolder.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>
#include "json.hpp" // Credits to https://github.com/nlohmann/json
#include "Utilities.hpp"

namespace js = nlohmann;
#include <unordered_set>
#include <unordered_map>
#include <string>

const VideoDataHolder::ParametersSearch VideoDataHolder::s_params_search{ "name", "id", "genre", "minrating", "maxrating", "minduration", "maxduration", "series" };
const VideoDataHolder::ParametersSort VideoDataHolder::s_params_sort{ "name", "id", "rating", "duration", { "descending", "-", "des", "false", "0" }, { "ascending", "+", "asc", "true", "1" } };
const VideoDataHolder::ParametersRate VideoDataHolder::s_params_rate{"id", "rating", "Invalid rating value."};

const std::string VideoDataHolder::s_msg_loading{"Loading data..."};
const std::string VideoDataHolder::s_msg_notFoundErr{ "ERROR: Could not find a result for the given query." };
const std::string VideoDataHolder::s_msg_help{ 
R"(+--------------------------------------------------------------------------------------------------------+
|                                                                                                        |
| Welcome to MovieIO, to use it type a command:                                                          |
|    Command:                                                                                            |
|    - search : Captures in the buffer the videos matching the query.                                    |
|                                                                                                        |
|        Arguments:                                                                                      |
|                                                                                                        |
|       - name        : Name of the video.                                                               |
|        - id          : Unique video ID.                                                                |
|        - genre          : Video genre classification.                                                  |
|        - series      : Series of which the video is an episode (if applicable).                        |
|        - minrating      : Minimum average rating.                                                      |
|        - maxrating      : Maximum average rating.                                                      |
|        - minduration : Minimum duration in minutes.                                                    |
|        - maxduration : Maximum duration in minutes.                                                    |
|                                                                                                        |
|    - sort : Puts the current entries captured in the buffer.                                           |
|                                                                                                        |
|        Arguments:                                                                                      |
|                                                                                                        |
|        - id         : Sort by video ID.                                                                |
|        - name         : Sort by video name.                                                            |
|        - rating     : Sort by video rating.                                                            |
|        - duration   : Sort by video duration.                                                          |
|        - +             : Goes after the arguments above to specify ascending order.                    |
|        - -             : Goes after the arguments above to specify descending order.                   |
|                                                                                                        |
|    - rate : Lets the user enter a new rating for the video selected.                                   |
|                                                                                                        |
|        Arguments:                                                                                      |
|                                                                                                        |
|        - id        : Specifiy the ID of the video to rate.                                             |
|        - rating    : Quantitative rating value.                                                        |
|                                                                                                        |
|   - clear : Clears the screen and the entry buffer, clering the search and sorting.                    |
|                                                                                                        |
|    - help    : Displays this message.                                                                  |
|                                                                                                        |
|    - quit  : Exits MovieIO.                                                                            |
|                                                                                                        |
+--------------------------------------------------------------------------------------------------------+")"
};
const std::string VideoDataHolder::s_msg_unknownCmdErr{ "Please enter a valid command. Use \"help\"." };
const std::string VideoDataHolder::s_msg_startScreen{ 
R"(
+--------------------------------------------------------------------------------------------------------+
|                                                                                                        |
|                                                                                                        |
|                                                                                                        |
|                                                                                                        |
|                                                                                                        |
|        @@@@@@@    @@@@@@@                                                           ###                |
|         @@@@@@@  @@@@@@@       %@@.                      @@@@@@@@@@@@@@@       @@@@@@@@@@@@@           |
|         @@@@@@@/%@@@@@@@    @@@@@@@@@@   (@@@@@@  @@@/        &@@@@          @@@@@*     @@@@@@         |
|         @@@@@@@@@@@@@@@@   @@@@@ #@@@@@   @@@@@@  @@&         &@@@@         @@@@@        .@@@@,        |
|         @@@@@@@@@@@@@@@@  @@@@@@ #@@@@@@   @@@@@,@@@          &@@@@         @@@@#         @@@@@        |
|         @@@@@@@@@&@@@@@@  @@@@@@ #@@@@@@   @@@@@@@@@          &@@@@         @@@@.         @@@@@        |
|         @@@ @@@@,&@@@@@@  @@@@@@ #@@@@@@    @@@@@@@           &@@@@         @@@@%         @@@@@        |
|         @@@  @@@ &@@@@@@  @@@@@@ #@@@@@%    @@@@@@@           &@@@@         @@@@@        (@@@@         |
|         @@@      &@@@@@@   @@@@@ #@@@@@      @@@@@            &@@@@          @@@@@@     @@@@@          |
|        @@@@@,   @@@@@@@@@   @@@@@@@@@.       @@@@@       @@@@@@@@@@@@@@@       @@@@@@@@@@@@            |
|              XXXXXXXXXXXXXXX                                               XXXXXXXXXXX XXX             |
|          XXXXXX        XXXXXXXXXXXXXXX                            XXXXXXXXXXXX                         |
|        XXXXX                    XXXXXXXXXXXXXXXXXXXXXXXXXXXX XXXXXXXX                                  |
|       XXXX                               XXXXXXXXXXXXXXXXXXXXXXX                                       |
|                                                                                                        |
|                                                                                                        |
|           +-------------------------------------------------------------------------------+            |
|           |             Command line movie rating system by Pedro Escoboza                |            |
|           +-------------------------------------------------------------------------------+            |
|                                                                                                        |
|                                                                                                        |
+--------------------------------------------------------------------------------------------------------+)" };
const std::string VideoDataHolder::s_msg_enterCmd{"MovIO>"};


VideoDataHolder::VideoDataHolder(std::ostream& t_out, std::istream& t_in) :
	m_out{ t_out },
	m_in{t_in},
	m_videosById{},
	m_buffer{},
	m_movies{}, 
	m_series{}, 
	m_actions{
		{"search",	ActionBindings::SEARCH},
		{"s",		ActionBindings::SEARCH},
		{"rate",	ActionBindings::RATE},
		{"r",		ActionBindings::RATE},
		{"sort",	ActionBindings::SORT},
		{"s",		ActionBindings::SORT},
		{"clear",	ActionBindings::CLEAR},
		{"c",		ActionBindings::CLEAR},
		{"help",	ActionBindings::HELP},
		{"h",		ActionBindings::HELP},
		{"quit",	ActionBindings::QUIT},
		{"q",		ActionBindings::QUIT}},
	m_cmds{ [this]() {
		CmdsMap cmds;

		CmdParamsMap params;

		// Search
		params.emplace(s_params_search.m_name,			1U);
		params.emplace(s_params_search.m_id,			1U);
		params.emplace(s_params_search.m_genre,			1U);
		params.emplace(s_params_search.m_minrating,		1U);
		params.emplace(s_params_search.m_maxrating,		1U);
		params.emplace(s_params_search.m_minduration,	1U);
		params.emplace(s_params_search.m_maxduration,	1U);
		params.emplace(s_params_search.m_series,		1U);

		cmds.emplace(ActionBindings::SEARCH, std::move(params));
		params.clear();

		// Sort
		params.emplace(s_params_sort.m_name,		1U);
		params.emplace(s_params_sort.m_id,			1U);
		params.emplace(s_params_sort.m_rating,		1U);
		params.emplace(s_params_sort.m_duration,	1U);

		cmds.emplace(ActionBindings::SORT, std::move(params));
		params.clear();

		// Rate
		params.emplace(s_params_rate.m_id, 1U);
		params.emplace(s_params_rate.m_rating, 1U);
		
		cmds.emplace(ActionBindings::RATE, std::move(params));

		// Commands with no extra parameters
		cmds.emplace(ActionBindings::CLEAR, CmdParamsMap{});
		cmds.emplace(ActionBindings::HELP,  CmdParamsMap{});
		cmds.emplace(ActionBindings::QUIT,  CmdParamsMap{});

		return std::move(cmds);
	}() }
{}

void VideoDataHolder::parseInfoFromFile(const std::string& t_filename){
	m_out << s_msg_loading << std::endl;
	std::ifstream file;
	file.open(t_filename);
	if (!file.is_open()) {
		file.close();
		throw std::runtime_error{ { "Could not open file \"" + t_filename + "\".\n" } };
	}

	js::json jf{ js::json::parse(file) };
	file.close();

	for (auto& entry : jf) {

		const auto name{ entry["name"].get<std::string>() };
		const auto duration{ entry["duration"].get<unsigned>() };
		const auto id{ entry["id"].get<std::string>() };
		
		Ratings ratings;
		for (const auto& r : entry["ratings"][0]) {
			ratings.push_back(r.get<float>());
		}
		
		Genre genre;
		{
			std::string genreStr{ entry["genre"].get<std::string>() };
			if (!Video::getGenreFromStr(genreStr, genre)) {
					throw std::runtime_error{ {"Could not read video type \"" + genreStr + "\".\n"} };
			}
		}
		
		VideoType type;
		{
			std::string typeStr{ entry["type"].get<std::string>() };
			if (!Video::getVideoTypeFromStr(typeStr, type)) {
				throw std::runtime_error{ {"Could not read genre \""+ typeStr + "\".\n" } };
			}
		}

		switch (type){
		case VideoType::MOVIE:
			addMovie(name, id, duration, genre, ratings);
			break;
		case VideoType::SERIES_EPISODE: 
		{
			std::string series{ entry["series"].get<std::string>() };
			unsigned season{ entry["season_num"].get<unsigned>() };
			unsigned episodeNum{ entry["episode_num"].get<unsigned>() };
			addEpisode(name, id, duration, genre, series, season, episodeNum, ratings);
		}
			break;
		default:
			throw std::runtime_error{ "Unrecognized video type.\n" };
			break;
		}
	}
}

std::pair<ActionBindings, bool> VideoDataHolder::structureCommand(const StrVec& t_words, Parameters& t_outParams) {
	
	// Check that the words are not empty
	if (t_words.empty()) { return { ActionBindings{}, false }; }

	// The first word is the action selected
	auto cmdPair{VideoDataHolder::strToActionBinding(t_words[0])};
	
	// Validate the command selsection
	if (!cmdPair.second) { return { ActionBindings{}, false }; }

	const auto& action{ cmdPair.first }; // Action corresponding to first word
	const auto& paramsMap{ m_cmds.at(action) }; // The parameters map of said action

	// Iterate through the words (starting from the second one)
	for (auto wIt{++t_words.cbegin()}; wIt != t_words.cend (); wIt++) {
		// Look for any parameter keyword
		const auto& word{*wIt};
		auto pIt{paramsMap.find(word)};
		
		// Check it the word is a parameter keyword
		if (pIt != paramsMap.cend()) {
			
			// Add the parameter selected
			t_outParams.emplace_back(&*wIt, PtrToConstStrVec{} );
			auto &pArgsIt{t_outParams.back().second};

			// For each arg required, advance the word interator and remmeber the word
			for (unsigned i{ 0U }; i < pIt->second && wIt != t_words.cend(); i++) {
				wIt++;
				pArgsIt.push_back(&*wIt);
			}
		}
	}
	return {action, true};
}

void VideoDataHolder::executeAction(ActionBindings t_action, const Parameters& t_params){
	switch (t_action)
	{
	case ActionBindings::SEARCH:
		action_search(t_params);
		break;
	case ActionBindings::RATE:
		action_rate(t_params);
		break;
	case ActionBindings::SORT:
		action_sort(t_params);
		break;
	case ActionBindings::CLEAR:
		action_clear();
		break;
	case ActionBindings::HELP:
		action_help();
		break;
	case ActionBindings::QUIT:
		action_quit();
		break;
	default:
		break;
	}
}

void VideoDataHolder::start(){
	
	// Print out start screen
	m_out << s_msg_startScreen <<"\n\n"<<std::endl;

	// Main application loop
	while (true) {
		
		// Get the command that the user entered
		m_out << s_msg_enterCmd;
		auto words{ utl::getWords(input()) };	
		
		// Build the command from the words
		Parameters params;
		auto actionPair{ structureCommand(words, params) };
		
		// Validate if the command exists (first word)
		if (!actionPair.second) {
			
			// Print error messsage continue
			if (!words.empty()) {
				m_out << s_msg_unknownCmdErr << std::endl;
			}
			continue;
		}

		executeAction(actionPair.first, params);
	}

}

void VideoDataHolder::registerVideo(Video* t_video){
	auto it{ m_videosById.emplace(t_video->getId(), t_video)};
	if (!it.second) { throw std::runtime_error{ "Error registering video.\n" }; }
	m_videosVec.push_back(it.first->second);
}

VideoDataHolder& VideoDataHolder::addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, const std::string& t_series, unsigned t_seasonNum, unsigned t_episodeNum, const Ratings& t_ratings) {
	// Validate that video does not already exist
	auto idIt{m_videosById.find(t_id)};	
	if (idIt != m_videosById.cend()) { return *this; }
	
	auto seriesIt{ m_series.emplace(t_series, std::move(Series::newSeries(t_series))) };
	auto &series{ *seriesIt.first->second.get() };
	series.addEpisode(t_name, t_id, t_duration, t_genre, t_seasonNum, t_episodeNum );
	
	Video* video{ series.getEpisode(t_seasonNum, t_episodeNum) };
	if (video == nullptr) {	throw std::runtime_error("Could not retrieve inserted video.\n");}

	// Add ratings to video
	video->rate(t_ratings);

	registerVideo(video);
	return *this;
}

VideoDataHolder& VideoDataHolder::addMovie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, const Ratings& t_ratings){
	// Validate that video does not already exist
	auto idIt{ m_videosById.find(t_id) };
	if (idIt != m_videosById.cend()) { return *this; }
	
	m_movies.emplace_back(Movie::newMovie(t_name, t_id, t_duration, t_genre));
	Video* movie{ m_movies.back().get() };

	// Add ratings to video
	movie->rate(t_ratings);
	
	registerVideo(movie);
	return *this;
}

std::string VideoDataHolder::input(){
	std::string str{""};
	std::getline(m_in, str);
	return str;
}

std::pair<ActionBindings, bool> VideoDataHolder::strToActionBinding(const std::string& t_cmdName) const{
	auto it(m_actions.find(t_cmdName));
	if (it == m_actions.end()) {
		return { ActionBindings{}, false };
	}
	return { it->second, true };
}

bool VideoDataHolder::isLesser(SortCriteria t_criterion, const Video& t_a, const Video& t_b){
	switch (t_criterion)
	{
	case SortCriteria::NAME:
		return  utl::lexCompStrs(t_b.getName(), t_a.getName());
	case SortCriteria::ID:
		return  utl::lexCompStrs(t_b.getId(), t_a.getId());
	case SortCriteria::RATING:
		return t_a.getRating() < t_b.getRating();
	case SortCriteria::DURATION:
		return t_a.getDuration() < t_b.getDuration();
	default:
		return false;
	}
}

bool VideoDataHolder::isEqual(SortCriteria t_criterion, const Video& t_a, const Video& t_b){
	switch (t_criterion)
	{
	case SortCriteria::NAME:
		return t_a.getName() == t_b.getName();
	case SortCriteria::ID:
		return t_a.getId() == t_b.getId();
	case SortCriteria::RATING:
		return t_a.getRating() == t_b.getRating();
	case SortCriteria::DURATION:
		return t_a.getDuration() == t_b.getDuration();
	default:
		return false;
	}
}

void VideoDataHolder::action_search(const Parameters& t_params) {
	const std::string* name{ &utl::emptyStr };
	const std::string* id{ &utl::emptyStr };
	const std::string* genre{ &utl::emptyStr };
	const std::string* series{ &utl::emptyStr };
	float minr{ Video::s_minRating };
	float maxr{ Video::s_maxRating };
	int mind{ Video::s_minDuration };
	int maxd{ Video::s_maxDuration };

	std::vector<bool> alreadyRead(8U, false);

	for (const auto& kwP : t_params) {
		const auto& param{ *kwP.first};
		const auto& args{ kwP.second };

		if (!alreadyRead[0] && param == s_params_search.m_name) {
			name = args[0]; 
			alreadyRead[0] = true;
		}
		else if (!alreadyRead[1] && param == s_params_search.m_id) {
			id = args[0]; 
			alreadyRead[1] = true;
		}
		else if (!alreadyRead[2] && param == s_params_search.m_genre) {
			genre = args[0];
			alreadyRead[2] = true;
		}
		else if (!alreadyRead[3] && param == s_params_search.m_series) {
			series = args[0]; 
			alreadyRead[3] = true;
		}
		else if (!alreadyRead[4] && param == s_params_search.m_minrating) {
			try {
				minr = std::stof(*args[0]);
			}
			catch (std::invalid_argument& e) {
				e;
				minr = Video::s_minRating;
			}
			alreadyRead[4] = true;
		}
		else if (!alreadyRead[5] && param == s_params_search.m_maxrating) {
			try {
				maxr = std::stof(*args[0]);
			}
			catch (std::invalid_argument& e) {
				e;
				maxr = Video::s_maxRating;
			}
			alreadyRead[5] = true;
		}
		else if (!alreadyRead[6] && param == s_params_search.m_minduration) {
			try {
				mind = std::stoi(*args[0]) * 60; // minutes -> seconds
			}
			catch (std::invalid_argument& e) {
				e;
				mind = Video::s_minDuration;
			}
			alreadyRead[6] = true;
		}
		else if (!alreadyRead[7] && param == s_params_search.m_maxduration) {
			try {
				maxd = std::stoi(*args[0]) * 60; // minutes -> seconds
			}
			catch (std::invalid_argument& e) {
				e;
				maxd = Video::s_maxDuration;
			}
			alreadyRead[7] = true;
		}

	}

	if (minr < Video::s_minRating) { minr = Video::s_minRating; }
	if (maxr > Video::s_maxRating) { maxr = Video::s_maxRating; }
	if (mind < Video::s_minRating) { mind = Video::s_minDuration; }
	if (maxd > Video::s_maxDuration) { maxd = Video::s_maxDuration; }

	VideoDataHolder::filterVideos((m_buffer.empty() ? m_videosVec : m_buffer), m_buffer,*name, *id, *genre, *series, { minr, maxr }, {mind, maxd});
	printVideos(m_buffer, 0U, true, m_out);
}

void VideoDataHolder::action_rate(const Parameters& t_params) {
	const std::string* id{ &utl::emptyStr };
	float rating{ Video::s_minRating };
	bool isRatingValid{ false };
	
	bool alreadyRead[2] = { false, false };
	
	for (const auto& kwP : t_params) {
		const auto& param{ *kwP.first};
		const auto& args{ kwP.second };	

		if (!alreadyRead[0] && param == s_params_rate.m_id) {
			id = args[0];
			alreadyRead[0] = true;
		}
		else if (!alreadyRead[1] && param == s_params_rate.m_rating) {
			try {
				isRatingValid = true;
				rating = std::stof(*args[0]);
				alreadyRead[1] = true;
			}
			catch (std::invalid_argument& e) {
				e;
				rating = Video::s_minRating;
				isRatingValid = false;

			}
		}
	}
	
	auto videoIt{ m_videosById.find(*id) };
	if (videoIt == m_videosById.end()) {
		m_out << s_msg_notFoundErr << std::endl;
	}
	else if (!isRatingValid) {
		m_out << s_params_rate.m_err_rating << std::endl;
	}
	else {
		videoIt->second->rate(rating);
	}	
}
void VideoDataHolder::action_sort(const Parameters& t_params){
	
	SortMemo sortCriteria;
	std::vector<bool> alreadyRead(8U, false);

	for (const auto& kwP : t_params) {
		const auto& param{ *kwP.first };
		const auto& args{ kwP.second };
		
		bool isDescending{ s_params_sort.m_descending.find(*args[0]) != s_params_sort.m_descending.cend()};


		if (!alreadyRead[0] && param == s_params_sort.m_name) {
			sortCriteria.emplace_back(SortCriteria::NAME, isDescending);
			alreadyRead[0] = true;
		}
		else if (!alreadyRead[1] && param == s_params_sort.m_id) {
			sortCriteria.emplace_back(SortCriteria::ID, isDescending);
			alreadyRead[1] = true;
		}
		else if (!alreadyRead[2] && param == s_params_sort.m_rating) {
			sortCriteria.emplace_back(SortCriteria::RATING, isDescending);
			alreadyRead[2] = true;
		}
		else if (!alreadyRead[3] && param == s_params_sort.m_duration) {
			sortCriteria.emplace_back(SortCriteria::DURATION, isDescending);
			alreadyRead[3] = true;
		}
	}

	sortVideosBy((m_buffer.empty() ? m_videosVec : m_buffer), m_buffer, sortCriteria);
	printVideos(m_buffer, 0U, true, m_out);
}

void VideoDataHolder::action_clear(const Parameters& t_params){
	m_buffer.clear();
#if defined( __WIN32__) || defined(_WIN32) || defined(__CYGWIN32__)
	std::system("cls");
#else
	std::system("clear");
#endif // WIN32
}

void VideoDataHolder::action_help(const Parameters& t_params){
	m_out << s_msg_help << std::endl;
}

void VideoDataHolder::action_quit(const Parameters& t_params){
	std::exit(0);
}

const Video* VideoDataHolder::getVideoById(const std::string& t_videoId) const{
	auto it{ m_videosById.find(t_videoId) };
	return it != m_videosById.cend() ? it->second : nullptr;
}

Video* VideoDataHolder::getVideoById(const std::string& t_videoId){
	auto it{m_videosById.find(t_videoId)};
	return it != m_videosById.end() ? it->second : nullptr;
}

const Series* VideoDataHolder::getSeriesByName(const std::string& t_name) const{
	auto it{m_series.find(t_name)};
	return it != m_series.cend() ? it->second.get() : nullptr;
}

Series* VideoDataHolder::getSeriesByName(const std::string& t_name){
	auto it{ m_series.find(t_name) };
	return it != m_series.end() ? it->second.get() : nullptr;
}

std::vector<Movie*>& VideoDataHolder::getMovies(std::vector<Movie*>& t_outMovies){
	t_outMovies.reserve(t_outMovies.size() + m_movies.size());
	for (const auto& movPtr : m_movies) {
		t_outMovies.push_back(movPtr.get());
	}
	return t_outMovies;
}


void VideoDataHolder::printVideos(const VideosVec& t_videos, unsigned t_numEntries, bool t_printHeader, std::ostream& t_out){
	if (t_printHeader == true) {
		Video::printTableHeader(t_out);
		t_out << '\n';
	}

	unsigned counter{ 1U };
	for (const auto video : t_videos) {
		const auto& ref{ *video };
		t_out << ref << '\n';
		if (counter == t_numEntries) { break; }
		counter++;
	}
}

VideosVec& VideoDataHolder::filterVideos(const VideosVec& t_inVideos, VideosVec& t_outVideos, const std::string& t_name, const std::string& t_id, const std::string& t_genre, const std::string& t_series, const std::pair<float, float>& t_rating, const std::pair<int ,int>& t_duration) const{
	
	auto filterFunction{ [&](const Video& t_video) {
		
		// Check name matches
		if (!t_name.empty()) {
			if (t_name != t_video.getName()) { return false; }
		}
		
		// Check id matches
		if (!t_id.empty()) {
			if (t_id != t_video.getId()) { return false; }
		} 

		// Check genre matches
		{
			Genre genre;
			if (!t_genre.empty()) {
				if (Video::getGenreFromStr(t_genre, genre) && genre != t_video.getGenre()) { return false; }
			} 
		}

		// Check rating matches
		{
			auto min{t_rating.first};
			auto max{t_rating.second};
			if (min > max) { std::swap(min, max); }
			auto r{t_video.getRating()};
			if (r < min || r > max) {
				return false;
			}
		}

		// Check duration matches
		{
			auto min{static_cast<unsigned>(t_duration.first)};
			auto max{ static_cast<unsigned>(t_duration.second)};
			if (min > max) { std::swap(min, max); }
			auto d{t_video.getDuration()};
			if (d < min || d > max) {
				return false;
			}
		}

		// Check series matches
		if (!t_series.empty()) {

			if (t_video.getType() == VideoType::SERIES_EPISODE) {
				
				if (t_series != dynamic_cast<const Episode&>(t_video).getSeriesName()) { 
					return false;	
				}

			}
			else { 
				return false; 
			}
		}	
		
		return true;
	}};


	if (&t_inVideos == &t_outVideos) {
		auto inCopy{t_inVideos};
		t_outVideos.clear();
		filter(filterFunction, inCopy, t_outVideos);
	}
	else {
		filter(filterFunction, t_inVideos, t_outVideos);
	}
	
	return t_outVideos;
}

VideosVec& VideoDataHolder::sortVideosBy(const VideosVec& t_inVideos, VideosVec& t_outVideos, const SortMemo& t_criteria){

	// Remove repeated entries, since for the order of criteria to be kept, a vector was to be used
	std::unordered_set<SortCriteria> validationSet;
	std::vector<const std::pair<SortCriteria, bool>*> validatedCriteria;
	for (const auto& p : t_criteria) {
		auto it{validationSet.emplace(p.first)};
		if (it.second) { validatedCriteria.push_back(&p); }
	}

	auto compare{
		[&validatedCriteria](const Video* t_a, const Video* t_b) {
			for (const auto& p : validatedCriteria) {
				const auto& criterion{p->first};
				bool isDescending{p->second};

				if (!VideoDataHolder::isEqual(criterion, *t_a, *t_b)) {
					bool c{ VideoDataHolder::isLesser(criterion, *t_a, *t_b) };
					return isDescending ? c : !c; 
				}
			}
			return false;
		}
	};

	t_outVideos = t_inVideos;
	std::sort(t_outVideos.begin(), t_outVideos.end(), compare);
	return t_outVideos;
}