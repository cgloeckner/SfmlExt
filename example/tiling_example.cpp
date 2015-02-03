#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <SfmlExt/tiling.hpp>

int main() {
	// Create tilings
	sfext::Tiling<sfext::GridMode::Orthogonal> ortho_tiling{{64.f, 64.f}};
	sfext::Tiling<sfext::GridMode::IsoDiamond> iso_tiling{{64.f, 32.f}};

	// Prepare window and camera
	int const width = 640;
	int const height = 480;
	sf::RenderWindow window{{width, height}, "Tiling example"};
	sf::View ortho_cam, iso_cam;
	
	ortho_cam.setSize({width, height/2});
	ortho_cam.setViewport({0.f, 0.f, 1.f, 0.5f});
	ortho_tiling.setView(ortho_cam);
	
	iso_cam.setSize({width, height/2});
	iso_cam.setViewport({0.f, 0.5f, 1.f, 0.5f});
	iso_tiling.setView(iso_cam);
	
	// Prepare tileset
	sf::Texture tileset;
	sf::Sprite tile;
	tileset.loadFromFile("data/tileset.png");
	tile.setTexture(tileset);
	
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) { window.close(); }
		}
		
		// move ortho camera
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { ortho_cam.move( 0.f, -4.f); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { ortho_cam.move( 0.f,  4.f); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { ortho_cam.move(-4.f,  0.f); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { ortho_cam.move( 4.f,  0.f); }
		
		// move iso camera
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))		{ iso_cam.move( 0.f, -4.f); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))		{ iso_cam.move( 0.f,  4.f); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))		{ iso_cam.move(-4.f,  0.f); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))	{ iso_cam.move( 4.f,  0.f); }
		
		window.clear(sf::Color::Black);
		// render ortho map
		window.setView(ortho_cam);
		tile.setTextureRect({0, 0, 64, 64});
		tile.setOrigin({32.f, 32.f});
		for (auto const & tile_pos: ortho_tiling) {
			if (tile_pos.x < 32 && tile_pos.y < 32) {
				auto spos = ortho_tiling.toScreen(static_cast<sf::Vector2f>(tile_pos));
				// render tile
				tile.setPosition(spos);
				window.draw(tile);
			}
		}
		
		// render iso diamondmap
		window.setView(iso_cam);
		tile.setTextureRect({64, 0, 64, 32});
		tile.setOrigin({32.f, 16.f});
		for (auto const & tile_pos: iso_tiling) {
			if (tile_pos.x < 32 && tile_pos.y < 32) {
				auto spos = iso_tiling.toScreen(static_cast<sf::Vector2f>(tile_pos));
				// render tile
				tile.setPosition(spos);
				window.draw(tile);
			}
		}
		
		window.display();
	}
}
