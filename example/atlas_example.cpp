#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <Thor/Animation.hpp>

#include <SfmlExt/atlas.hpp>

// clipping data per frame
using AtlasData = std::map<std::string, sf::IntRect>;

// just a helper (see below)
AtlasData create_atlas() {
	// Load images from disk and append to the atlas
	// note: the atlas itself doesn't hold images, but non-owning pointers
	//		so here the images are stored in a std::vector
	//		to avoid rearrangment of elements while push_back, we resize the
	//		container at the beginning, so the five images already exist
	//		and pointers to those images (as used inside the atlas!) are
	//		not invalidated!
	//	brief: all added images must stay at their memory addresses until
	//		the atlas finished genertion
	std::vector<sf::Image> images;
	sfext::ImageAtlas<std::string> atlas;
	images.resize(5);
	for (auto i = 0u; i < 5u; ++i) {
		std::string fname{"data/wesnoth.org/attack" + std::to_string(i) + ".png"};
		images.at(i).loadFromFile(fname);
		atlas.add(fname, images.at(i));
	}
	
	// Generate atlas image
	auto target = atlas.generate(256); // limit target image to 256x256
	target.saveToFile("atlas.png");
	
	// Export all clippingdata to a user-defined structure
	// here: simple lookup-table
	AtlasData lookup;
	for (auto const & chunk: atlas) {
		lookup[chunk.getKey()] = chunk.getClipping();
	}
	return std::move(lookup);
}

int main() {
	// create atlas from images (see above)
	// alternative: load clipping data from a file
	auto clipping = create_atlas();
	// now we have a "atlas.png" and our clipping information

	// Create Animation -- works just as normal (using SFML and Thor)
	thor::FrameAnimation attack;
	for (auto i = 0u; i < 5u; ++i) {
		std::string fname{"data/wesnoth.org/attack" + std::to_string(i) + ".png"};
		attack.addFrame(1.f, clipping[fname]);
	}
	thor::Animator<sf::Sprite, std::string> animator;
	animator.addAnimation("attack", attack, sf::milliseconds(625)); // 5x 125ms

	// Load frameset and bind it to a sprite
	sf::Texture frameset;
	frameset.loadFromFile("atlas.png"); // <-- file was created by atlas
	sf::Sprite sprite;
	sprite.setTexture(frameset);

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
