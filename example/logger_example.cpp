#include <ctime>
#include <fstream>
#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <SfmlExt/logger.hpp>

/// Customized logger using a logfile, stdout and sf::Text to display the log
class MyLogger
		: public sfext::Logger
		, public sf::Drawable {
	private:
		sf::Text label;
		sfext::LogBuffer lines;
		std::ofstream file;
		
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			// just draw the label
			target.draw(label, states);
		}
		
		std::string getTimestring() {
			// create a string with the current date and time
			time_t rawtime;
			struct tm* timeinfo;
			char buffer[80];
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			strftime(buffer, 80, "[%x|%X] ", timeinfo);
			return std::string(buffer);
		}
		
	public:
		MyLogger(sf::Font& font, sf::Vector2f const & pos, sf::Color const & color, std::size_t num_lines, std::string const & filename)
			: label{}
			, lines{num_lines}
			, file{} {
			label.setFont(font);
			label.setCharacterSize(16);
			label.setColor(color);
			label.setPosition(pos);
			file.open(filename);
		}
		
		void operator()(std::string const & line) {
			auto prefix = getTimestring();
			
			// add the line to file, stdout and our logbuffer
			if (lines.isEmpty()) {
				// add first line
				file << prefix << line;
				std::cout << prefix << line;
				lines.addLine(prefix + line);
			} else if (lines.getLast().back() == '\n') {
				// add new line
				file << prefix << line;
				std::cout << prefix << line;
				lines.addLine(getTimestring() + line);
			} else {
				// append to previous line
				file << line;
				std::cout << line;
				lines.getLast() += line;
			}
			
			// update the label with the latest lines
			std::string buffer;
			for (auto const & line: lines) {
				buffer += line;
			}
			label.setString(buffer);
		}
};

int main() {
	// prepare rendering
	sf::RenderWindow window{{640, 480}, "Logger example"};
	sf::Font font;
	font.loadFromFile("data/wesnoth.org/DejaVuSans.ttf");
	
	// create two log instances
	MyLogger error{font, {0.f, 0.f}, {255, 100, 0}, 3, "error.log"};
	MyLogger debug{font, {0.f, 60.f}, sf::Color::White, 5, "debug.log"};
	
	// write to the loggers
	error << "please use your mouse ;)\n";
	debug << "Number: " << 123 << ", Color: " << sf::Color::Red << "\n"
		<< "Time: " << sf::seconds(3.5f) << " .. now press some keys :)\n";
	
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::KeyPressed) {
				// write key to the debug logger
				debug << "Key: " << event.key.code << "\n";
			} else if (event.type == sf::Event::MouseButtonPressed) {
				// write mouse button and position to the error logger
				sf::Vector2i pos(event.mouseButton.x, event.mouseButton.y);
				error << "Click: " << event.mouseButton.button << " at " << pos << "\n";
			}
		}
		
		// just draw them
		window.clear(sf::Color::Black);
		window.draw(error);
		window.draw(debug);
		window.display();
	}
}
