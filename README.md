SfmlExt
=======

This is a small set of features I found useful when writing Games using C++ and SFML. Feel free to explore :)

**Dependencies**
- C++11
- SFML 2.2 (might also work with 2.x, but not tested, yet)
- Boost.Math
- Thor

**Licence**: All resources inside `data/wesnoth.org/` are taken from **Battle for Wesnoth**, so all copyright is reserved by the corresponding artists. See www.wesnoth.org for more details. All other resources are made on my own - just for demonstration purpose. They're public domain resources. See the `LICENCE` file for information about the source code of this repository. Feel free to share!

**Features**
- `atlas`: Image atlas implementation to create large framesets from many small single frames.
- `cache`: Multi-resource cache holding unique ownership. Each resource is identified by it's filename.
- `tiling`: Provides different 2d tiling approaches (such as orthogonal or isometric tiling).
- `lightui`: A light-weight and customizable gui implementation for option-based menus (especially with pure keyboard/gamepad input).

See `examples/` directory for full (compilable) examples.

**Future plans**
- Extend `cache` to handle sf::Shader.
- Extend `tiling` to provide staggered isometric and hexagonal maps.
- `astar`: A customized A-Star-implementation for 2d grids, because typical A-Star on general graphs cannot easily be implemented with `std::priority_queue`.
