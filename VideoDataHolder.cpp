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
