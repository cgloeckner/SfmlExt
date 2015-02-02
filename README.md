SfmlExt
=======

This is a small set of features I found useful when writing Games using C++ and SFML. Feel free to explore :)

**Dependencies**
- C++11
- SFML 2.2 (might also work with 2.x, but not tested, yet)
- Boost.Math, Boost.Algorithm
- Thor (see below)

**Licence**: All resources inside `data/wesnoth.org/` are taken from **Battle for Wesnoth**, so all copyright is reserved by the corresponding artists. See www.wesnoth.org for more details. All other resources are made on my own - just for demonstration purpose. They're public domain resources. See the `LICENCE` file for information about the source code of this repository. Feel free to share!

**Features**
- `atlas`: Image atlas implementation to create large framesets from many small single frames. It also supports shrinking images to their minimum before adding them.
- `cache`: Multi-resource cache holding unique ownership. Each resource is identified by it's filename.
- `tiling`: Provides different 2d tiling approaches (such as orthogonal or isometric tiling) as well as range-based iteration in rendering order.
- `menu`: A light-weight and customizable gui implementation for option-based menus (using pure keyboard/gamepad input).
- `state`: A customizable context-related state machine and application wrapper class.
- `logger`: Blueprint for a logging mechanism with support for various SFML types.

See `examples/` directory for full (compilable) examples.

**Dependency to Thor**
Some features depend on the Thor Library. But `atlas` provides also setting an origin for a frame, e.g. when using `sf::Sprite`. Because this isn't supported by Thor, yet, the example code for `atlas` depends on a folk of Thor. The commit can be found here: https://github.com/cgloeckner/Thor/commit/7bd9d4a63947958a00691eae67da5a5378b1b343

**Future plans**
- Do more automated testing :)
- Extend `cache` to handle sf::Shader.
- Extend `tiling` to provide staggered isometric and hexagonal maps.
- Extend `menu` to provide a text input widget
- `loading`: A thread-based progress bar implementation to enable e.g. loading game resource while redrawing the window.
- `astar`: A customized A-Star-implementation for 2d grids, because typical A-Star on general graphs cannot easily be implemented with `std::priority_queue`.
