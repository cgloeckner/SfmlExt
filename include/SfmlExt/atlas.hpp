#pragma once
#include <vector>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace sfext {

template <typename Key>
class ImageAtlas;

/// Frame Chunk
template <typename Key>
class Chunk final {
	friend class ImageAtlas<Key>;
	
	private:
		Key key;
		sf::Image image;
		sf::IntRect range; // describes position inside atlas
		
	public:
		/// Create a chunk
		/**
		 * @param key to identify the image with
		 * @param image to use
		 */
		Chunk(Key const & key, sf::Image&& image);
		
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
		using ChunkArray	= typename std::vector<Chunk<Key>>;
		using Iterator		= typename ChunkArray::const_iterator;

		ChunkArray chunks; // added chunks
		
		/// Converts a key to a string (for throwing an exception)
		std::string keyToString(Key const & key);

	public:
		/// Try to add an image
		/**
		 * Creates a new chunk with `key` and a pointer to the given `image`.
		 * This always succeeds because the atlas itself is not generated here
		 * @param key used to identify the image
		 * @param image should be moved to the atlas
		 */
		void add(Key const & key, sf::Image&& image);
		
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

// include implementation details
#include <SfmlExt/details/atlas.inl>
