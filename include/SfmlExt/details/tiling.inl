#pragma once
#include <cmath>
#include <cassert>

namespace sfext {

template <GridMode M>
TilingIterator<M>::TilingIterator(sf::Vector2i const & start, sf::Vector2i const & range)
	: current{start}
	, start{start}
	, range{range}
	, count{0u} {
}

template <GridMode M>
sf::Vector2i const & TilingIterator<M>::operator*() const {
	return current;
}

template <GridMode M>
bool TilingIterator<M>::operator!=(TilingIterator<M> const & other) const {
	return (current != other.current);
}

// specialization for orthogonal grids
template <>
inline void TilingIterator<GridMode::Orthogonal>::operator++() {
	// step through (screen) row's cells
	++current.x;
	if (current.x > start.x + range.x) {
		// go to next (screen) row
		current.x = start.x;
		//++current.screen_row;
		++current.y;
	}
}

// specialization for isometric diamond grids
template <>
inline void TilingIterator<GridMode::IsoDiamond>::operator++() {
	// step through (screen) row's cells
	--current.y;
	++current.x;
	++count;
	if (count > range.x) {
		// go to next screen row --> zigzag
		if ((current.x + current.y) % 2 == 0) {
			++start.y;
		} else {
			++start.x;
		}
		current = start;
		count = 0u;
	}
}

template <GridMode M>
sf::Vector2i TilingIterator<M>::getRange() const {
	return range;
}

// ----------------------------------------------------------------------------

template <GridMode M>
Tiling<M>::Tiling(sf::Vector2u const & map_size, sf::Vector2f const & tile_size)
	: camera{nullptr}
	, map_size{map_size}
	, tile_size{tile_size} {
}

template <GridMode M>
void Tiling<M>::setCamera(sf::View const & camera) {
	this->camera = &camera;
}

template <GridMode M>
bool Tiling<M>::hasCamera() const {
	return (camera != nullptr);
}

template <GridMode M>
bool Tiling<M>::isTilePos(sf::Vector2i const & tile_pos) {
	return (tile_pos.x >= 0 && tile_pos.x < map_size.x && 
		tile_pos.y >= 0 && tile_pos.y < map_size.y);
}

// specialization for orthogonal maps
template<>
inline sf::Vector2f Tiling<GridMode::Orthogonal>::toScreen(sf::Vector2f const & world_pos) const {
	return {world_pos.x * tile_size.x, world_pos.y * tile_size.y};
}

// specialization for isometric (diamond) maps
template<>
inline sf::Vector2f Tiling<GridMode::IsoDiamond>::toScreen(sf::Vector2f const & world_pos) const {
	return {
		(world_pos.x - world_pos.y) * tile_size.x / 2.f,
		(world_pos.x + world_pos.y) * tile_size.y / 2.f
	};
}

// specialization for orthogonal maps
template<>
inline sf::Vector2f Tiling<GridMode::Orthogonal>::fromScreen(sf::Vector2f const & screen_pos) const {
	return {screen_pos.x / tile_size.x, screen_pos.y / tile_size.y};
}

// specialization for isometric (diamond) maps
template<>
inline sf::Vector2f Tiling<GridMode::IsoDiamond>::fromScreen(sf::Vector2f const & screen_pos) const {
	return {
		(screen_pos.x / (tile_size.x / 2.f) + screen_pos.y / (tile_size.y / 2.f)) / 2.f,
		(screen_pos.y / (tile_size.y / 2.f) - screen_pos.x / (tile_size.x / 2.f)) / 2.f
	};
}

// specialization for orthogonal maps
template <>
inline std::vector<sf::Vector2i> Tiling<GridMode::Orthogonal>::getNeighbors(sf::Vector2i const & tile_pos) const {
	std::vector<sf::Vector2i> next;
	sf::Vector2i pos;
	next.reserve(8);
	for (pos.x = -1; pos.x <= 1; ++pos.x) {
		for (pos.y = -1; pos.y <= 1; ++pos.y) {
			if (pos.x == 0 && pos.y == 0) {
				continue;
			}
			next.push_back(pos);
		}
	}
	return next;
}

// specialization for isometric diamond maps
template <>
inline std::vector<sf::Vector2i> Tiling<GridMode::IsoDiamond>::getNeighbors(sf::Vector2i const & tile_pos) const {
	// note that, isometric diamond is orthogonal in it's model
	std::vector<sf::Vector2i> next;
	sf::Vector2i pos;
	next.reserve(8);
	for (pos.x = -1; pos.x <= 1; ++pos.x) {
		for (pos.y = -1; pos.y <= 1; ++pos.y) {
			if (pos.x == 0 && pos.y == 0) {
				continue;
			}
			next.push_back(pos);
		}
	}
	return next;
}

// specialization for orthogonal maps
template <>
inline TilingIterator<GridMode::Orthogonal> Tiling<GridMode::Orthogonal>::begin() const {
	assert(camera != nullptr);
	auto center = fromScreen(camera->getCenter());
	auto size = camera->getSize();
	sf::Vector2i topleft, range;
	
	// calculate range
	// size + 2 : because tile might be rendered centered (else: gap at bottom/right)
	range.x = static_cast<int>(std::ceil(size.x / tile_size.x)) + 2;
	range.y = static_cast<int>(std::ceil(size.y / tile_size.y)) + 2;
	// calculate topleft
	// x - width / 2 & y - height / 2 : go to topleft corner
	topleft.x = static_cast<int>(center.x - std::ceil(range.x / 2.f));
	topleft.y = static_cast<int>(center.y - std::ceil(range.y / 2.f));
	
	// create iterator
	return {topleft, range};
}

// specialization for orthogonal maps
template <>
inline TilingIterator<GridMode::Orthogonal> Tiling<GridMode::Orthogonal>::end() const {
	assert(camera != nullptr);
	auto center = fromScreen(camera->getCenter());
	auto size = camera->getSize();
	sf::Vector2i bottomleft, range;
	
	// calculate range
	// size + 2 : because tile might be rendered centered (else: gap at bottom/right)
	range.x = static_cast<int>(std::ceil(size.x / tile_size.x)) + 2;
	range.y = static_cast<int>(std::ceil(size.y / tile_size.y)) + 2;
	// calculate topleft
	// x - width / 2 & y + height / 2 : go to bottom left corner
	// y + 1 : next line (1st line which isn't iterated anymore)
	bottomleft.x = static_cast<int>(center.x - std::ceil(range.x / 2.f));
	bottomleft.y = static_cast<int>(center.y + std::ceil(range.y / 2.f)) + 1;
	
	// create iterator
	return {bottomleft, range};
}

// specialization for isometric (diamond) maps
template <>
inline TilingIterator<GridMode::IsoDiamond> Tiling<GridMode::IsoDiamond>::begin() const {
	assert(camera != nullptr);
	auto center = fromScreen(camera->getCenter());
	auto size = camera->getSize();
	sf::Vector2i topleft, range;
	
	// calculate range
	// size + 2 : because tile might be rendered centered (else: gap at bottom/right)
	// size + 2 : because topleft starts 2 rows above (see topleft calc)
	// height x 2 : because of vertical zig-zag
	range.x =  static_cast<int>(std::ceil(size.x / tile_size.x)) + 2 + 2;
	range.y = (static_cast<int>(std::ceil(size.y / tile_size.y)) + 2 + 2) * 2;
	// calculate topleft
	// x - width : go to topleft corner
	// x - 2 & y - 2 : because tile might be rendered centered (else: gap at top), vertical zigzag -> x2
	topleft.x = static_cast<int>(center.x) - range.x - 2;
	topleft.y = static_cast<int>(center.y) - 2;
	
	// create iterator
	return {topleft, range};
}

// specialization for orthogonal maps
template <>
inline TilingIterator<GridMode::IsoDiamond> Tiling<GridMode::IsoDiamond>::end() const {
	assert(camera != nullptr);
	auto center = fromScreen(camera->getCenter());
	auto size = camera->getSize();
	sf::Vector2i bottomleft, range;
	
	// calculate range
	// size + 2 : because tile might be rendered centered (else: gap at bottom/right)
	// size + 2 : because topleft starts 2 rows above (see topleft calc)
	// height x 2 : because of vertical zig-zag
	range.x =  static_cast<int>(std::ceil(size.x / tile_size.x)) + 2 + 2;
	range.y = (static_cast<int>(std::ceil(size.y / tile_size.y)) + 2 + 2) * 2;
	// calculate topleft
	// x - width + height / 2 & y + height / 2 : go to bottomleft corner
	bottomleft.x = static_cast<int>(center.x) - range.x + range.y / 2;
	bottomleft.y = static_cast<int>(center.y) + range.y / 2;
	
	// create iterator
	return {bottomleft, range};
}

} // ::sfext
