#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <vector>

#include <SfmlExt/details/subcache.hpp>

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
		std::unordered_map<std::type_index, std::unique_ptr<details::ISubCache>> subcaches;
		
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

// include implementation details
#include <SfmlExt/details/cache.inl>
