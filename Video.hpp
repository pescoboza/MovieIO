#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <string>
#include <unordered_map>
#include <vector>

enum class VideoType;

using Ratings = std::vector<float>;
using VideoTypesMap = std::unordered_map<std::string, VideoType>;

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
	static const VideoTypesMap s_videoTypesStrings;

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
	
	static bool getVideoTypeFromStr(const std::string& t_str, VideoType& t_outType);
	static const std::string& getVideoStrFromType(VideoType t_type);
};

#endif // !VIDEO_HPP