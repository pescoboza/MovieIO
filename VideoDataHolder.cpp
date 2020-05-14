#include "VideoDataHolder.hpp"
#include <fstream>
#include <sstream>
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
		std::ostringstream errMsg;
		errMsg << "Could not open file \"" << t_filename << "\".\n";
		throw std::runtime_error{errMsg.str()};
	}

	js::json jf{ js::json::parse(file) };
	file.close();

	for (auto& entry : jf) {

		const auto name{ entry["name"].get<std::string>() };
		const auto duration{ entry["duration"].get<unsigned>() };
		const auto ratings{ entry["ratings"].get<std::vector<float>>() };
		const auto id{ entry["id"].get<std::string>() };
		Genre genre;
		{
			std::string genreStr{ entry["genre"].get<std::string>() };
			if (!Video::getGenreFromStr(genreStr, genre)) {
				std::ostringstream error{ "Could not read video type \"" };
				error << genreStr <<"\".\n";
				throw std::runtime_error{error.str()};
			}
		}
		
		VideoType type;
		{
			std::string typeStr{ entry["type"].get<std::string>() };
			if (!Video::getVideoTypeFromStr(typeStr, type)) {
				std::ostringstream error{"Could not read genre \""};
				error << typeStr << "\".\n";
				throw std::runtime_error{ error.str() };
			}
		}

		switch (type){
		case VideoType::MOVIE:
			addMovie(name, id, duration, genre);
			break;
		case VideoType::SERIES_EPISODE: 
		{
			std::string series{ entry["series"].get<std::string>() };
			unsigned season{ entry["season_num"].get<unsigned>() };
			unsigned episodeNum{ entry["episode_num"].get<unsigned>() };
			addEpisode(name, id, duration, genre, series, season, episodeNum);
		}
			break;
		default:
			break;
		}
	}
}

void VideoDataHolder::start(){
	
	std::cout << ""
	input();

}




VideoDataHolder& VideoDataHolder::addEpisode(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, const std::string& t_series, unsigned t_season, unsigned t_episodeNum){
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

	registerVideo(episode);
	return *this;
}

VideoDataHolder& VideoDataHolder::addMovie(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre){
	m_movies.emplace_back(Movie::newMovie(t_name, t_id, t_duration, t_genre));
	Video* movie{ m_movies.back().get() };
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

std::vector<Video*>& VideoDataHolder::getVideosOfGenre(Genre t_genre, std::vector<Video*>& t_outVideos) const{

}

std::vector<Video*>& VideoDataHolder::getVideosOfGenre(Genre t_genre, std::vector<Video*>& t_inVideos,std::vector<Video*>& t_outVideos){
	return filter([&t_genre](const Video& t_video) {
		return t_genre == t_video.getGenre();
	}, t_inVideos,t_outVideos);
}

std::vector<Video*>& VideoDataHolder::getVideosOfRating(float t_min, float t_max, std::vector<Video*>& t_inVideos, std::vector<Video*>& t_outVideos){
	if (t_min > t_max) { std::swap(t_min, t_max); }
	if (t_min < Video::s_minRating) { t_min = Video::s_minRating; }
	if (t_max > Video::s_maxRating) { t_max = Video::s_maxRating; }
	return filter([&t_min, &t_max](const Video& t_video) {
		float rating{t_video.getRating()};
		return t_min <= rating && t_max >= rating;
		}, t_inVideos, t_outVideos);
}

void VideoDataHolder::printVideos(const std::vector<Video*>& t_videos){
	for (const auto vid : t_videos) {
		if (!vid) { continue; }
		
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

VideoDataHolder& VideoDataHolder::registerVideo(Video* t_video){
	m_videosById.emplace(t_video->getId(), t_video);
	m_videosByName.emplace(t_video->getName(), t_video);
	m_videosVec.push_back(t_video);
	return *this;
}

VideoDataHolder& VideoDataHolder::addVideos(const std::vector<Video*>& t_videos){
	for (auto ptr : t_videos) {	registerVideo(ptr);}
	return *this;
}
