#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Window.hpp>

namespace sfext {

/// Camera abstraction using sf::View
/**
 * A camera's view depends on the camera id and the maximum number of cameras.
 */
class Camera: public sf::View {
	private:
		/// resolution of the entire window
		sf::Vector2u resolution;
		/// camera id
		std::size_t const id;
		/// maximum number of cameras
		std::size_t const num;
		
	public:
		/// Create a camera for a window
		/**
		 * The camera will obtain a specific viewport of the window depending
		 * on the camera id and the maximum number of cameras.
		 * `num_cams` is currently limited to [1..4]. `cam_id` must be less
		 * than `num_cams`.
		 * @param window to create the camera for
		 * @param cam_id camera id
		 * @param num_cams maximum number of camera views
		 */
		Camera(sf::Window const & window, std::size_t cam_id, std::size_t num_cams);
		
		/// Get camera index
		/**
		 * @return camera index
		 */
		std::size_t getIndex() const;
		
		/// Get maximum number of cameras
		/**
		 * @return maximum number of cameras
		 */
		std::size_t getNumCams() const;
		
		/// Get size of the camera in window coordinates
		/**
		 * @return vector of size
		 */
		sf::Vector2f getSize() const;
};

}

