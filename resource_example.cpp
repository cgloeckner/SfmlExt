#include <cassert>
#include <SFML/Graphics.hpp>

#include "resource_cache.hpp"
#include "resource_archive.hpp"

/// texture loader
struct TextureLoader {
	// interface used by resource_archive
	std::unique_ptr<sf::Texture> operator()(char* buffer, std::size_t bytes) const {
		std::unique_ptr<sf::Texture> res{new sf::Texture{}};
		if (!res->loadFromMemory(buffer, bytes)) {
			return nullptr;
		}
		return res;
	}

	// interface used by resource_cache
	std::unique_ptr<sf::Texture> operator()(std::string const & filename) const {
		std::unique_ptr<sf::Texture> res{new sf::Texture{}};
		if (!res->loadFromFile(filename)) {
			return nullptr;
		}
		return res;
	}
};

// typedef my texture archive writer/reader with dummy packing (does nothing)
// to apply e.g. zipping, you'll need to implement a ZipPacker for the archive
// writer and a ZipUnpacker for the archive reader
using TexArchiveWriter = ResourceArchiveWriter<sf::Texture, DummyPacking>;
using TexArchiveReader = ResourceArchiveReader<sf::Texture, TextureLoader, DummyPacking>;

// typedef my texture caches using my texture loader
using SimpleTexCache = StandaloneCache<sf::Texture, TextureLoader>;
using ExtendedTexCache = ResourceCache<sf::Texture, TextureLoader, TexArchiveReader>;

int main() {
	// list of some resources used in this example
	std::vector<std::string> resources = {
		"wraith-s-attack-1.png", "wraith-s-attack-2.png",
		"wraith-s-attack-3.png", "wraith-s-attack-4.png",
		"wraith-s-attack-5.png"
	};
	
	// ------------------------------------------------------------------------
	// create a standalone texture cache and fetch a texture
	// resource will be loaded from disk directly
	SimpleTexCache cache;
	auto& my_tex = cache.get(resources.front());

	// ------------------------------------------------------------------------
	// write all resources to a binary archive called "out.bin"
	TexArchiveWriter writer;
	writer.save(resources, "out.bin");
	
	// ------------------------------------------------------------------------
	// create an extendable texture cache
	ExtendedTexCache ext_cache;
	// load that archive
	TexArchiveReader reader;
	assert(reader.open("out.bin"));
	// and extend the cache with the archive
	ext_cache.extend(reader);
	// resource will be loaded from the archive
	auto& another_tex = ext_cache.get(resources.back());
	
}

