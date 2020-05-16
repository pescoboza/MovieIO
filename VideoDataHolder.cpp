#include "VideoDataHolder.hpp"
#include <fstream>
#include <stdexcept>
#include "json.hpp" // Credits to https://github.com/nlohmann/json

namespace js = nlohmann;

const std::string VideoDataHolder::s_initMsg{
R"(Usage)"
};


VideoDataHolder::VideoDataHolder() : m_videosById{}, m_movies{}, m_series{}{}

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

void VideoDataHolder::start(){
	
	std::cout << "";
	input();

}

VideoDataHolder& VideoDataHolder::addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, const std::string& t_series, unsigned t_season, unsigned t_episodeNum, const Ratings& t_ratings) {
	auto it{m_series.find(t_series)};
	Episode* episode{ nullptr };
	if (it != m_series.end()) {
		it->second->addEpisode(t_name, t_id, t_duration, t_genre, t_season, t_episodeNum);
		episode = it->second->getEpisode(t_season, t_episodeNum);
	}
	else {
		auto & series{*m_series.emplace(t_series, Series::newSeries(t_series)).first->second.get()};
		series.addEpisode(t_name,t_id, t_duration, t_genre, t_season, t_episodeNum);
		episode = series.getEpisode(t_season, t_episodeNum);
	}

	if (!episode) {
		throw std::runtime_error{"Could not get address of video.\n"};
	}

	for (const auto r : t_ratings) {
		episode->rate(r);
	}

	registerVideo(episode);
	return *this;
}

VideoDataHolder& VideoDataHolder::addMovie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, const Ratings& t_ratings){
	m_movies.emplace_back(Movie::newMovie(t_name, t_id, t_duration, t_genre));
	Video* movie{ m_movies.back().get() };
	for (const auto& r : t_ratings) {movie->rate(r);}
	registerVideo(movie);
	return *this;
}

std::string VideoDataHolder::input(std::istream& t_in){
	std::string str{""};
	std::getline(t_in, str);
	return str;
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

VideosVec& VideoDataHolder::getVideos(VideosVec& t_outVideos, const std::string& t_name, const std::string& t_genre, const std::string& t_series, const std::pair<float, float>& t_rating) const{

	auto filterFunction{ [&](const Video& t_video) {
		
		// Check name matches
		if (!t_name.empty() && t_name != t_video.getName()) {return false;}
		
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
#ifdef _DEBUG
	#include <unordered_set>
#endif // _DEBUG
VideoDataHolder& VideoDataHolder::registerVideo(Video* t_video){


#ifdef _DEBUG
	static std::unordered_set<Video*> vids;
	if (!vids.emplace(t_video).second)	{
		std::cerr << "POINTER REPEATED!!!\n";
	}
	static int count{0};
	std::cerr << "registerVideo(): " << ++count << '\n';
#endif // _DEBUG
	
	auto it{ m_videosById.emplace(t_video->getId(), t_video) };
	
	if (it.second) { m_videosVec.push_back(t_video); }
#ifdef _DEBUG
	else {
		bool foundInVidVec{false};
		for (const auto& ptr : m_videosVec) {
			if (t_video == ptr) { 
				foundInVidVec = true; 
				break; 
			}
		}
		std::cerr << std::boolalpha << "Repeated video: " << "foundInVidVec: "<< foundInVidVec << ' ' << it.first->second << " == " << t_video << ": " <<  (it.first->second == t_video) << '\n';
		std::cerr << *it.first->second << '\n';
		std::cerr << *t_video << '\n';
	}
#endif // _DEBUG
		
	return *this;
}

VideoDataHolder& VideoDataHolder::addVideos(const std::vector<Video*>& t_videos){
	for (auto ptr : t_videos) {	registerVideo(ptr);}
	return *this;
}
