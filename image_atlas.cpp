#include <cassert>
#include <boost/math/common_factor_rt.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "image_atlas.hpp"

ImageAtlas::ImageAtlas()
	: ImageAtlas(sf::Texture::getMaximumSize()) {
}

ImageAtlas::ImageAtlas(std::size_t size)
	: chunks{}
	, size{size}
	, step_range{1u, 1u} {
}

bool ImageAtlas::add(sf::Image const & image, sf::Vector2u& offset) {
	auto img_size = image.getSize();
	Chunk chunk;
	chunk.image			= &image;
	chunk.size.width	= static_cast<unsigned int>(img_size.x);
	chunk.size.height	= static_cast<unsigned int>(img_size.y);
	assert(chunk.size.width <= size);
	assert(chunk.size.height <= size);
	auto max_width		= size - chunk.size.width;
	auto max_height		= size - chunk.size.height;
	
	if (chunks.empty()) {
		// free space found
		offset.x = chunk.size.left;
		offset.y = chunk.size.top;
		step_range.x = chunk.size.width;
		step_range.y = chunk.size.height;
		chunks.push_back(std::move(chunk));
		return true;
	}

	// search for space
	for (chunk.size.top = 0u; chunk.size.top < max_height; chunk.size.top += step_range.y) {
		for (chunk.size.left = 0u; chunk.size.left < max_width; chunk.size.left += step_range.x) {
			bool found{true};
			for (auto const & other: chunks) {
				if (chunk.size.intersects(other.size)) {
					found = false;
					break;
				}
			}
			
			if (found) {
				// recalculate step range using gcd per dimension
				step_range.x = boost::math::gcd(step_range.x, chunk.size.width);
				step_range.y = boost::math::gcd(step_range.y, chunk.size.height);
				// place chunk
				offset.x = chunk.size.left;
				offset.y = chunk.size.top;
				chunks.push_back(std::move(chunk));

				return true;
			}
		}
	}

	// no space available
	return false;
}

sf::Image ImageAtlas::generate() const {
	sf::Image image;
	image.create(size, size, sf::Color::Transparent);
	for (auto const & c: chunks) {
		image.copy(*c.image, c.size.left, c.size.top);
	}
	return std::move(image);
}

