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
		videoSystem.parseInfoFromFile(argv[1]);
	}
	catch (std::exception& e){
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}