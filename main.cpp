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

		videoSystem.parseInfoFromFile(argv[1]);
		videoSystem.start();

	
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

// TODO: Create user incomposting fat terface.