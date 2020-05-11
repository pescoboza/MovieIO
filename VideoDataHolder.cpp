#include "VideoDataHolder.hpp"

VideoDataHolder& VideoDataHolder::addSeries(SeriesPtr t_series){
	m_series.emplace_back(std::move(t_series));

}

VideoDataHolder& VideoDataHolder::addMovie(MoviePtr t_movie){
	m_movies.emplace_back(std::move(t_movie));
}
