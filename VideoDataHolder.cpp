#include "VideoDataHolder.hpp"
#include <cstdlib>
#include <stdexcept>
#include <fstream>
#include <functional>
#include <sstream>
#include "json.hpp" // Credits to https://github.com/nlohmann/json
#include "Utilities.hpp"

namespace js = nlohmann;


#ifdef _DEBUG
#include <unordered_set>
#include <unordered_map>
#include <string>
using EpNumSet = std::unordered_set<unsigned>;
using SsnNumMap = std::unordered_map<unsigned, EpNumSet>;
using  SrsMap = std::unordered_map<std::string, SsnNumMap>;


namespace debug {

	SrsMap series_seasons_eps;

	bool is_something_repeated(SrsMap& t_map, const std::string& t_seriesName, unsigned t_ssnNum, unsigned t_epNum) {
		auto series_it{ t_map.find(t_seriesName) };
		if (series_it == t_map.end()) {
			t_map.emplace(
				[&t_seriesName, &t_ssnNum, &t_epNum]() {
					EpNumSet ep{ t_epNum };
					SsnNumMap ssn{ {t_ssnNum, std::move(ep)} };
					return std::make_pair(t_seriesName, ssn);
				}()
					);
			return false;
		}

		auto& ssnMap{ series_it->second };
		auto ssn_it{ ssnMap.find(t_ssnNum) };
		if (ssn_it == ssnMap.end()) {
			ssnMap.emplace(t_ssnNum, EpNumSet{ t_epNum });
			return false;
		}

		auto& epSet{ ssn_it->second };
		auto ep_it{ epSet.find(t_epNum) };
		if (ep_it == epSet.end()) {
			epSet.emplace(t_epNum);
			return false;
		}
		return true;
	}
}
#endif // _DEBUG

const std::string VideoDataHolder::s_initMsg{
R"(Usage)"
};

const std::string VideoDataHolder::s_unkownCmdErrMsg{ "Err" };
const std::string VideoDataHolder::s_startScreen{ "SC" };

VideoDataHolder::VideoDataHolder(std::ostream& t_out) :
	m_out{ t_out },
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
		{"-h",		ActionBindings::HELP},
		{"quit",	ActionBindings::QUIT},
		{"q",		ActionBindings::QUIT}},
	m_actionBindings{[]() {
		ActionMap bindings;
		bindings.reserve(static_cast<unsigned>(ActionBindings::QUIT) + 1U);
		bindings.emplace(ActionBindings::SEARCH,	std::make_unique<Action>(ActionBindings::SEARCH,"descritpion", "search <id> <name> <duration> <>"));
		bindings.emplace(ActionBindings::RATE,		std::make_unique<Action>(ActionBindings::RATE,	"Add a rating to a video.", "rate <id> <rating>"));
		bindings.emplace(ActionBindings::SORT,		std::make_unique<Action>(ActionBindings::SORT,	"descritpion", "sort <category>"));
		bindings.emplace(ActionBindings::CLEAR,		std::make_unique<Action>(ActionBindings::CLEAR,	"descritpion", "usage"));
		bindings.emplace(ActionBindings::HELP,		std::make_unique<Action>(ActionBindings::HELP,	"description", "usage"));
		bindings.emplace(ActionBindings::QUIT,		std::make_unique<Action>(ActionBindings::QUIT,	"descritpion", "usage"));
		return std::move(bindings);
	}()},
	m_cmds{ [this]() {
		CmdsMap cmds;

		CmdParamsMap params;

		// Search
		params.emplace(m_params_search.m_name,			1U);
		params.emplace(m_params_search.m_id,			1U);
		params.emplace(m_params_search.m_genre,			1U);
		params.emplace(m_params_search.m_minrating,		1U);
		params.emplace(m_params_search.m_maxrating,		1U);
		params.emplace(m_params_search.m_minduration,	1U);
		params.emplace(m_params_search.m_minduration,	1U);
		params.emplace(m_params_search.m_series,		1U);

		cmds.emplace(ActionBindings::SEARCH, std::move(params));
		params.clear();

		// Sort
		params.emplace(m_params_sort.m_nameAsc, 0U);
		params.emplace(m_params_sort.m_nameDes, 0U);
		params.emplace(m_params_sort.m_idAsc,	0U);
		params.emplace(m_params_sort.m_idDes,	0U);
		params.emplace(m_params_sort.m_ratingAsc, 0U);
		params.emplace(m_params_sort.m_ratingDes, 0U);
		params.emplace(m_params_sort.m_durationAsc, 0U);
		params.emplace(m_params_sort.m_durationDes, 0U);

		cmds.emplace(ActionBindings::SORT, std::move(params));
		params.clear();

		// Rate
		params.emplace(m_params_rate.m_id, 1U);
		params.emplace(m_params_rate.m_rating, 1U);
		
		cmds.emplace(ActionBindings::RATE, std::move(params));

		return std::move(cmds);
	}() }
{}

