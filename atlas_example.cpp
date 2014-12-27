#include <iostream>
#include <cassert>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <Thor/Animation.hpp>

#include "image_atlas.hpp"

int main() {
	// ------------------------------------------------------------------------
	// Create Image Atlas
	AtlasMapper<std::string> mapper;
	for (auto i = 1u; i <= 5; ++i) {
		sf::Image tmp;
		std::string fname{"wraith-s-attack-"+std::to_string(i)+".png"};
		assert(tmp.loadFromFile(fname));
		mapper.addImage(fname, std::move(tmp));
	}
	auto atlas = mapper.generate(256u);
	atlas.saveToFile("out.png");

	// ------------------------------------------------------------------------
	// Create Animation
	thor::FrameAnimation attack;
	for (auto i = 1u; i <= 5; ++i) {
		std::string fname{"wraith-s-attack-"+std::to_string(i)+".png"};
		attack.addFrame(1.f, mapper.getClipping(fname));
	}
	
	thor::Animator<sf::Sprite, std::string> animator;
	animator.addAnimation("attack", attack, sf::milliseconds(625)); // 5*125ms

	// ------------------------------------------------------------------------
	// Create Sprite
	sf::Texture frameset;
	assert(frameset.loadFromFile("out.png"));
	sf::Sprite sprite;
	sprite.setTexture(frameset);

	// ------------------------------------------------------------------------
	// Render
	sf::RenderWindow window{{320, 240}, "Animation example"};
	sf::Clock clock;
	animator.playAnimation("attack");

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) { window.close(); }
		}
		auto elapsed_time = clock.restart();
		
		animator.update(elapsed_time);
		animator.animate(sprite);
		
		if (!animator.isPlayingAnimation()) {
			animator.playAnimation("attack");
		}
		
		window.clear(sf::Color::Black);
		window.draw(sprite);
		window.display();
	}
}

