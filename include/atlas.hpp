#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <boost/math/common_factor_rt.hpp>

namespace sfext {

/// Image Atlas Generator
/**
 * It generates an atlas of multiple images. Each image is identified by a key,
 * which type is templated here as `Key`.
 * Typical usage: add multiple images, generate the atlas image and iterate the
 * atlas object to fetch all frames' clipping information.
 * A given `Key` is only valid if `std::hash<Key>` and `std::equal_to<Key>`
 * exist in order to use it as key of `std::unordered_map`. It also should be
 * convertable to string by `std::to_string` (or being string itself).
 */
template <typename Key>
class ImageAtlas final {
	private:
		/// Frame Chunk
		class Chunk {
			friend ImageAtlas<Key>;
			private:
				Key key;
				sf::Image const * image;
				sf::IntRect range; // describes position inside atlas
			public:
				/// Returns key
				/**
				 * @return key to uniquely identify this chunk
				 */
				Key getKey() const;
				/// Returns clipping information
				/**
				 * @return clipping rectangle referring to the atlas image
				 */
				sf::IntRect getClipping() const;
		};
		
		using Iterator = typename std::vector<Chunk>::const_iterator;

		std::vector<Chunk> chunks; // added chunks
		
		/// Converts a key to a string (for throwing an exception)
		std::string keyToString(Key const & key);

	public:
		/// Try to add an image
		/**
		 * Creates a new chunk with `key` and a pointer to the given `image` lreference.
		 * This always succeeds because the atlas itself is not generated here
		 * @param key used to identify the image
		 * @param image should be added to the atlas
		 */
		void add(Key const & key, sf::Image const & image);
		/// Clears the entire atlas
		void clear();
		/// Generate the final image
		/**
		 * Generates the actual atlas image. This method provides an additional overload.
		 * When using this, the size of the target image can be specified. If it is not
		 * specified, it will use `sf::Texture::getMaximumSize()` as default.
		 * @throw std::length_error if an image is larger than the target
		 * @throw std::out_of_range if the target is too small to carry all images
		 * @param size determines the target image's size per dimension.
		 * @return image object containing all added images
		 */
		sf::Image generate();
		sf::Image generate(std::size_t size);
		
		/// Returns beginning of the chunks
		/**
		 * Returns an iterator to a constant chunk. @See ImageAtlas::Chunk
		 * @return iterator to beginning of chunks
		 */
		Iterator begin() const;
		/// Returns end of the chunks
		/**
		 * @return iterator to end of chunks
		 */
		Iterator end() const;
};

} // ::sfext

// ----------------------------------------------------------------------------
// implementation

namespace sfext {

template <typename Key>
Key ImageAtlas<Key>::Chunk::getKey() const {
	return key;
}

template <typename Key>
sf::IntRect ImageAtlas<Key>::Chunk::getClipping() const {
	return range;
}

template <>
std::string ImageAtlas<std::string>::keyToString(std::string const & key) {
	return key;
}

template <typename Key>
std::string ImageAtlas<Key>::keyToString(Key const & key) {
	return std::to_string(key);
}

template <typename Key>
void ImageAtlas<Key>::add(Key const & key, sf::Image const & image) {
	// add chunk
	auto img_size = image.getSize();
	Chunk chunk;
	chunk.key			= key;
	chunk.image			= &image;
	chunk.range.width	= img_size.x;
	chunk.range.height	= img_size.y;
	chunks.push_back(std::move(chunk));
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
		[](Chunk const & left, Chunk const & right) {
		auto a = left.range.width * left.range.height;
		auto b = right.range.width * right.range.height;
		return a > b;
	});

	// place chunks
	std::unordered_map<Key, Chunk const *> lookup;
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
