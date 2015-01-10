#include <cassert>

#include <atlas.hpp>

void test_atlas_tooLargeChunk() {
	sfext::ImageAtlas<int> atlas;
	sf::Image img;
	img.create(1024, 1024, sf::Color::Red);
	atlas.add(0, img);
	bool error{false};
	
	try {
		atlas.generate(512);
	} catch (std::length_error const &) {
		error = true;
	}
	assert(error);
}

void test_atlas_tooManyChunks() {
	sfext::ImageAtlas<int> atlas;
	std::vector<sf::Image> images;
	images.resize(200);
	int i = 0;
	for (auto& img: images) {
		img.create(64, 64, sf::Color::Blue);
		atlas.add(i++, img);
	}
	bool error{false};

	error = false;
	try {
		atlas.generate(512);
	} catch (std::out_of_range const &) {
		error = true;
	}
	assert(error);
}

void test_atlas_maxNumChunks() {
	sfext::ImageAtlas<int> atlas;
	std::vector<sf::Image> images;
	images.resize(64);
	int i = 0;
	for (auto& img: images) {
		img.create(64, 64, sf::Color::Blue);
		atlas.add(i++, img);
	}
	bool error{false};

	error = false;
	try {
		atlas.generate(512);
	} catch (std::out_of_range const &) {
		error = true;
	}
	assert(!error);
}

void test_atlas_typicalNumChunks() {
	sfext::ImageAtlas<int> atlas;
	std::vector<sf::Image> images;
	images.resize(30);
	int i = 0;
	for (auto& img: images) {
		img.create(64, 64, sf::Color::Blue);
		atlas.add(i++, img);
	}
	bool error{false};

	error = false;
	try {
		atlas.generate(512);
	} catch (std::out_of_range const &) {
		error = true;
	}
	assert(!error);
}
