#pragma once
#include <stdexcept>
#include <cassert>
#include <SFML/Audio/Music.hpp>

namespace sfext {
namespace details {

template <typename Resource>
void* SubCache<Resource>::get(std::string const & fname) noexcept {
	auto i = cache.find(fname);
	if (i == cache.end()) {
		// resource not found
		return nullptr;
	}
	return static_cast<void*>(i->second.get());
}

template <typename Resource>
bool SubCache<Resource>::set(std::string const & fname, void* ptr) noexcept {
	auto i = cache.find(fname);
	if (i != cache.end()) {
		// fname already used
		return false;
	}
	auto casted_ptr = static_cast<Resource*>(ptr);
	std::unique_ptr<Resource> resource{casted_ptr};
	cache[fname] = std::move(resource);
	return true;
}

template <typename Resource>
std::vector<std::string> SubCache<Resource>::getKeys() noexcept {
	std::vector<std::string> keys;
	keys.resize(cache.size());
	std::size_t i{0u};
	for (auto const & pair: cache) {
		keys[i++] = pair.first;
	}
	return keys;
}

} // ::details


template <typename Resource>
std::unique_ptr<Resource> DefaultLoader<Resource>::operator()(std::string const & fname) {
	std::unique_ptr<Resource> resource{new Resource{}};
	if (!resource->loadFromFile(fname)) {
		return nullptr;
	}
	return resource;
}

// loader specialization for sf::Music
template <>
inline std::unique_ptr<sf::Music> DefaultLoader<sf::Music>::operator()(std::string const & fname) {
	std::unique_ptr<sf::Music> resource{new sf::Music{}};
	if (!resource->openFromFile(fname)) {
		return nullptr;
	}
	return resource;
}

// ---------------------------------------------------------------------------

template <typename Resource, typename Loader>
Resource& Cache::get(std::string const & fname) {
	// search subcache
	auto index = std::type_index(typeid(Resource));
	auto i = subcaches.find(index);
	if (i == subcaches.end()) {
		// create subcache
		std::unique_ptr<details::ISubCache> sub{new details::SubCache<Resource>{}};
		auto result = subcaches.emplace(std::make_pair(index, std::move(sub)));
		assert(result.second);
		i = result.first;
	}
	
	// search resource
	auto ptr = static_cast<Resource*>(i->second->get(fname));
	if (ptr != nullptr) {
		// resource found
		return *ptr;
	}
	
	// handle cache miss
	Loader load;
	std::unique_ptr<Resource> res = load(fname); // note: forces right signature
	if (res == nullptr) {
		// loading failed
		throw std::out_of_range(fname);
	}
	
	// write to subcache
	ptr = res.release();
	auto raw = static_cast<void*>(ptr);
	bool success = i->second->set(fname, raw);
	assert(success); // note that !success will cause memory leak in debug mode
	return *ptr;
}

template <typename Resource>
bool Cache::has(std::string const & fname) const {
	// search subcache
	auto index = std::type_index(typeid(Resource));
	auto i = subcaches.find(index);
	if (i == subcaches.end()) {
		return false;
	}
	
	// search resource
	auto ptr = i->second->get(fname);
	return (ptr != nullptr);
}

template <typename Resource>
std::vector<std::string> Cache::getKeys() const {
	std::vector<std::string> keys;
	
	// search subcache
	auto index = std::type_index(typeid(Resource));
	auto i = subcaches.find(index);
	if (i != subcaches.end()) {
		keys = i->second->getKeys();
	}
	
	return keys;
}

} // ::sfext
