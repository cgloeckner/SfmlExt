#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <SfmlExt/splitcam.hpp>

// run this example with "<binary> [<num_cams>]"
int main(int argc, char* argv[]) {
	std::size_t num_cams = 3;
	if (argc == 2) {
		try {
			num_cams = std::stoul(std::string(argv[1]));
		} catch (...) {
		}
	}
	
	// prepare rendering
	sf::RenderWindow window{{640, 480}, "Splitcam example"};
	sf::Font font;
	font.loadFromFile("data/wesnoth.org/DejaVuSans.ttf");
	sf::Text label;
	label.setFont(font);
	sf::RectangleShape shape;
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineColor(sf::Color::Red);
	shape.setOutlineThickness(-3.f);
	
	// create cameras
	std::vector<sfext::Camera> cams;
	for (std::size_t id = 0u; id < num_cams; ++id) {
		cams.emplace_back(window, id, num_cams);
	}
	
	// here we go: usage is simple
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
		
		window.clear(sf::Color::Black);
		// render all cameras
		for (auto& cam: cams) {
			label.setString("Camera #" + std::to_string(cam.getIndex()));
			// center label
			auto rect = label.getLocalBounds();
			label.setOrigin({
				rect.left + rect.width / 2.f,
				rect.top + rect.height / 2.f
			});
			auto size = cam.getSize();
			label.setPosition(size / 2.f);
			// resize shape
			shape.setSize(size);
			// draw cam
			window.setView(cam);
			window.draw(shape);
			window.draw(label);
		}
		window.display();
	}
}