void VideoDataHolder::parseInfoFromFile(const std::string& t_filename){
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

void VideoDataHolder::start(std::ostream& t_out, std::istream& t_in){
	bool isQuit{ false };
	while (!isQuit) {
		t_out << s_startScreen;
		
		// Get the command that the user entered
		auto words{ utl::getWords(input()) };	
		auto actionPair{ strToActionBinding(words[0]) };

		// Splash an error screen if no command is recognized or if no parameters are given
		if (!actionPair.second || words.size() == 1U) {
			t_out << s_unkownCmdErrMsg << std::endl;
			continue;
		}

		// Look for keywords of parameters of the entered command
		auto action = actionPair.first;
		auto cmdIt{ m_cmds.find(action) };

		// Store the parameter and its arguments
		CmdParamsMemo paramsMemo;

		// If the command exists, but its not in the map, it means it does not need any additional parameters
		if (cmdIt != m_cmds.cend()) {
			auto& paramsMap{cmdIt->second};

			bool isUnknownKeyword{ false };

			// Iterate through the words
			for (auto it{ ++words.cbegin() }; it != words.cend();) {
				const auto& word{ *it };

				auto keywordIt{ paramsMap.find(word) };

				// Check if the word is a keyword, else break and remember to splash error
				if (keywordIt == paramsMap.cend()) {
					isUnknownKeyword = true;
					break;
				}

				// Remember the new emplaced parameter
				auto& paramArgs{ paramsMemo.emplace(*it, PtrToConstStrVec{}).first->second};
				paramArgs.reserve(keywordIt->second);
				
				// Capture the number of args needed for the keyword
				for (unsigned i{ 0 }; i < keywordIt->second; i++) {
					it++;
					paramArgs.push_back(&*it);
				}

				// Go to the next keyword
				it++;
			}
		}


		// We have collected the keywords and parameters for the command
		// We now pass them to the callback assigned to the command function

		switch (action)
		{
		case ActionBindings::SEARCH:
			break;
		case ActionBindings::RATE:
			break;
		case ActionBindings::SORT:
			break;
		case ActionBindings::CLEAR:
			break;
		case ActionBindings::HELP:
			break;
		case ActionBindings::QUIT:
			break;
		default:
			break;
		}

		m_actionBindings
		paramsMemo

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
	for (const auto& r : t_ratings) { video->rate(r) ; }

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
	for (const auto& r : t_ratings) {movie->rate(r);}
	
	registerVideo(movie);
	return *this;
}

std::string VideoDataHolder::input(std::istream& t_in){
	std::string str{""};
	std::getline(t_in, str);
	return str;
}

std::pair<ActionBindings, bool> VideoDataHolder::strToActionBinding(const std::string& t_cmdName) const{
	auto it(m_actions.find(t_cmdName));
	if (it != m_actions.end()) {
		return { ActionBindings{}, false };
	}
	return { it->second, true };
}

void VideoDataHolder::action_search(const CmdParamsMemo& t_memo) {
	const std::string* name{ &utl::emptyStr };
	const std::string* id{ &utl::emptyStr };
	const std::string* genre{ &utl::emptyStr };
	const std::string* series{ &utl::emptyStr };
	float minr{ Video::s_minRating };
	float maxr{ Video::s_maxRating };
	float mind{ Video::s_minDuration };
	float maxd{ Video::s_maxDuration };

	std::vector<bool> alreadyRead{8, false};

	for (const auto& kwP : t_memo) {
		const auto& param{ kwP.first.get() };
		const auto& args{ kwP.second };

		if (!alreadyRead[0] && param == m_params_search.m_name) {
			name = args[0]; 
			alreadyRead[0] = true;
		}
		else if (!alreadyRead[1] && param == m_params_search.m_id) {
			id = args[0]; 
			alreadyRead[1] = true;
		}
		else if (!alreadyRead[2] && param == m_params_search.m_genre) {
			genre = args[0]; 
			alreadyRead[2] = true;
		}
		else if (!alreadyRead[3] && param == m_params_search.m_series) {
			series = args[0]; 
			alreadyRead[3] = true;
		}
		else if (!alreadyRead[4] && param == m_params_search.m_minrating) {
			try {
				minr = std::stof(*args[0]);
			}
			catch (std::invalid_argument& e) {
				minr = Video::s_minRating;
			}
			alreadyRead[4] = true;
		}
		else if (!alreadyRead[5] && param == m_params_search.m_maxrating) {
			try {
				maxr = std::stof(*args[0]);
			}
			catch (std::invalid_argument& e) {
				maxr = Video::s_maxRating;
			}
			alreadyRead[5] = true;
		}
		else if (!alreadyRead[6] && param == m_params_search.m_minduration) {
			try {
				mind = std::stof(*args[0]);
			}
			catch (std::invalid_argument& e) {
				mind = Video::s_minDuration;
			}
			alreadyRead[6] = true;
		}
		else if (!alreadyRead[7] && param == m_params_search.m_maxduration) {
			try {
				maxd = std::stof(*args[0]);
			}
			catch (std::invalid_argument& e) {
				maxd = Video::s_maxDuration;
			}
			alreadyRead[7] = true;
		}

	}

	if (minr < Video::s_minRating) { minr = Video::s_minRating; }
	if (maxr > Video::s_maxRating) { maxr = Video::s_maxRating; }
	if (mind < Video::s_minRating) { minr = Video::s_minDuration; }
	if (maxd > Video::s_maxDuration) { maxd = Video::s_maxDuration; }

	VideoDataHolder::filterVideos(m_buffer, *name, *id, *genre, *series, { minr, maxr }, {mind, maxd});
}

void VideoDataHolder::actoin_rate(const CmdParamsMemo& t_memo) {
	const std::string* id{ &utl::emptyStr };
	float rating{ Video::s_minRating };
	bool isRatingValid{ false };

	for (const auto& kwP : t_memo) {
		const auto& param{ kwP.first.get() };
		const auto& args{ kwP.second };

		bool alreadyRead[2] = { false, false };

		if (!alreadyRead[0] && param == m_params_rate.m_id) {
			id = args[0];
			alreadyRead[0] = true;
		}
		else if (!alreadyRead[1] && param == m_params_rate.m_rating) {
			try {
				isRatingValid = true;
				rating = std::stof(*args[0]);
				alreadyRead[1] = true;
			}
			catch (std::invalid_argument& e) {
				rating = Video::s_minRating;
				isRatingValid = false;

			}
		}
	}
	
	auto videoIt{ m_videosById.find(*id) };
	if (videoIt == m_videosById.end()) {
		m_out << s_notFoundErr << std::endl;
	}
	else if (!isRatingValid) {
		m_out << m_params_rate.m_err_rating << std::endl;
	}
	else {
		videoIt->second->rate(rating);
	}	
}
void VideoDataHolder::action_sort(const CmdParamsMemo& t_memo)
{
}

void VideoDataHolder::action_clear(const CmdParamsMemo& t_memo){
#if defined( __WIN32__) || defined(_WIN32) || defined(__CYGWIN32__)
	std::system("cls");
#else
	std::system("clear");
#endif // WIN32
}

void VideoDataHolder::action_help(const CmdParamsMemo& t_memo){
	m_out << s_helpMsg << std::endl;
}

void VideoDataHolder::action_quit(const CmdParamsMemo& t_memo){
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

VideosVec& VideoDataHolder::filterVideos(VideosVec& t_outVideos, const std::string& t_name, const std::string& t_id, const std::string& t_genre, const std::string& t_series, const std::pair<float, float>& t_rating, const std::pair<int ,int>& t_duration) const{

	auto filterFunction{ [&](const Video& t_video) {
		
		// Check name matches
		if (!t_name.empty() && t_name != t_video.getName()) {return false;}
		
		// Check id matches
		if (!t_id.empty() && t_id != t_video.getId()) { return false; }

		// Check genre matches
		{
			Genre genre;
			if (!t_genre.empty() && Video::getGenreFromStr(t_genre, genre) && genre != t_video.getGenre()) { return false; }
		}

		// Check rating matches
		{
			const auto& min{t_rating.first};
			const auto& max{t_rating.second};
			auto r{t_video.getRating()};
			if (r < min || r > max) {
				return false;
			}
		}

		// Check series matches
		if (!t_series.empty() && t_video.getType() == VideoType::SERIES_EPISODE && 
			t_series != dynamic_cast<const Episode*>(&t_video)->getSeason().getSeries().getName()) {
			return false;
		}

		return true;
	}};

	filter(filterFunction, m_videosVec, t_outVideos);
	return t_outVideos;
}

VideosVec& VideoDataHolder::sortVideosBy(const VideosVec& t_inVideos, VideosVec& t_outVideos, SortVideosBy t_criteria, bool t_ascending){
	VideosVec temp;
	temp.reserve(t_inVideos.size());

	switch (t_criteria)	{
	case SortVideosBy::NAME: 
	{
		std::multimap<std::string, Video*> videos;
		for (const auto& v : t_inVideos) { videos.emplace(v->getName(), v); }
		for (const auto& p : videos) { temp.push_back(p.second); }
	}
		break;
	case SortVideosBy::ID:
	{
		std::multimap<std::string, Video*> videos;
		for (const auto& v : t_inVideos) { videos.emplace(v->getId(), v); }
		for (const auto& p : videos) { temp.push_back(p.second); }
	}
		break;
	case SortVideosBy::RATING:
	{
		std::multimap<float, Video*> videos;
		for (const auto& v : t_inVideos) { videos.emplace(v->getRating(), v); }
		for (const auto& p : videos) { temp.push_back(p.second); }
	}
		break;
	case SortVideosBy::DURATION:
	{
		std::multimap<int, Video*> videos;
		for (const auto& v : t_inVideos) { videos.emplace(v->getDuration(), v); }
		for (const auto& p : videos) { temp.push_back(p.second); }
	}
		break;
	default:
		break;
	}

	if (t_ascending) { std::reverse(temp.begin(), temp.end()); }

	t_outVideos = std::move(temp);
	return t_outVideos;
}



Action::Action(ActionBindings t_boundAction, const std::string& t_desc, const std::string& t_usage) : 
	m_boundAction{ t_boundAction }, m_desc{ t_desc }, m_usage{ t_usage }{}


const std::string& Action::getDesc() const { return m_desc; }

const std::string& Action::getUsage() const { return m_usage; }
