#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <SfmlExt/lightui.hpp>

class MyOption
		: public sfext::Widget {
		
	private:
		sf::Text label;
		
	public:
		MyOption(std::string const & caption, sf::Font const & font)
			: sfext::Widget{}
			, label{caption, font} {
			// align label centered
			auto rect = label.getLocalBounds();
			label.setOrigin({
				rect.left + rect.width / 2.f,
				rect.top + rect.height / 2.f
			});
		}
		
		void setFocus(bool focused) override {
			if (focused) {
				label.setColor(sf::Color::Yellow);
			} else {
				label.setColor(sf::Color::White);
			}
		}
		
		void setPosition(sf::Vector2f const & pos) override {
			label.setPosition(pos);
		}
		
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			target.draw(label, states);
		}
};

int main() {
	// prepare rendering
	sf::RenderWindow window{{320, 240}, "LightUI example"};
	sf::Clock clock;
	sf::Font font;
	font.loadFromFile("data/wesnoth.org/DejaVuSans.ttf");
	
	// create gui and widgets
	sfext::Gui gui{sf::milliseconds(150)};
	auto& start		= gui.create<MyOption>("start game", font);
	auto& load		= gui.create<MyOption>("load game", font);
	auto& settings	= gui.create<MyOption>("settings", font);
	auto& quit		= gui.create<MyOption>("quit", font);
	
	// place widgets
	start.setPosition({160, 30});
	load.setPosition({160, 70});
	settings.setPosition({160, 110});
	quit.setPosition({160, 210});
	
	// hide load-widget
	load.setVisible(false);
	
	// bind actions
	gui.bindAction(
		sfext::Action::Activate,
		// bind return, space and gamepad button #0 on release
		thor::Action(sf::Keyboard::Return, thor::Action::ReleaseOnce) ||
		thor::Action(sf::Keyboard::Space, thor::Action::ReleaseOnce) ||
		thor::Action(thor::joystick(0).button(0), thor::Action::ReleaseOnce)
	);
	gui.bindAction(
		sfext::Action::Up,
		// bind key up and gamepad y-axis up* on hold
		thor::Action(sf::Keyboard::Up) ||
		thor::Action(thor::joystick(0).axis(sf::Joystick::Y).below(-15.f))
	);
	gui.bindAction(
		sfext::Action::Down,
		// bind key down and gamepad y-axis down* on hold
		thor::Action(sf::Keyboard::Down) ||
		thor::Action(thor::joystick(0).axis(sf::Joystick::Y).above(15.f))
	);
	// (*) settings work at least for my device - might be different from your's
	
	// create lambda callbacks
	auto onStart = []() {
		std::cout << "Starting option is just a dummy :)" << std::endl;
	};
	auto onLoad = []() {
		std::cout << "Loading doesn't work neither" << std::endl;
	};
	auto onSettings = [&]() {
		std::cout << "Aaah c'mon XD" << std::endl;
		// toggle load-widget's visibility
		load.setVisible(!load.isVisible());
	};
	auto onQuit = [&]() {
		std::cout << "Cya^^" << std::endl;
		window.close();
	};
	
	// bind callbacks
	gui.bindCallback(start,		onStart);
	gui.bindCallback(load,		onLoad);
	gui.bindCallback(settings,	onSettings);
	gui.bindCallback(quit,		onQuit);
	
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			gui.notify(event); // collect events
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
		// process all events
		gui.update(clock.restart());
		
		// standard drawing
		window.clear(sf::Color::Black);
		window.draw(gui);
		window.display();
	}
}
