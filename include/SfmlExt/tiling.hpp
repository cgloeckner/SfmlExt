#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/View.hpp>

namespace sfext {

/// Supported grid modes
enum class GridMode {
	Orthogonal, IsoDiamond
	// later: IsoStaggered, Hexagonal
};

/// Used for iteration over visible area
template <GridMode M>
class TilingIterator {
	private:
		sf::Vector2i start, range, current;
		unsigned int count; // counting colums per row (iso diamond only)
		
	public:
		/// Create a new iterator with a start and an iteration rage.
		/**
		 * @param start Tile position used as topleft starting position
		 * @param range Number of rows and columns to iterate over
		 */
		TilingIterator(sf::Vector2i const & start, sf::Vector2i const & range);
		
		/// Returns the current tile position
		/**
		 * @return Tile position which the iterator is indicating at
		 */
		sf::Vector2u operator*() const;
				
		/// Used to compare two iterators
		/**
		 * Two iterators satisfy the != relation if their current
		 * positions do not equal
		 * @param other Iterator to compare with
		 * @return true if positions do not equal
		 */
		bool operator!=(TilingIterator<M> const & other) const;
				
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
		/// Current camera view
		sf::View const * view;
		/// Number of pixels per tile graphics' dimension
		sf::Vector2f tile_size;
		
	public:
		/// Create a new tiling for a fixed map size and tile size
		/**
		 * @param map_size number of tiles per map dimension
		 * @param tile_size number of pixels per tile graphics' dimension
		 */
		Tiling(sf::Vector2f const & tile_size);
		
		/// Set the current camera's view
		/**
		 * Some operations require a camera to be set. This method is used to
		 * set a sf::View as a camera. The camera can be changed during the
		 * runtime of the tiling object to enable e.g. splitscreen rendering
		 * with only one instance of Tiling.
		 * @param cam The view which describes the camera
		 */
		void setView(sf::View const & cam);
		
		/// Set the tile size
		/**
		 * To use the tiling object in the context of another map, the tile
		 * size can be modified during runtime.
		 * @param tsize The new tile size to use
		 */
		void setTileSize(sf::Vector2f const & tsize);
		
		/// Get the current tile size
		/**
		 * @return tile size which is currently set
		 */
		sf::Vector2f getTileSize() const;
		
		/// Returns whether the tiling uses a camera, yet
		/**
		 * Some operations require a camera to be set. This method is used to
		 * check whether a camera was set.
		 * @return true if a camera is set
		 */
		bool hasView() const;
		
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
		
		/// Returns iterator to first tile position in range
		/**
		 * This operation requires a camera to be set!
		 * @return Iterator to first tile position in range
		 */
		TilingIterator<M> begin() const;
		
		/// Returns iterator to first tile position out of range
		/**
		 * This operation requires a camera to be set!
		 * @return Iterator to first tile position out of range
		 */
		TilingIterator<M> end() const;
};

} // ::sfext

// include implementation details
#include <SfmlExt/details/tiling.inl>
