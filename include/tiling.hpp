#pragma once
#include <vector>
#include <cassert>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/View.hpp>

namespace sfext {

/// Supported grid modes
enum class GridMode {
	Orthogonal, IsoDiamond
	// later: IsoStaggered, Hexagonal
};

/// Provides several tiling functionality depending on the actual grid mode
/**
 * The given GridMode M determines the actual tiling approach. See `GridMode`
 * for a list of all supported modes.
 * Each tiling instance can perform operations of tiling coordinates according
 * to the given mode. Some operations require a camera to be set.
 */
template <GridMode M>
class Tiling {
	private:
		/// Used for iteration over visible area
		class Iterator {
			private:
				sf::Vector2i current, start, range;
				unsigned int count; // counting colums per row (iso diamond only)
				
			public:
				/// Create a new iterator with a start and an iteration rage.
				/**
				 * @param start Tile position used as topleft starting position
				 * @param range Number of rows and columns to iterate over
				 */
				Iterator(sf::Vector2i const & start, sf::Vector2i const & range);
				
				/// Returns the current tile position
				/**
				 * @return Tile position which the iterator is indicating at
				 */
				sf::Vector2i const & operator*() const;
				
				/// Used to compare two iterators
				/**
				 * Two iterators satisfy the != relation if their current
				 * positions do not equal
				 * @param other Iterator to compare with
				 * @return true if positions do not equal
				 */
				bool operator!=(Iterator const & other) const;
				
				/// Step to next position
				/**
				 * The actual implementation depends on the `GridMode` given
				 * to `Tiling`
				 */
				void operator++();
				
				/// Returns the number of columns and rows to iterate over
				/**
				 * @return range (as given in ctor)
				 */
				sf::Vector2i getRange() const;
		};
		
		/// Current camera view
		sf::View const * camera;
		/// Number of tiles per map dimension
		sf::Vector2u const & map_size;
		/// Number of pixels per tile graphics' dimension
		sf::Vector2f const & tile_size;
		
	public:
		/// Create a new tiling for a fixed map size and tile size
		/**
		 * @param map_size number of tiles per map dimension
		 * @param tile_size number of pixels per tile graphics' dimension
		 */
		Tiling(sf::Vector2u const & map_size, sf::Vector2f const & tile_size);
		
		/// Set the current camera's view
		/**
		 * Some operations require a camera to be set. This method is used to
		 * set a sf::View as a camera. The camera can be changed during the
		 * runtime of the tiling object to enable e.g. splitscreen rendering
		 * with only one instance of Tiling.
		 * @param camera The view which describes the camera
		 */
		void setCamera(sf::View const & camera);
		
		/// Returns whether the tiling uses a camera, yet
		/**
		 * Some operations require a camera to be set. This method is used to
		 * check whether a camera was set.
		 * @return true if a camera is set
		 */
		bool hasCamera() const;
		
		/// Checks whether a tile position is valid referring to map size
		/**
		 * A position is valid if it is located within the map boundary
		 * @param tile_pos Tile position to check whether it is valid
		 * @return true if tile position is valid
		 */
		bool isTilePos(sf::Vector2i const & tile_pos);
		
		/// Converts a world position to a screen position
		/**
		 * A world position is assumed to be in the same scale as a tile
		 * position. Additionally a world position is float-based, so
		 * <1.3f, 2.6f> is located at tile <1,2> with inner-tile offsets
		 * 0.3f and 0.6f. This transformation depends on the actual `GridMode`.
		 * @param world_pos Position in world-scale to convert to screen pos
		 * @return Result of the position transformation.
		 */
		sf::Vector2f toScreen(sf::Vector2f const & world_pos) const;
		
		/// Converts a screen position to a world position
		/**
		 * A world position is assumed to be in the same scale as a tile
		 * position. Additionally a world position is float-based, so
		 * <1.3f, 2.6f> is located at tile <1,2> with inner-tile offsets
		 * 0.3f and 0.6f. This transformation depends on the actual `GridMode`.
		 * @param screen_pos Position in screen-scale to convert to world pos
		 * @return Result of the position transformation.
		 */
		sf::Vector2f fromScreen(sf::Vector2f const & screen_pos) const;
		
