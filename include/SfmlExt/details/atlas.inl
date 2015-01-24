#pragma once
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <boost/math/common_factor_rt.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sfext {

template <typename Key>
Chunk<Key>::Chunk(Key const & key, sf::Image const & image)
	: key{key}
	, image{&image}
	, range{
		0, 0, static_cast<int>(image.getSize().x),
		static_cast<int>(image.getSize().y)
	} {
}

template <typename Key>
Key Chunk<Key>::getKey() const {
	return key;
}

template <typename Key>
sf::IntRect Chunk<Key>::getClipping() const {
	return range;
}

// ---------------------------------------------------------------------------

template <>
inline std::string ImageAtlas<std::string>::keyToString(std::string const & key) {
	return key;
}

template <typename Key>
std::string ImageAtlas<Key>::keyToString(Key const & key) {
	return std::to_string(key);
}

template <typename Key>
void ImageAtlas<Key>::add(Key const & key, sf::Image const & image) {
	chunks.emplace_back(key, image);
}

template <typename Key>
void ImageAtlas<Key>::clear() {
	chunks.clear();
}

template <typename Key>
sf::Image ImageAtlas<Key>::generate() {
	return generate(sf::Texture::getMaximumSize());
}

template <typename Key>
sf::Image ImageAtlas<Key>::generate(std::size_t size) {
	// sort chunks by size (descending)
	std::sort(chunks.begin(), chunks.end(),
		[](Chunk<Key> const & left, Chunk<Key> const & right) {
		auto a = left.range.width * left.range.height;
		auto b = right.range.width * right.range.height;
		return a > b;
	});

	// place chunks
	std::unordered_map<Key, Chunk<Key> const *> lookup;
	sf::Vector2i step_range{1u, 1u};
	for (auto& chunk: chunks) {
		if (chunk.range.width > size || chunk.range.height > size) {
			throw std::length_error(keyToString(chunk.key));
		}
		
		if (lookup.empty()) {
			// free space found
			step_range.x = chunk.range.width;
			step_range.y = chunk.range.height;
			// placed at (0,0)
			lookup[chunk.key] = &chunk;
			continue;
		}

		// search for space
		auto max_left	= size - chunk.range.width;
		auto max_top	= size - chunk.range.height;
		bool added{false}; // true = chunk was added to atlas
		for (chunk.range.top = 0u; chunk.range.top <= max_top; chunk.range.top += step_range.y) {
			for (chunk.range.left = 0u; chunk.range.left <= max_left; chunk.range.left += step_range.x) {
				bool found{true}; // true = space was found
				// compare with already placed chunks
				for (auto const & pair: lookup) {
					if (chunk.range.intersects(pair.second->range)) {
						// collision with another chunk, skip this position
						found = false;
						break;
					}
				}

				if (found) {
					// recalc step range using gcd per dimension
					step_range.x = boost::math::gcd(step_range.x, chunk.range.width);
					step_range.y = boost::math::gcd(step_range.y, chunk.range.height);
					// placed at current (top,left)
					added = true;
					lookup[chunk.key] = &chunk;
					break;
				}
			}
			if (added) {
				break;
			}
		}
		if (added) {
			continue;
		}

		// no space found
		throw std::out_of_range(keyToString(chunk.key));
	}

	// create target image
	sf::Image image;
	image.create(size, size, sf::Color::Transparent);
	for (auto const & chunk: chunks) {
		image.copy(*chunk.image, chunk.range.left, chunk.range.top);
	}

	return std::move(image);
}

template <typename Key>
typename ImageAtlas<Key>::Iterator ImageAtlas<Key>::begin() const {
	return chunks.cbegin();
}

template <typename Key>
typename ImageAtlas<Key>::Iterator ImageAtlas<Key>::end() const {
	return chunks.cend();
}

} // ::sfext
