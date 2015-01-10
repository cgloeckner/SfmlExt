#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <SFML/Audio/Music.hpp>

namespace sfext {

/// Default loader for Sfml resources
template <typename Resource>
struct DefaultLoader {
	std::unique_ptr<Resource> operator()(std::string const & fname);
};

/// Unique ownership multi resource cache
/**
 * Holds unique ownership to multiple resource of multiple resource types.
 * Calling `get()` returns a non-const reference to the resource. It can also
 * be used for loading a resource, which is done on each cache miss.
 * Resources' ownership is always up to the cache.
 * So keep the cache alive as long as at least one resource is used outside
 * the cache.
 * The cache will never free resources until it's destroyed. To free all
 * resources, the cache itself has to be destroyed. Do so only if none of its
 * resources is used anywhere.
 */
class Cache {
	private:
		/// Generalized interface for all subcaches
		/**
		 * The signature is designed to be very general to all types of resources.
		 * Therefore the signature itself cannot obey RAII. To avoid exception-based
		 * memory violations or leaks, getting and setting must be implemented
		 * in a noexcept way.
		 */
		class ISubCache {
			public:
				virtual void* get(std::string const & fname) noexcept = 0;
				virtual bool set(std::string const & fname, void* ptr) noexcept = 0;
				virtual std::vector<std::string> getKeys() noexcept = 0;
		};
		
		/// A subcache dealing with a given resource type
		/**
		 * All resources are stored type-safe and using unique_ptr to obey RAII.
		 * @see `Cache::ISubCache`
		 */
		template <typename Resource>
		class SubCache: public ISubCache {
			private:
				std::unordered_map<std::string, std::unique_ptr<Resource>> cache;
			public:
				/// Get a resource identified by a key
				/**
				 * Guarantee's noexcept standard in each usecase.
				 * @param fname used as key to identify the resource
				 * @return non-owning pointer to the resource or nullptr
				 */
				void* get(std::string const & fname) noexcept override;
				
				/// Set a resource identified by a key
				/**
				 * Guarantee's noexcept standard in each usecase
				 * A owning raw pointer is given to this function. If returning
				 * true, the set operation succeeded and the resource is now
				 * owned by the subcache (so do not free it on your own!)
				 * This operation might fail if the given key is already in use.
				 * In this case it will return false and the ownership of the
				 * given resource remains at it's previous owner.
				 * @param fname used as key to identify the resource
				 * @param ptr owning raw pointer to the resource
				 * @return true if succeeded
				 */
				bool set(std::string const & fname, void* ptr) noexcept override;
				
				/// Get all keys
				/**
				 * @return array of keys
				 */
				std::vector<std::string> getKeys() noexcept override;
		};
		
		std::unordered_map<std::type_index, std::unique_ptr<ISubCache>> subcaches;
		
	public:
		/// Get a resource reference
		/**
		 * Returns a reference to the desired resource. The resource is identified
		 * by its filename. If the resource wasn't already loaded, the Loader
		 * template argument will be used to load it.
		 * A valid Loader must implement the following signature:
		 *		std::unique_ptr<Resource> operator()(std::string const &);
		 * This get operation might fail if a resource should be loaded but cannot be
		 * found.
		 * @throw std::ouf_of_range if resource cannot be loaded
		 * @param Resource type
		 * @param fname used as key to identify resource
		 * @return reference to resource
		 */
		template <typename Resource, typename Loader=DefaultLoader<Resource>>
		Resource& get(std::string const & fname);
		
		/// Determines whether the described resource exists inside the cache or not
		/**
		 * @param Resource type
		 * @param fname key to identify the resource
		 * @return true if resource was found inside cache
		 */
		template <typename Resource>
		bool has(std::string const & fname);
		
		/// Return all existing keys of a given resource type
		/**
		 * @param Resource type
		 * @return array of keys
		 */
		template <typename Resource>
		std::vector<std::string> getKeys() const;
};

} // ::sfext

// ----------------------------------------------------------------------------
// implementation

namespace sfext {

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
std::unique_ptr<sf::Music> DefaultLoader<sf::Music>::operator()(std::string const & fname) {
	std::unique_ptr<sf::Music> resource{new sf::Music{}};
	if (!resource->openFromFile(fname)) {
		return nullptr;
	}
	return resource;
}

// ----------------------------------------------------------------------------

template <typename Resource>
void* Cache::SubCache<Resource>::get(std::string const & fname) noexcept {
	auto i = cache.find(fname);
	if (i == cache.end()) {
		// resource not found
		return nullptr;
	}
	return static_cast<void*>(i->second.get());
}

template <typename Resource>
bool Cache::SubCache<Resource>::set(std::string const & fname, void* ptr) noexcept {
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
std::vector<std::string> Cache::SubCache<Resource>::getKeys() noexcept {
	std::vector<std::string> keys;
	keys.resize(cache.size());
	std::size_t i{0u};
	for (auto const & pair: cache) {
		keys[i++] = pair.first;
	}
	return keys;
}

template <typename Resource, typename Loader>
Resource& Cache::get(std::string const & fname) {
	// search subcache
	auto index = std::type_index(typeid(Resource));
	auto i = subcaches.find(index);
	if (i == subcaches.end()) {
		// create subcache
		std::unique_ptr<ISubCache> sub{new SubCache<Resource>{}};
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
bool Cache::has(std::string const & fname) {
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
