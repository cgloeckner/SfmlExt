#pragma once
#include <cmath>
#include <cassert>

namespace sfext {

template <GridMode M>
TilingIterator<M>::TilingIterator(sf::Vector2i const & start, sf::Vector2i const & range)
	: start{start}
	, range{range}
	, current{start}
	, count{0u} {
}

template <GridMode M>
sf::Vector2u TilingIterator<M>::operator*() const {
	return sf::Vector2u{current};
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
		if ((current.x + current.y) % 2 == 0u) {
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
Tiling<M>::Tiling(sf::Vector2f const & tile_size)
	: view{nullptr}
	, tile_size{tile_size} {
}

template <GridMode M>
void Tiling<M>::setView(sf::View const & cam) {
	view = &cam;
}

template <GridMode M>
void Tiling<M>::setTileSize(sf::Vector2f const & tsize) {
	tile_size = tsize;
}

template <GridMode M>
sf::Vector2f Tiling<M>::getTileSize() const {
	return tile_size;
}

template <GridMode M>
bool Tiling<M>::hasView() const {
	return (view != nullptr);
}

// specialization for orthogonal maps
template<>
inline sf::Vector2u Tiling<GridMode::Orthogonal>::getRange() const {
	auto size = view->getSize();
	sf::Vector2u range;
	
	// calculate range
	// size + 2 : because tile might be rendered centered (else: gap at bottom/right)
	range.x = static_cast<unsigned int>(std::ceil(size.x / tile_size.x)) + 2;
	range.y = static_cast<unsigned int>(std::ceil(size.y / tile_size.y)) + 2;
	
	return range;
}

// specialization for isometric (diamond) maps
template<>
inline sf::Vector2u Tiling<GridMode::IsoDiamond>::getRange() const {
	auto size = view->getSize();
	sf::Vector2u range;
	
	// calculate range
	// size + 2 : because tile might be rendered centered (else: gap at bottom/right)
	// size + 2 : because topleft starts 2 rows above (see topleft calc)
	// height x 2 : because of vertical zig-zag
	range.x =  static_cast<unsigned int>(std::ceil(size.x / tile_size.x)) + 2 + 2;
	range.y = (static_cast<unsigned int>(std::ceil(size.y / tile_size.y)) + 2 + 2) * 2;
	
	return range;
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
inline TilingIterator<GridMode::Orthogonal> Tiling<GridMode::Orthogonal>::begin() const {
	assert(view != nullptr);
	auto center = fromScreen(view->getCenter());
	sf::Vector2i topleft, range;
	
	range = sf::Vector2i{getRange()};
	
	// calculate topleft
	// x - width / 2 & y - height / 2 : go to topleft corner
	topleft.x = static_cast<unsigned int>(center.x - std::ceil(range.x / 2.f));
	topleft.y = static_cast<unsigned int>(center.y - std::ceil(range.y / 2.f));
	
	// create iterator
	return {topleft, range};
}

// specialization for orthogonal maps
template <>
inline TilingIterator<GridMode::Orthogonal> Tiling<GridMode::Orthogonal>::end() const {
	assert(view != nullptr);
	auto center = fromScreen(view->getCenter());
	sf::Vector2i bottomleft, range;
	
	range = sf::Vector2i{getRange()};
	
	// calculate topleft
	// x - width / 2 & y - height / 2 : go to topleft corner
	// y + 1 to end in next line (first invisible line)
	bottomleft.x = static_cast<unsigned int>(center.x - std::ceil(range.x / 2.f));
	bottomleft.y = static_cast<unsigned int>(center.y + std::ceil(range.y / 2.f)) + 1;
	
	// create iterator
	return {bottomleft, range};
}

// specialization for isometric (diamond) maps
template <>
inline TilingIterator<GridMode::IsoDiamond> Tiling<GridMode::IsoDiamond>::begin() const {
	assert(view != nullptr);
	auto center = fromScreen(view->getCenter());
	sf::Vector2i topleft, range;
	
	range = sf::Vector2i{getRange()};
	
	// calculate topleft
	// x - width : go to topleft corner
	// x - 2 & y - 2 : because tile might be rendered centered (else: gap at top), vertical zigzag -> x2
	topleft.x = static_cast<unsigned int>(center.x) - range.x - 2;
	topleft.y = static_cast<unsigned int>(center.y) - 2;
	
	// create iterator
	return {topleft, range};
}

// specialization for orthogonal maps
template <>
inline TilingIterator<GridMode::IsoDiamond> Tiling<GridMode::IsoDiamond>::end() const {
	assert(view != nullptr);
	auto center = fromScreen(view->getCenter());
	sf::Vector2i bottomleft, range;
	
	range = sf::Vector2i{getRange()};
	
	// calculate topleft
	// x - width + height / 2 & y + height / 2 : go to bottomleft corner
	bottomleft.x = static_cast<unsigned int>(center.x) - range.x + range.y / 2;
	bottomleft.y = static_cast<unsigned int>(center.y) + range.y / 2;
	
	// create iterator
	return {bottomleft, range};
}

} // ::sfext
