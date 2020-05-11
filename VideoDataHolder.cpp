#include "VideoDataHolder.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

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

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	// TODO: Create file parser
}

VideoDataHolder& VideoDataHolder::addSeries(SeriesPtr t_series){
	{
		auto it{ m_series.find(t_series->getName()) };
		if (it != m_series.end()) {
			m_series.erase(it);
		}
	}
	auto empl_it{ m_series.emplace(t_series->getName(), std::move(t_series)) };
	std::vector<Video*> episodes;
	empl_it.first->second->getAllEpisodes(episodes);
	addVideos(episodes);
	return *this;
}

VideoDataHolder& VideoDataHolder::addMovie(MoviePtr t_movie){
	m_movies.emplace_back(std::move(t_movie));
	Video* movie{ m_movies.back().get() };
	addVideo(movie);
	return *this;
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

VideoDataHolder& VideoDataHolder::addVideo(Video* t_video){
	m_videosById.emplace(t_video->getId(), t_video);
	m_videosByName.emplace(t_video->getName(), t_video);
	return *this;
}

VideoDataHolder& VideoDataHolder::addVideos(const std::vector<Video*>& t_videos){
	for (auto ptr : t_videos) {	addVideo(ptr);}
	return *this;
}
