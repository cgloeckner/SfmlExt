SfmlExt
=======

All extensions' examples make use of resources from **Battle for Wesnoth**, which are NOT included here. Battle for Wesnoth is an open-source game, so you can easily get it's resources.

**Features**
- `image_atlas`: Provides adding multiple images to container, which is another image. When adding, the atlas tells you where to find the original image inside the container. See `altas_example.cpp` for further details.
- `resource_cache` and `resource_archive`: Provides a simple API to cache resources and deal with binary resource archive files. A cached resource is uniquely owned by the cache. So a reference is returned (or an exception thrown) when querying a resource. An archive allows to dump multiple resources (of the same type) into a single binary file. The API allows writing and reading such files.

