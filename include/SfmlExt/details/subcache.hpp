#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace sfext {
namespace details {

/// Interface for subcaches
class ISubCache {
	public:
		virtual void* get(std::string const & fname) noexcept = 0;
		virtual bool set(std::string const & fname, void* ptr) noexcept = 0;
		virtual std::vector<std::string> getKeys() noexcept = 0;
};

/// A subcache dealing with a given resource type
/**
 * All resources are stored type-safe and using unique_ptr to obey RAII.
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

} // ::details
} // ::sfext
