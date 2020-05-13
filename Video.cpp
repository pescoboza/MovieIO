#include "Video.hpp"
#include <iomanip>
#include <sstream>

namespace n {
	const std::string emptyStr{ "" };

};

// 		unsigned id, name, duration, genre, rating, type;
const Video::TableWidths Video::s_tblw{8U, 32U, 8U, 8U, 4U, 8U };

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
	return accRating / size;
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

void Video::print(std::ostream& t_out) const{
	t_out <<
		std::left << std::setw(s_tblw.m_id) << m_id <<
		std::left << std::setw(s_tblw.m_name) << m_name << 
		std::left << std::setw
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

