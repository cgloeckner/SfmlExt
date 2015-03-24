#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <SfmlExt/menu.hpp>

// A customized button
class MyButton: public sfext::Button {
	private:
		sf::Text label;
		
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			target.draw(label, states);
		}
	public:
		MyButton(std::string const & caption, sf::Font const & font)
			: sfext::Button{}
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
};

// A customized checkbox
class MyCheckbox: public sfext::Button {
	private:
		sf::Text label;
		std::string caption;
		bool checked;
		
		void update() {
			if (checked) {
				label.setString("[X] " + caption);
			} else {
				label.setString("[  ] " + caption);
			}
			// align label centered
			auto rect = label.getLocalBounds();
			label.setOrigin({
				rect.left + rect.width / 2.f,
				rect.top + rect.height / 2.f
			});
		}
		void onActivate() override {
			checked = !checked;
			update();
		}
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			target.draw(label, states);
		}
	public:
		MyCheckbox(std::string const & caption, sf::Font const & font)
			: sfext::Button{}
			, label{}
			, caption{caption}
			, checked{false} {
			label.setFont(font);
			update();
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
		bool isChecked() const {
			return checked;
		}
};

// A customized selection
class MySelect: public sfext::Select {
	private:
		sf::Text label;
		
		void onChanged() override {
			auto item = at(getIndex());
			if (getIndex() > 0u) {
				item = "< " + item;
			} else {
				item = "   " + item;
			}
			if (getIndex() < size() - 1) {
				item += " >";
			} else {
				item += "   ";
			}
			label.setString(item);
			// align label centered
			auto rect = label.getLocalBounds();
			label.setOrigin({
				rect.left + rect.width / 2.f,
				rect.top + rect.height / 2.f
			});
		}
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			target.draw(label, states);
		}
	public:
		MySelect(sf::Font const & font)
			: sfext::Select{}
			, label{"undefined", font} {
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
};

// some integer constants to identify the widgets
int const START_BTN = 0;
int const OPTION_BOX = 1;
int const SETTINGS_BTN = 2;
int const MODE_SELECT = 3;
int const QUIT_BTN = 4;

int main() {
	// prepare rendering
	sf::RenderWindow window{{320, 320}, "Menu example"};
	sf::Font font;
	font.loadFromFile("data/wesnoth.org/DejaVuSans.ttf");
	
	// create menu and widgets
	sfext::Menu<int> menu; // identify widgets by an int
	auto& start		= menu.acquire<MyButton>(START_BTN, "start game", font);
	auto& option	= menu.acquire<MyCheckbox>(OPTION_BOX, "hidden option?", font);
	auto& settings	= menu.acquire<MyButton>(SETTINGS_BTN, "settings", font);
	auto& mode		= menu.acquire<MySelect>(MODE_SELECT, font);
	menu.acquire<MyButton>(QUIT_BTN, "quit", font);
	menu.acquire<MyButton>(5, "dummy", font);
	
	// release widget #5
	menu.release(5);
	
	// query a widget QUIT_BTN
	auto& quit = menu.query<MyButton>(QUIT_BTN);
	
	// place widgets
	start.setPosition({160, 30});
	option.setPosition({160, 70});
	settings.setPosition({160, 110});
	mode.setPosition({160, 150});
	quit.setPosition({160, 280});
		
	// bind callbacks
	start.bind([]() {
		std::cout << "Starting option is just a dummy :)" << std::endl;
	});
	settings.bind([&]() {
		std::cout << "It's like black magic ... o.o" << std::endl;
		option.setVisible(!option.isVisible());
	});
	option.bind([&]() {
		std::cout << "Ok" << std::endl;
	});
	mode.bind([&]() {
		std::cout << "Yeah!" << std::endl;
	});
	quit.bind([&]() {
		std::cout << "Cya^^" << std::endl;
		window.close();
	});
	
	// bind actions
	menu.bind(
		sfext::MenuAction::Activate,
		thor::Action(sf::Keyboard::Return, thor::Action::PressOnce)
	);
	menu.bind(
		sfext::MenuAction::NavigatePrev,
		thor::Action(sf::Keyboard::Up, thor::Action::PressOnce)
	);
	menu.bind(
		sfext::MenuAction::NavigateNext,
		thor::Action(sf::Keyboard::Down, thor::Action::PressOnce)
	);
	menu.bind(
		sfext::MenuAction::AlternatePrev,
		thor::Action(sf::Keyboard::Left, thor::Action::PressOnce)
	);
	menu.bind(
		sfext::MenuAction::AlternateNext,
		thor::Action(sf::Keyboard::Right, thor::Action::PressOnce)
	);
	
	// some more setup
	mode.push_back("640x480");
	mode.push_back("800x600");
	mode.push_back("1024x768");
	mode.push_back("1400x900");
	mode.push_back("very large");
	mode.push_back("extremly large");
	mode.push_back("xD");
	mode.setIndex(3);
	option.setVisible(false);
	
	// set focus via key
	menu.setFocus(SETTINGS_BTN);
	// or via the widget itself
	menu.setFocus(option);
	
	// here we go: usage is simple
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			menu.handle(event); // collect events
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
		// process all events
		menu.update();
		
		// standard drawing
		window.clear(sf::Color::Black);
		window.draw(menu);
		window.display();
	}
}
