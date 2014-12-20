#pragma once
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <vector>

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

