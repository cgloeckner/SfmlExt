# What is SfmlExt?
It is a small set of features I found useful when writing Games using C++ and SFML. Feel free to explore :)

## Dependencies
- C++11
- SFML 2.2 (might also work with 2.x, but not tested, yet)
- Boost.Math, Boost.Algorithm
- Thor 2.0 (see below)

## Licence
All resources inside `data/wesnoth.org/` are taken from **Battle for Wesnoth**, so all copyright is reserved by the corresponding artists. See www.wesnoth.org for more details. All other resources are made on my own - just for demonstration purpose. They're public domain resources. See the `LICENCE` file for information about the source code of this repository. Feel free to share!

## Features
- `atlas`: Image atlas implementation to create large framesets from many small single frames. It also supports shrinking images to their minimum before adding them.
- `cache`: Multi-resource cache holding unique ownership. Each resource is identified by it's filename.
- `tiling`: Provides different 2d tiling approaches (such as orthogonal or isometric tiling) as well as range-based iteration in rendering order.
- `menu`: A light-weight and customizable gui implementation for option-based menus (using pure keyboard/gamepad input).
- `state`: A customizable context-related state machine and application wrapper class.
- `logger`: Blueprint for a logging mechanism with support for various SFML types.
- `fader`: Provides a fading implementation for Sounds and Music.

See `examples/` directory for full (compilable) examples.

## Dependency to Thor
Some features depend on the Thor Library. But `atlas` provides also setting an origin for a frame, e.g. when using `sf::Sprite`. Because this isn't supported by Thor, yet, the example code for `atlas` depends on a folk of Thor. The patch is located in this repository: `thor_changes.diff`

# About the features

## About `atlas`
The motivation of `atlas` is driven by the fact, that GPUs have an individual maximum texture size, which limits possibilities when porting a game to an older or less-powerfull platform (e.g. a netbook). But most sprite framesets have lots of spaced pixels inside, so shrinking them would be possible without losing quality.
The idea of `atlas` is to have an interface to merge multiple single image (e.g. frame of a sprite animation) and pack them tightly together to achieve a well-packed texture atlas. This implies having individual clipping rectangles and offsets (determining the image's origin) for each frame. This mapping between original image (e.g. identified by a string or an integer, see `Atlas<>`) to the "clipping information" (rectangle + offset) is also offered by `atlas`.
There are two classes: An atlas generator and the actual atlas. The generator collects all images and creates the atlas. The atlas itself holds the tighly packed texture and the clipping information.

## About `cache`
Of course there is already a cache at **Thor**. But this one uses `shared_ptr` which is another approach of "how the world could work". An easy approach is implemented by `cache`: The cache holds all resources uniquely until the cache is destroyed. So it can always deliver references to desired resources, which can be used by other parts of the code. The downside is that the `cache` must exist as long as any of its resources if used outside of it. But in most cases, a resource holder is bound to the game instance or the entire program, so it will be released when the game session or the entire program is quit.
The `cache` implementation offers a single structure for holding multiple resources of multiple types. In order to load a resource, the SFML-like interface `bool T::loadFromFile(string const &)` is used by the default loader. Feel free to specialize the default loader template for your types or write your own loader. The loader can always be determine when acquiring a resource.

```c++
sfext::Cache cache;
sf::Sprite sprite;
sprite.setTexture(cache.get<sf::Texture>("data/enemy/goblin.png"));
```

## About `tiling`
Tiling can be straight-forward or a bit strange - depending on the actual type of tiling (e.g. pure orthogonal, diamond-shaped isometric etc.). Also traversing all visible tiles can be bloated, especially if multiple parts need to traverse the tiles on their own. The idea of `tiling` is to determine the type of tiling at compile-time for best efficiency.
So at runtime, you only need to apply your `sf::View` to the `tiling` and iterate it. While execution the range-based loop, all visible tiles are delivered in rendering order. If you need to access tiles which are out of view but located near by (e.g. to fetch light sources whose light cone might be visible on the screen), you can set a padding width/height.
So `tiling` allows simple and fast iteration of visible tiles.

```c++
// typedef your tiling
using Tiling = sfext::Tiling<sfext::GridMode::Orthogonal>;

// create tiling and view
Tiling tiling{{64.f, 64.f}}; // tilesize 64x64
sf::View view = window.getDefaultView();

// apply view and iterate
tiling.setView(view);
for (auto const & pos: tiling) {
	// process tile position
}
```

## About `menu`
`menu` is a very simple approach of providing a minimalistic set of widgets, whose appearance can be customized by the programmer using it. There is a set of base widgets such as a button. Those base widgets can be extended by implementing their actual representation (e.g. a simple text label or a more complex button sprite). All widgets need to be created using an owning container called `Menu`. It will create and own widgets as well as deliver references to the widgets in order to access them. Lambda functions are used to specify their behavior (e.g. on button activation).
Another idea of `menu` is to enable pure keyboard- and/or gamepad-based menu control. So there's a limited set of commands that can be bound individually. Those bindings can be set using `Thor::Action`.

# Future Plans
- Remove Cache because Thor now provides unique ownership caching
- Add more "About XY"-stuff
- (re)write unit testing
- Extend `tiling` to provide staggered isometric and hexagonal maps.
- `astar`: A customized A-Star-implementation for 2d grids, because typical A-Star on general graphs cannot easily be implemented with `std::priority_queue`.
