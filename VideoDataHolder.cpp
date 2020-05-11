#include "VideoDataHolder.hpp"

VideoDataHolder::VideoDataHolder() : m_videos{}, m_movies{}, m_series{}{}

VideoDataHolder& VideoDataHolder::addSeries(SeriesPtr t_series){
	auto it{m_series.find(t_series->getName())};
	if (it != m_series.end()) {
		m_series.erase(it);
	}
	m_series.emplace(t_series->getName(), std::move(t_series));
	return *this;
}

VideoDataHolder& VideoDataHolder::addMovie(MoviePtr t_movie){
	m_movies.emplace_back(std::move(t_movie));
	auto mov{ m_movies.back().get() };
	m_videos.emplace(mov->getId(), mov);
	return *this;
}

const Video* VideoDataHolder::getVideoById(const std::string& t_videoId) const{
	auto it{ m_videos.find(t_videoId) };
	return it != m_videos.cend() ? it->second : nullptr;
}

Video* VideoDataHolder::getVideoById(const std::string& t_videoId){
	auto it{m_videos.find(t_videoId)};
	return it != m_videos.end() ? it->second : nullptr;
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
