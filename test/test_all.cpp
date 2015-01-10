#include <atlas_test.hpp>
#include <cache_test.hpp>

int main() {
	// perform atlas tests
	test_atlas_tooLargeChunk();
	test_atlas_tooManyChunks();
	test_atlas_maxNumChunks();
	test_atlas_typicalNumChunks();
	
	// perform cache tests
	test_cache_existingResource();
	test_cache_missingResource();
	test_cache_cachingOnce();
	test_cache_getKeys();
}
