#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <string>
#include <vector>

using Ratings = std::vector<float>;

enum class Genre {
	DRAMA,
	ACTION,
	MYSTERY
};

enum class VideoType {
	MOVIE,
	SERIES_EPISODE
};

class Video {
public:	
	static const float s_minRating;
	static const float s_maxRating;

protected:

	std::string m_name;
	std::string m_id;
	unsigned m_duration;
	Genre m_genre;
	VideoType m_type;
	Ratings m_ratings;


	Video(const std::string& t_name,  const std::string& t_id, unsigned t_duration, Genre t_genre, VideoType t_type);

public:
	const std::string& getName() const;
	float getRating() const;
	const std::string& getId()const;
	VideoType getType() const;
	unsigned getDuration() const;
	Genre getGenre() const;
	void rate(float t_rating);
};

#endif // !VIDEO_HPP