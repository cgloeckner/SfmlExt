#include <cassert>
#include <algorithm>
#include <SFML/Graphics/Texture.hpp>

#include <cache.hpp>

void test_cache_existingResource() {
	sfext::Cache cache;
	bool error{false};
	assert(!cache.has<sf::Texture>("data/attack0.png"));
	try {
		cache.get<sf::Texture>("data/attack0.png");
	} catch (std::out_of_range const &) {
		error = true;
	}
	assert(!error);
	assert(cache.has<sf::Texture>("data/attack0.png"));
}

void test_cache_missingResource() {
	sfext::Cache cache;
	bool error{false};
	try {
		cache.get<sf::Texture>("data/foo.png");
	} catch (std::out_of_range const &) {
		error = true;
	}
	assert(error);
}

void test_cache_cachingOnce() {
	sfext::Cache cache;
	bool error{false};
	try {
		auto& a = cache.get<sf::Texture>("data/attack0.png");
		auto& b = cache.get<sf::Texture>("data/attack0.png");
		assert(&a == &b); // pointing to same resource
	} catch (std::out_of_range const &) {
		error = true;
	}
	assert(!error);
}

void test_cache_getKeys() {
	sfext::Cache cache;
	bool error{false};
	try {
		for (std::size_t i = 0u; i < 5; ++i) {
			cache.get<sf::Texture>("data/attack" + std::to_string(i) + ".png");
		}
	} catch (std::out_of_range const &) {
		error = true;
	}
	assert(!error);
	
	auto keys = cache.getKeys<sf::Texture>();
	assert(keys.size() == 5);
	for (std::size_t i = 0u; i < 5; ++i) {
		std::string fname{"data/attack" + std::to_string(i) + ".png"};
		auto j = std::find_if(begin(keys), end(keys), [&](std::string const & s) {
			return (s == fname);
		});
		assert(j != keys.end());
	}
}
