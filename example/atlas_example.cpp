#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <Thor/Animations.hpp>

#include <SfmlExt/atlas.hpp>

// just a helper (see below)
bool create_atlas(sfext::Atlas<int>& atlas) {
	// Load images from disk and append to the atlas with an origin
	sfext::AtlasGenerator<int> builder;
	for (auto i = 0u; i < 5u; ++i) {
		sf::Image img;
		std::string fname{"data/human_melee_" + std::to_string(i) + ".png"};
		img.loadFromFile(fname);
		builder.add(i, std::move(img), {32.f, 32.f});
	}

	// Generate atlas image
	return builder.generate({16u, 16u}, 256, atlas);
}

int main() {
	// create atlas from images (see above)
	sfext::Atlas<int> atlas;
	if (!create_atlas(atlas)) {
		std::cout << "Building failed!" << std::endl;
		return 0;
	}
	atlas.image.saveToFile("atlas.png");
	// now we have a "atlas.png" and our clipping information (= atlas.frames)

	// Create Animation -- works just as normal (using SFML and Thor)
	thor::FrameAnimation attack;
	for (auto i = 0u; i < 5u; ++i) {
		attack.addFrame(1.f, atlas.frames[i].clipping, atlas.frames[i].origin);
	}
	thor::Animator<sf::Sprite, std::string> animator;
	animator.addAnimation("attack", attack, sf::milliseconds(625)); // 5x 125ms

	// Load frameset and bind it to a sprite
	sf::Texture frameset;
	frameset.loadFromFile("atlas.png"); // <-- file was created by atlas
	sf::Sprite sprite;
	sprite.setTexture(frameset);
	sprite.setPosition({160, 120});
	sprite.setScale({2.f, 2.f});

	// Render!
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
