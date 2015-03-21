#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <SfmlExt/cache.hpp>

int main() {
	sfext::Cache cache;
	sf::Sprite sprite;
	sf::Sound sound;
	
	sprite.setTexture(cache.get<sf::Texture>("data/wesnoth.org/attack0.png"));
	sound.setBuffer(cache.get<sf::SoundBuffer>("data/wesnoth.org/axe.ogg"));
	
	// play sounds and render sprite
	sf::RenderWindow window{{320, 240}, "Animation example"};
	sound.play();
	
	// force texture to be reloaded
	cache.reload<sf::Texture>("data/wesnoth.org/attack0.png");
	
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) { window.close(); }
		}
		
		window.clear(sf::Color::Black);
		window.draw(sprite);
		window.display();
	}
}
