#include "Video.hpp"
#include <iomanip>
#include <sstream>

namespace n {
	const std::string emptyStr{ "" };

};

const Video::TableParams Video::s_tbl{
	16U, // id
	80U, // name 
	16U, // duration
	8U, // genre
	8U, // rating
	8U, // type
	32U, // series
	" | " ,// separator
	"ID", 
	"NAME", 
	"DURATION", 
	"GENRE", 
	"RATING", 
	"TYPE", 
	"SERIES", 
};

const float Video::s_minRating{ 0.f };
const float Video::s_maxRating{ 5.f };

const GenreMap Video::s_genresStrings{
	{"action", Genre::ACTION},
	{"drama", Genre::DRAMA},
	{"mystery", Genre::MYSTERY}
};

const VideoTypeMap Video::s_videoTypesStrings{
	{"movie", VideoType::MOVIE},
	{"episode", VideoType::SERIES_EPISODE}
};

Video::Video(const std::string& t_name, const std::string& t_id, unsigned t_duration, Genre t_genre, VideoType t_type):
	m_name{ t_name }, m_id{ t_id }, m_duration{ t_duration }, m_genre{ t_genre }, m_type{t_type}{}

const std::string& Video::getName() const { return m_name; }

float Video::getRating() const {
	float accRating{ 0.f };
	int size{ 0 };
	for (const auto& rating : m_ratings) {
		accRating += rating;
		size++;
	}
	return size != 0 ? accRating / size : 0;
}

const std::string& Video::getId() const { return m_id; }

VideoType Video::getType() const{ return m_type;}

unsigned Video::getDuration() const {return m_duration;}

Genre Video::getGenre() const { return m_genre; }

void Video::rate(float t_rating) { m_ratings.emplace_back(t_rating); }

std::string Video::formattedDuration() const{
	unsigned minutes{ m_duration / 60 };
	unsigned hours{ minutes / 60 };
	minutes %= 60;
	unsigned seconds = m_duration % 60;

	std::string minutesStr{ std::to_string(minutes) };
	if (minutesStr.size() == 1U) {
		minutesStr = '0' + minutesStr;
	}

	std::string secondsStr{ std::to_string(seconds) };
	if (secondsStr.size() == 1U) {
		secondsStr = '0' + secondsStr;
	}

	std::ostringstream s;
	s << hours << ':' << minutesStr << ':' << secondsStr;
	return s.str();
}


void Video::printTableHeader(std::ostream& t_out){
	const auto& sep{ s_tbl.m_separator };
	const auto& t{ s_tbl };
	
	t_out << sep << ' ' <<
		std::left << std::setw(t.m_id) << t.m_idHeader << sep <<
		std::left << std::setw(t.m_name) << t.m_nameHeader << sep <<
		std::left << std::setw(t.m_duration) << t.m_durationHeader << sep <<
		std::left << std::setw(t.m_genre) << t.m_genreHeader << sep <<
		std::left << std::setw(t.m_rating) << t.m_ratingHeader << sep <<
		std::left << std::setw(t.m_type) << t.m_typeHeader << sep;	
}

void Video::print(std::ostream& t_out) const{
	const auto& sep{ s_tbl.m_separator };
	const auto& t{ s_tbl };
	t_out << sep << ' ' <<
		std::left << std::setw(t.m_id) << m_id << sep <<
		std::left << std::setw(t.m_name) << m_name << sep <<
		std::left << std::setw(t.m_duration) << formattedDuration() << sep <<
		std::left << std::setw(t.m_genre) << getStrFromGenre(m_genre) << sep <<
		std::left << std::setw(t.m_rating) << std::fixed << std::setprecision(1) <<  getRating() << sep <<
		std::left << std::setw(t.m_type) << getStrFromVideoType(m_type) << sep;
}

const std::string& Video::getStrFromGenre(Genre t_genre){
	for (const auto& p : s_genresStrings) {
		if (t_genre == p.second) { return p.first; }
	}
	return n::emptyStr;
}

bool Video::getGenreFromStr(const std::string& t_str, Genre& t_outGenre){
	auto it{ s_genresStrings.find(t_str) };
	if (it != s_genresStrings.cend()) {
		t_outGenre = it->second;
		return true;
	}
	return false;
}	

bool Video::getVideoTypeFromStr(const std::string& t_str, VideoType& t_outType){
	auto it{s_videoTypesStrings.find(t_str)};
	if (it != s_videoTypesStrings.cend()) {
		t_outType = it->second;
		return true;
	}
	return false;
}

const std::string& Video::getStrFromVideoType(VideoType t_type){
	for (const auto & p : s_videoTypesStrings) {
		if (t_type == p.second) { return p.first; }
	}
	return n::emptyStr;
}

std::ostream& operator<<(std::ostream& t_out, const Video& t_video){
	t_video.print(t_out);
	return t_out;
}
