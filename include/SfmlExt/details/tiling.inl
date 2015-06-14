#pragma once
#include <cmath>

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
	if (current.x >= start.x + range.x) {
		// go to next (screen) row
		current.x = start.x;
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
	if (count >= range.x) {
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
	: view{}
	, tile_size{tile_size}
	, padding{0u, 0u} {
}

template <GridMode M>
void Tiling<M>::setView(sf::View const & cam) {
	view = cam;
}

template <GridMode M>
void Tiling<M>::setTileSize(sf::Vector2f const & tsize) {
	tile_size = tsize;
}

template <GridMode M>
void Tiling<M>::setPadding(sf::Vector2u const & pad) {
	padding = pad;
}

template <GridMode M>
sf::Vector2f Tiling<M>::getTileSize() const {
	return tile_size;
}

template <GridMode M>
sf::View Tiling<M>::getView() const {
	return view;
}

// specialization for orthogonal maps
template<>
inline sf::Vector2u Tiling<GridMode::Orthogonal>::getRange() const {
	auto size = view.getSize();
	sf::Vector2u range;
	
	// calculate range
	// size + 2 : because tile might be rendered centered (else: gap at bottom/right)
	range.x = static_cast<unsigned int>(std::ceil(size.x / tile_size.x)) + 2;
	range.y = static_cast<unsigned int>(std::ceil(size.y / tile_size.y)) + 2;
	
	// apply padding
	range += padding * 2u;
	
	return range;
}

// specialization for isometric (diamond) maps
template<>
inline sf::Vector2u Tiling<GridMode::IsoDiamond>::getRange() const {
	auto size = view.getSize();
	sf::Vector2u range;
	
	// calculate range
	// size + 2 : because tile might be rendered centered (else: gap at bottom/right)
	// size + 2 : because topleft starts 2 rows above (see topleft calc)
	// height x 2 : because of vertical zig-zag
	range.x =  static_cast<unsigned int>(std::ceil(size.x / tile_size.x)) + 2 + 2;
	range.y = (static_cast<unsigned int>(std::ceil(size.y / tile_size.y)) + 2 + 2) * 2;
	
	// apply padding
	range += padding * 2u;
	
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

// specialization for orthogonal
template <>
inline sf::Vector2i Tiling<GridMode::Orthogonal>::getTopleft() const {
	sf::Vector2i topleft;
	auto center = sf::Vector2i{fromScreen(view.getCenter())};
	auto range = sf::Vector2i{getRange()};
	
	// calculate topleft
	// x - width / 2 & y - height / 2 : go to topleft corner
	topleft.x = static_cast<unsigned int>(center.x - std::ceil(range.x / 2.f));
	topleft.y = static_cast<unsigned int>(center.y - std::ceil(range.y / 2.f));
	
	return topleft;
}

// specialization for isometric (diamond) maps
template <>
inline sf::Vector2i Tiling<GridMode::IsoDiamond>::getTopleft() const {
	// Use view's center to determine topleft position
	auto pos = view.getCenter() - view.getSize() / 2.f;
	auto topleft = sf::Vector2i{fromScreen(pos)};
	topleft -= sf::Vector2i{padding};
	return topleft;
}

// specialization for orthogonal maps
template<>
inline sf::Vector2i Tiling<GridMode::Orthogonal>::getBottomleft() const {
	auto range = sf::Vector2i{getRange()};
	
	// calculate bottomleft position (note: range.y == 1 causes ++y)
	auto pos = getTopleft();
	pos.y += range.y;
	
	return pos;
}

// specialization for isometric (diamond) maps
template <>
inline sf::Vector2i Tiling<GridMode::IsoDiamond>::getBottomleft() const {
	auto range = sf::Vector2i{getRange()};
	
	// calculate bottomleft position
	// note: zig-zack increases both coordinates by half height
	auto pos = getTopleft();
	pos.x += range.y / 2;
	pos.y += range.y / 2;
	
	return pos;
}

// ---------------------------------------------------------------------------

template <GridMode M>
inline TilingIterator<M> begin(Tiling<M> const & tiling) {
	return {tiling.getTopleft(), sf::Vector2i{tiling.getRange()}};
}

template <GridMode M>
inline TilingIterator<M> end(Tiling<M> const & tiling) {
	return {tiling.getBottomleft(), sf::Vector2i{tiling.getRange()}};
}

} // ::sfext
