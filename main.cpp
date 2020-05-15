#include "VideoDataHolder.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
#ifndef _DEBUG
	try {
#endif // ! _DEBUG
		if (argc != 2) {
			std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
			return 1;
		}
	
		VideoDataHolder videoSystem;

		std::cout << "Loading data..." << std::endl;
		videoSystem.parseInfoFromFile(argv[1]);
		
		VideosVec videos;
		videoSystem.getVideos(videos, "", "", "", {0.f, 5.f});
		VideoDataHolder::printVideos(videos, 10U);
		
	// TODO: Add main loop

#ifndef _DEBUG	
	}
	catch (std::exception& e){
		std::cerr << e.what() << std::endl;

		return 1;
	}
#endif // ! _DEBUG

	return 0;
}

// TODO: Debug parsing.
// TODO: Create user interface.