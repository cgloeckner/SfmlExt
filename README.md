SfmlExt
=======

This is a small set of features I found useful when writing Games using C++ and SFML. Feel free to explore :)

**Dependencies**
- C++11
- SFML 2.2 (might also work with 2.x, but not tested, yet)
- Boost.Math
- Thor (examples only!)

**Licence**: All graphics (except `data/tileset.png`) and sounds/music are taken from **Battle for Wesnoth**, so all copyright is reserved by the corresponding artists. See www.wesnoth.org for more details. See the `LICENCE` file for information about the source code of this repository. Feel free to share!

**Features**
- `atlas`: Provides image atlas creating to create large framesets from many small single frames.
- `cache`: Provides unique ownership based resource caching. Each resource is identified by it's filename.
- `tiling`: Provides different 2d tiling approaches such as orthogonal or isometric tiling.

See `examples/` directory for full (compilable) examples.

**Future plans**
- Extend `cache` to handle sf::Shader
- `gui`: A light-weight and highly customizable gui implementation without mouse support (but keyboard/gamepad access).
- `astar`: A customized A-Star-implementation for 2d grids, because typical A-Star on general graphs cannot easily be implemented with `std::priority_queue`.
