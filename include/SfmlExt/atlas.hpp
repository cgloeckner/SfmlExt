#pragma once
#include <vector>
#include <unordered_map>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace sfext {

template <typename Key>
class AtlasGenerator;

/// Frame Chunk
template <typename Key>
struct Chunk final {
	Key key;
	sf::Image image;
	sf::IntRect bounds; // describe image rectangle (changed if shrinked)
	sf::IntRect target; // describes position inside atlas
	sf::Vector2f origin; // image's origin (changed if shrinked)
		
	/// Create a chunk
	/**
	 * @param key to identify the image with
	 * @param image to use
	 * @param bounds describing the image's source rectangle
	 * @param origin describing the render origin for e.g. sf::Sprite
	 */
	Chunk(Key const & key, sf::Image&& image, sf::IntRect const & bounds, sf::Vector2f const & origin);
};

struct AtlasFrame {
	sf::IntRect clipping;
	sf::Vector2f origin;
};

template <typename Key, typename HashFunc=std::hash<Key>>
struct Atlas {
	sf::Image image;
	std::unordered_map<Key, AtlasFrame, HashFunc> frames;
};

// ---------------------------------------------------------------------------

/// Image Atlas Generator
/**
 * It generates an atlas of multiple images. Each image is identified by a key,
 * which type is templated here as `Key`.
 * Typical usage: add multiple images and generate the atlas image to obtain
 * the final image and each frame's information.
 * Each frame is strinked to it's minimum size. The provided origin will be
 * modified in order to enable exact rendering later on.
 */
template <typename Key>
class AtlasGenerator final {
	private:
		std::vector<Chunk<Key>> chunks; // added chunks
		
		/// Check whether a row only holds transparent pixels
		bool rowIsEmpty(sf::Image const & image, std::size_t row);
		
		/// Check whether a column only holds transparent pixels
		bool colIsEmpty(sf::Image const & image, std::size_t col);
		
	public:
		/// Try to add an image
		/**
		 * Creates a new chunk with `key` and the given `image`. The image
		 * is shrinked to it's minimum size. This modifies the `origin` for
		 * rendering with e.g. sf::Sprite.
		 * This always succeeds because the atlas itself is not generated here.
		 * @param key used to identify the image
		 * @param image should be moved to the atlas
		 * @param origin for the original image
		 */
		void add(Key const & key, sf::Image&& image, sf::Vector2f origin);
		
		/// Clears the entire atlas
		void clear();
		
		/// Generate the final image
		/**
		 * Generates the actual atlas image. To increase performance, specify
		 * `min_step` in order to increase the minimum step range for searching
		 * a free location. Using min_step greater (1,1) will increase exection
		 * speed but the result won't be perfect anymore.
		 * The `HashFunc` argument is unsed in order to arrange all frames
		 * inside an `std::unordered_map`.
		 * @throw std::length_error if an image is larger than the target
		 * @param min_step determines the minimum step range for searching a free spot
		 * @param size determines the target image's size per dimension.
		 * @param [out] atlas which is generated
		 * @return true if success, false if not all images could be placed.
		 */
		template <typename HashFunc=std::hash<Key>>
		bool generate(sf::Vector2u const & min_step, std::size_t size, Atlas<Key, HashFunc>& atlas);
};

} // ::sfext

// include implementation details
#include <SfmlExt/details/atlas.inl>
