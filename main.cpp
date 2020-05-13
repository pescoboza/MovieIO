#include "VideoDataHolder.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
	try {

		if (argc != 2) {
			std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
			return 1;
		}
	
		VideoDataHolder videoSystem;

		std::cout << "Loading data..." << std::endl;
		videoSystem.parseInfoFromFile(argv[1]);
		
		// std::vector<Video*> videos;
		// videoSystem.getVideosOfGenre(,)
	
	}
	catch (std::exception& e){
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

// TODO: Debug parsing.
// TODO: Create user interface.