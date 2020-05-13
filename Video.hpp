#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>


enum class Genre;
enum class VideoType;

using Ratings = std::vector<float>;
using GenreMap = std::unordered_map<std::string, Genre>;
using VideoTypeMap = std::unordered_map<std::string, VideoType>;

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

private:
	static const GenreMap s_genresStrings;
	static const VideoTypeMap s_videoTypesStrings;

protected:
	std::string m_name;
	std::string m_id;
	unsigned m_duration;
	Genre m_genre;
	VideoType m_type;
	Ratings m_ratings;

	static const struct TableWidths{
		unsigned m_id, m_name, m_duration, m_genre, m_rating, m_type;
	}s_tblw;


	Video(const std::string& t_name,  const std::string& t_id, unsigned t_duration, Genre t_genre, VideoType t_type);

public:
	const std::string& getName() const;
	float getRating() const;
	const std::string& getId()const;
	Genre getGenre() const;
	VideoType getType() const;
	unsigned getDuration() const;
	void rate(float t_rating);
	std::string formattedDuration() const;

	virtual void print(std::ostream& t_out = std::cout) const;

	static const std::string& getStrFromGenre(Genre t_genre);
	static bool getGenreFromStr(const std::string& t_str, Genre& t_outGenre);
	static bool getVideoTypeFromStr(const std::string& t_str, VideoType& t_outType);
	static const std::string& getStrFromVideoType(VideoType t_type);
};

#endif // !VIDEO_HPP