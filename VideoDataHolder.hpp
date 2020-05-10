#ifndef VIDEO_DATA_HOLDER_HPP
#define VIDEO_DATA_HOLDER_HPP

#include "Video.hpp"
#include <memory>
#include <vector>

using VideoPtr = std::unique_ptr<Video>;
using Videos = std::vector<VideoPtr>;

class VideoDataHolder {
	Videos m_videos;

public:
	VideoDataHolder();

};

#endif // !VIDEO_DATA_HOLDER_HPP