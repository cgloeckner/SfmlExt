#pragma once
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <unordered_map>
#include <algorithm>

/// Helper class
class ImageAtlas {
	private:
		struct Chunk {
			sf::Image const * image;
			sf::Rect<unsigned int> size;
		};

		std::vector<Chunk> chunks;
		std::size_t size;
		sf::Vector2u step_range;

	public:
		/// Create empty image atlas with the maximum supported size
		ImageAtlas();
		/// Create empty image atlas with a given size
		ImageAtlas(std::size_t size);
		/// Try to add an image; offset refers to position within atlas
		bool add(sf::Image const & image, sf::Vector2u& offset);
		/// Generate the final image
		sf::Image generate() const;
};

// ----------------------------------------------------------------------------

/// Maps images (identified by key) to a clipping rectangle within the altas
template <typename Key>
class AtlasMapper {
	private:
		using ImgPair = std::pair<Key, sf::Image>;
		std::vector<ImgPair> images;
		std::unordered_map<Key, sf::IntRect> clipping;

	public:
		void addImage(Key const & key, sf::Image&& image) {
			auto pair = std::make_pair(key, std::move(image));
			images.push_back(pair);
		}
		
		sf::Image generate(std::size_t size) {
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
					clipping[pair.first] = sf::IntRect{
						static_cast<sf::Vector2i>(pos),
						static_cast<sf::Vector2i>(pair.second.getSize())
					};
				}
			}
			return std::move(atlas.generate());
		}

		sf::IntRect getClipping(Key const & key) const {
			return clipping.at(key);
		}
};

