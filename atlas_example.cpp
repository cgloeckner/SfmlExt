#include <unordered_map>
#include <image_atlas.hpp>

namespace sf {

using UintRect = sf::Rect<unsigned int>;

}

class MyPacker {
	private:	
		using ImgPair = std::pair<std::string, sf::Image>;

		std::vector<ImgPair> images;
		std::unordered_map<std::string, sf::UintRect> clipping;

	public:
		void addImage(std::string const & filename);

		sf::Image generate(std::size_t size);
		sf::UintRect getClipping(std::string const & filename) const;
};

// ----------------------------------------------------------------------------

#include <algorithm>
#include <cassert>

void MyPacker::addImage(std::string const & filename) {
	images.push_back(std::move(make_pair(filename, sf::Image{})));
	assert(images.back().second.loadFromFile(filename));
}

sf::Image MyPacker::generate(std::size_t size) {
	ImageAtlas atlas{size};
	clipping.clear();
	sf::Vector2u pos;
	// sort images by size (descending)
	std::sort(images.begin(), images.end(),
		[](ImgPair const & left, ImgPair const & right) {
		auto size_a = left.second.getSize();
		auto size_b = right.second.getSize();
		return size_a.x * size_a.y > size_b.x * size_b.y;
	});
	// add images to atlas
	for (auto const & pair: images) {
		if (atlas.add(pair.second, pos)) {
			clipping[pair.first] = sf::UintRect{pos, pair.second.getSize()};
		}
	}
	return std::move(atlas.generate());
}

sf::UintRect MyPacker::getClipping(std::string const & filename) const {
	return clipping.at(filename);
}

// ----------------------------------------------------------------------------

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <Thor/Animation.hpp>

int main() {
	// ------------------------------------------------------------------------
	// Create Image Atlas

	MyPacker packer;
	for (auto i = 1u; i <= 5; ++i) {
		packer.addImage("wraith-s-attack-"+std::to_string(i)+".png");
	}
	auto atlas = packer.generate(256u);
	atlas.saveToFile("out.png");

	// ------------------------------------------------------------------------
	// Create Animation
	thor::FrameAnimation attack;
	for (auto i = 1u; i <= 5; ++i) {
		std::string fname{"wraith-s-attack-"+std::to_string(i)+".png"};
		attack.addFrame(1.f, static_cast<sf::IntRect>(packer.getClipping(fname)));
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

