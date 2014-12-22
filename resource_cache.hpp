#pragma once
#include <unordered_map>
#include <memory>
#include <vector>

/// Holding resources with unique ownership.
/**
 *	Each resource is keyed by it's filename. Accessing a resources DOES NOT
 *	imply ownership semantics. Therefore no resource is allowed to be released
 *	until ALL resource observers went out of scope.
 *	Therefore no copy assignment is implemented, because this would imply to
 *	clear the assignment target's cache. But copy construction is possible
 *	by deep-copying all resources, which implies Resource to be copy
 *	constructable.
 *	A loader needs to satisfy the following operator() syntax to be valid:
 *		std::unique_ptr<Resource> operator()(std::string const & filename)
 *	Note that Resource needs to be replaced with the actual template type.
 */
template <typename Resource, typename Loader, typename Archive>
class ResourceCache {
	private:
		using Cache = std::unordered_map<std::string, std::unique_ptr<Resource>>;
		using CacheIterator = typename Cache::const_iterator;

		mutable Cache cache;
		Loader loader;
		std::vector<Archive const *> archives;

	public:
		ResourceCache()
			: cache{}
			, loader{} {
		}

		virtual ~ResourceCache() {
		}

		// copy construction by deep copying all resources
		ResourceCache(ResourceCache const & other)
			: cache{} {
			loader = other.loader;
			// deep copy all resources
			for (auto const & entry: other.cache) {
				cache[entry.first] = std::unique_ptr<Resource>{new Resource{
					*entry.second.get()
				}};
			}
		}

		// move construction needs to be re-implemented
		ResourceCache(ResourceCache&& other)
			: cache{std::move(other.cache)} {
			loader = std::move(other.loader);
		}

		// copy assignment would force to clear
		ResourceCache& operator=(ResourceCache const & other) = delete;

		void extend(Archive const & archive) {
			archives.push_back(&archive);
		}

		std::size_t size() const {
			return cache.size();
		}

		bool has(std::string const & filename) const {
			return cache.find(filename) != cache.end();
		}

		Resource& get(std::string const & filename) const {
			// search cache
			auto i = cache.find(filename);
			if (i != cache.end()) {
				// return reference
				return *i->second.get();
			}
			// search archives
			std::unique_ptr<Resource> resource{nullptr};
			for (auto const & ar: archives) {
				auto resource = ar->load(filename);
				if (resource != nullptr) {
					break;
				}
			}
			if (resource == nullptr) {
				// try direct loading
				resource = loader(filename);
			}
			if (resource != nullptr) {
				// save to cache and return reference
				auto ptr = resource.get();
				cache[filename] = std::move(resource);
				return *ptr;
			}
			// resource is unavailable
			throw std::out_of_range(filename);
		}
};

template <typename Resource>
struct DummyArchive {
	std::unique_ptr<Resource> load(std::string const & key) const {
		return nullptr;
	}
};

template <typename Resource, typename Loader>
using StandaloneCache = ResourceCache<Resource, Loader, DummyArchive<Resource>>;

