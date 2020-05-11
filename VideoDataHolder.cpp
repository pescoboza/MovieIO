#include "VideoDataHolder.hpp"

VideoDataHolder& VideoDataHolder::addSeries(SeriesPtr t_series){
	m_series.emplace_back(std::move(t_series));
	std::vector<Episode*> episodes;
	m_series.back()->getAllEpisodes(episodes);
	for (auto ep : episodes) {
		m_videos.emplace(ep->getId(), ep);
	}
	return *this;
}

VideoDataHolder& VideoDataHolder::addMovie(MoviePtr t_movie){
	m_movies.emplace_back(std::move(t_movie));
	auto mov{ m_movies.back().get() };
	m_videos.emplace(mov->getId(), mov);
	return *this;
}
