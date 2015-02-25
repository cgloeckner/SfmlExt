#pragma once
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <boost/math/common_factor_rt.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sfext {

template <typename Key>
Chunk<Key>::Chunk(Key const & key, sf::Image&& img, sf::IntRect const & bounds, sf::Vector2f const & origin)
	: key{key}
	, image{img}
	, bounds{bounds}
	, target{0, 0, bounds.width, bounds.height}
	, origin{origin} {
}

// ---------------------------------------------------------------------------

template <typename Key>
bool AtlasGenerator<Key>::rowIsEmpty(sf::Image const & image, std::size_t row) {
	auto size = image.getSize();
	for (std::size_t x = 0u; x < size.x; ++x) {
		if (image.getPixel(x, row).a > 0) {
			return false;
		}
	}
	return true;
}

template <typename Key>
bool AtlasGenerator<Key>::colIsEmpty(sf::Image const & image, std::size_t col) {
	auto size = image.getSize();
	for (std::size_t y = 0u; y < size.y; ++y) {
		if (image.getPixel(col, y).a > 0) {
			return false;
		}
	}
	return true;
}

template <typename Key>
void AtlasGenerator<Key>::add(Key const & key, sf::Image&& image, sf::Vector2f origin) {
	auto size = sf::Vector2i{image.getSize()};
	sf::IntRect bounds{{0u, 0u}, size};
	
	// shrink from bottom
	while (rowIsEmpty(image, bounds.height - 2u)) {
		--bounds.height;
	}
	// shrink from right
	while (colIsEmpty(image, bounds.width - 2u)) {
		--bounds.width;
	}
	// shrink from top
	while (rowIsEmpty(image, bounds.top + 1u)) {
		++bounds.top;
		--bounds.height;
		--origin.y;
	}
	// shrink from left
	while (colIsEmpty(image, bounds.left + 1u)) {
		++bounds.left;
		--bounds.width;
		--origin.x;
	}
	
	// create chunk
	chunks.emplace_back(key, std::move(image), bounds, origin);
}

template <typename Key>
void AtlasGenerator<Key>::clear() {
	chunks.clear();
}

template <typename Key>
template <typename HashFunc>
bool AtlasGenerator<Key>::generate(sf::Vector2u const & min_step, std::size_t size, Atlas<Key, HashFunc>& atlas) {
	// sort chunks by size (descending)
	std::sort(chunks.begin(), chunks.end(),
		[](Chunk<Key> const & left, Chunk<Key> const & right) {
		auto a = left.bounds.width * left.bounds.height;
		auto b = right.bounds.width * right.bounds.height;
		return a > b;
	});

	// place chunks
	std::unordered_map<Key, Chunk<Key> const *, HashFunc> lookup;
	sf::Vector2i step_range{min_step};
	for (auto& chunk: chunks) {
		if (chunk.bounds.width > size || chunk.bounds.height > size) {
			throw std::runtime_error("Too small target size");
		}
		
		if (lookup.empty()) {
			// free space found
			step_range.x = chunk.bounds.width;
			step_range.y = chunk.bounds.height;
			// placed at (0,0)
			lookup[chunk.key] = &chunk;			
			continue;
		}

		// search for space
		auto max_left	= size - chunk.bounds.width;
		auto max_top	= size - chunk.bounds.height;
		bool added{false}; // true = chunk was added to atlas
		for (chunk.target.top = 0u; chunk.target.top <= max_top; chunk.target.top += step_range.y) {
			for (chunk.target.left = 0u; chunk.target.left <= max_left; chunk.target.left += step_range.x) {
				bool found{true}; // true = space was found
				// compare with already placed chunks
				for (auto const & pair: lookup) {
					if (chunk.target.intersects(pair.second->target)) {
						// collision with another chunk, skip this position
						found = false;
						break;
					}
				}

				if (found) {
					// recalc step range using gcd per dimension
					step_range.x = boost::math::gcd(step_range.x, chunk.bounds.width);
					step_range.y = boost::math::gcd(step_range.y, chunk.bounds.height);
					
					// note: this might speedup generation, but the result won't be perfect anymore
					step_range.x = std::max(step_range.x, static_cast<int>(min_step.x));
					step_range.y = std::max(step_range.y, static_cast<int>(min_step.y));
					
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
		return false;
	}
	
	// create actual atlas
	atlas.image.create(size, size, sf::Color::Transparent);
	atlas.frames.clear();
	for (auto const & chunk: chunks) {
		atlas.image.copy(chunk.image, chunk.target.left, chunk.target.top, chunk.bounds);
		AtlasFrame frame;
		frame.clipping = chunk.target;
		frame.origin = chunk.origin;
		atlas.frames[chunk.key] = std::move(frame);
	}
	
	return true;
}

} // ::sfext
