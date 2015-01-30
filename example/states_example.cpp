#include <algorithm>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <SfmlExt/states.hpp>

// app context holding a global font
struct MyContext {
	sf::Font font;
	
	MyContext() {
		font.loadFromFile("data/wesnoth.org/DejaVuSans.ttf");
	}
};

class AnotherState;

// demo state drawing a rectangle
class DemoState: public sfext::State<MyContext> {
	private:
		sf::RectangleShape shape;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			target.draw(shape);
		}
	public:
		DemoState(sfext::Application<MyContext>& application, MyContext& context, sf::Color const & color)
			: sfext::State<MyContext>{application, context} {
			auto& app = getApplication();
			auto& window = app.getWindow();
			auto size = static_cast<sf::Vector2f>(window.getSize());
			shape.setSize(size * 0.8f);
			shape.setPosition(size / 2.f);
			shape.setOrigin(size * 0.4f);
			shape.setFillColor(color);
		}
		void handle(sf::Event const & event) override {
			if (event.type == sf::Event::Closed) {
				// quit this state (-> quit app in this case)
				quit();
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
				// push successor state
				auto& app = getApplication();
				app.push<AnotherState>(sf::Color::Green);
			}
		}
		void update(sf::Time const & elapsed) override {
		}
};

// another state drawing a circle and a fps counter
class AnotherState: public sfext::State<MyContext> {
	private:
		sf::CircleShape shape;
		sf::Text fps;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			target.draw(shape);
			target.draw(fps);
		}
	public:
		AnotherState(sfext::Application<MyContext>& application, MyContext& context, sf::Color const & color)
			: sfext::State<MyContext>{application, context} {
			auto& app = getApplication();
			auto& window = app.getWindow();
			auto size = static_cast<sf::Vector2f>(window.getSize());
			shape.setPosition(size / 2.f);
			float radius = std::min(size.x, size.y) / 2.f;
			shape.setRadius(radius);
			shape.setOrigin({radius, radius});
			shape.setFillColor(color);
			fps.setFont(context.font);
		}
		void handle(sf::Event const & event) override {
			if (event.type == sf::Event::Closed ||
				(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)) {
				// quit this state (here: return to DemoState);
				quit();
			}
		}
		void update(sf::Time const & elapsed) override {
		}
		void onFramerateUpdate(float framerate) override {
			// update frame counter
			fps.setString(std::to_string(static_cast<int>(framerate)));
		}
};

int main() {
	// create context
	MyContext context;
	
	// create application (which creates a window and obtains the context)
	sfext::Application<MyContext> app{context, sf::VideoMode(640, 480), "states example"};
	app.getWindow().setVerticalSyncEnabled(true);
	
	// use DemoState as initial state (drawing a red rectangle)
	app.push<DemoState>(sf::Color::Red);
	
	// run the application's mainloop
	app.run();
}
