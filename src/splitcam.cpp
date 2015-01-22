#include <cassert>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <SfmlExt/splitcam.hpp>

namespace sfext {

Camera::Camera(sf::Window const & window, std::size_t cam_id, std::size_t num_cams)
	: sf::View{}
	, resolution{window.getSize()}
	, id{cam_id}
	, num{num_cams} {
	assert(num_cams >= 1);
	assert(num_cams <= 4);
	assert(cam_id < num_cams);
	auto w = static_cast<float>(resolution.x);
	auto h = static_cast<float>(resolution.y);
	
	switch (num_cams) {
		// singleplayer camera (fullscreen)
		case 1:
			setSize({w, h});
			setViewport({0.f, 0.f, 1.f, 1.f});
			break;
		
		// splitscreen with 2 cameras (top / bottom)
		case 2:
			setSize({w, h / 2.f});
			setViewport({0.f, cam_id * 0.5f, 1.f, 0.5f});
			break;
		
		// splitscreen with 3 cameras (top, bottom left, bottom right)
		case 3:
			if (cam_id == 0u) {
				// top cam
				setSize({w, h / 2.f});
				setViewport({0.f, 0.f, 1.f, 0.5f});
			} else {
				// bottom cams
				setSize({w / 2.f, h / 2.f});
				setViewport({(cam_id-1u) * 0.5f, 0.5f, 0.5f, 0.5f});
			}
			break;
		
		// splitscreen with 4 cameras (top left, top right, bottom left, bottom right)
		case 4:
			setSize({w/2.f, h/2.f});
			if (cam_id <= 1) {
				// top cams
				setViewport({cam_id*0.5f, 0.f, 0.5f, 0.5f});
			} else {
				// bottom cams
				setViewport({(cam_id-2u)*0.5f, 0.5f, 0.5f, 0.5f});
			}
			break;
	}
	
	auto size = getSize();
	setCenter({size.x / 2.f, size.y / 2.f});
}

std::size_t Camera::getIndex() const {
	return id;
}

std::size_t Camera::getNumCams() const {
	return num;
}

sf::Vector2f Camera::getSize() const{
	auto& viewport = getViewport();	
	return {resolution.x * viewport.width, resolution.y * viewport.height};
}

}
