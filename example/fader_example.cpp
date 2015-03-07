#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include <SfmlExt/fader.hpp>

int main() {
	auto t = sf::milliseconds(50);
	sfext::MusicManager manager;
	float volume = 100.f;
	
	manager.play("data/wesnoth.org/battle_edit.ogg");
	
	while (true) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
			// fade-in / cross-fade
			manager.play("data/wesnoth.org/battle_edit.ogg");
			std::cout << "Fading tracks..." << std::endl;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			// fade-out
			manager.stop();
			std::cout << "Stopped music" << std::endl;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			// volume up
			volume = std::min(100.f, volume + 5.f);
			manager.setMaxVolume(volume);
			std::cout << "Set max volume to " << volume << "%" << std::endl;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			// volume up
			volume = std::max(0.f, volume - 5.f);
			manager.setMaxVolume(volume);
			std::cout << "Set max volume to " << volume << "%" << std::endl;
		}
		
		manager.update(t);
		sf::sleep(t);
	}
}
