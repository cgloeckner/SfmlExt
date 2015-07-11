#include <fstream>
#include <iostream>
#include <SFML/Window.hpp>

#include <SfmlExt/logger.hpp>

int main() {
	sfext::Logger err, dbg;
	std::ofstream error_file{"error.log"};
	
	// prepare rendering
	sf::Window window{{640, 480}, "Logger example"};
	
	// make `err` write to file and stderr
	err.add(error_file);
	err.add(std::cerr);
	// make `dbg` write to stdout 
	dbg.add(std::cout);
	
	// write to the loggers
	err << "please use your mouse ;)\n";
	dbg << "Number: " << 123 << ", Color: " << sf::Color::Red << "\n"
		<< "Time: " << sf::seconds(3.5f) << " .. now press some keys :)\n";
	
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				dbg << sfext::now << "Window closed\n";
				window.close();
			} else if (event.type == sf::Event::KeyPressed) {
				// write key to the debug logger
				dbg << "Key: " << event.key.code << "\n";
			} else if (event.type == sf::Event::MouseButtonPressed) {
				// write mouse button and position to the error logger
				sf::Vector2i pos(event.mouseButton.x, event.mouseButton.y);
				err << "Click: " << event.mouseButton.button << " at " << pos << "\n";
			}
		}
		
		window.display();
	}
}