		/// Returns an array of tile positions next to the given position
		/**
		 * The returned tile position are not automatically valid! Neigher the
		 * given tile position is checked to be valid. You might need to check
		 * it using `isTilePos()`. The calculation of neighbors depends on the
		 * actual `GridMode`.
		 * @param tile_pos Tile position used as origin
		 * @return array of tile positions located next to tile_pos
		 */
		std::vector<sf::Vector2i> getNeighbors(sf::Vector2i const & tile_pos) const;
		
		/// Returns iterator to first tile position in range
		/**
		 * This operation requires a camera to be set!
		 * @return Iterator to first tile position in range
		 */
		Iterator begin() const;
		
		/// Returns iterator to first tile position out of range
		/**
		 * This operation requires a camera to be set!
		 * @return Iterator to first tile position out of range
		 */
		Iterator end() const;
};

} // ::sfext

// ----------------------------------------------------------------------------
// implementation

namespace sfext {

template <GridMode M>
Tiling<M>::Iterator::Iterator(sf::Vector2i const & start, sf::Vector2i const & range)
	: current{start}
	, start{start}
	, range{range}
	, count{0u} {
}

template <GridMode M>
sf::Vector2i const & Tiling<M>::Iterator::operator*() const {
	return current;
}

template <GridMode M>
bool Tiling<M>::Iterator::operator!=(Tiling<M>::Iterator const & other) const {
	return (current != other.current);
}

// specialization for orthogonal grids
template <>
void Tiling<GridMode::Orthogonal>::Iterator::operator++() {
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
void Tiling<GridMode::IsoDiamond>::Iterator::operator++() {
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
sf::Vector2i Tiling<M>::Iterator::getRange() const {
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
sf::Vector2f Tiling<GridMode::Orthogonal>::toScreen(sf::Vector2f const & world_pos) const {
	return {world_pos.x * tile_size.x, world_pos.y * tile_size.y};
}

// specialization for isometric (diamond) maps
template<>
sf::Vector2f Tiling<GridMode::IsoDiamond>::toScreen(sf::Vector2f const & world_pos) const {
	return {
		(world_pos.x - world_pos.y) * tile_size.x / 2.f,
		(world_pos.x + world_pos.y) * tile_size.y / 2.f
	};
}

// specialization for orthogonal maps
template<>
sf::Vector2f Tiling<GridMode::Orthogonal>::fromScreen(sf::Vector2f const & screen_pos) const {
	return {screen_pos.x / tile_size.x, screen_pos.y / tile_size.y};
}

// specialization for isometric (diamond) maps
template<>
sf::Vector2f Tiling<GridMode::IsoDiamond>::fromScreen(sf::Vector2f const & screen_pos) const {
	return {
		(screen_pos.x / (tile_size.x / 2.f) + screen_pos.y / (tile_size.y / 2.f)) / 2.f,
		(screen_pos.y / (tile_size.y / 2.f) - screen_pos.x / (tile_size.x / 2.f)) / 2.f
	};
}

// specialization for orthogonal maps
template <>
std::vector<sf::Vector2i> Tiling<GridMode::Orthogonal>::getNeighbors(sf::Vector2i const & tile_pos) const {
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
std::vector<sf::Vector2i> Tiling<GridMode::IsoDiamond>::getNeighbors(sf::Vector2i const & tile_pos) const {
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
typename Tiling<GridMode::Orthogonal>::Iterator Tiling<GridMode::Orthogonal>::begin() const {
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
typename Tiling<GridMode::Orthogonal>::Iterator Tiling<GridMode::Orthogonal>::end() const {
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
typename Tiling<GridMode::IsoDiamond>::Iterator Tiling<GridMode::IsoDiamond>::begin() const {
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
typename Tiling<GridMode::IsoDiamond>::Iterator Tiling<GridMode::IsoDiamond>::end() const {
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
