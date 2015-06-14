#include <algorithm>
#include <SFML/Graphics.hpp>
#include <boost/test/unit_test.hpp>

#include <SfmlExt/tiling.hpp>

BOOST_AUTO_TEST_CASE(tiling_iterator_ctor) {
	sfext::TilingIterator<sfext::GridMode::Orthogonal> i{{2, 1}, {2, 3}};
	sfext::TilingIterator<sfext::GridMode::Orthogonal> end{{4, 1}, {2, 3}};
	
	auto pos = *i;
	BOOST_CHECK(i != end);
	BOOST_CHECK_EQUAL(pos.x, 2u);
	BOOST_CHECK_EQUAL(pos.y, 1u);
	BOOST_CHECK_EQUAL(i.getRange().x, 2);
	BOOST_CHECK_EQUAL(i.getRange().y, 3);
	BOOST_CHECK_EQUAL(i.getRange().x, end.getRange().x);
	BOOST_CHECK_EQUAL(i.getRange().y, end.getRange().y);
}

BOOST_AUTO_TEST_CASE(tiling_ortho_iterator_step) {
	sfext::TilingIterator<sfext::GridMode::Orthogonal> i{{2, 1}, {2, 3}};
	sf::Vector2i range;
	range = i.getRange();
	
	// step to the right
	++i;
	auto pos = *i;
	BOOST_CHECK_EQUAL(pos.x, 3u);
	BOOST_CHECK_EQUAL(pos.y, 1u);
	BOOST_CHECK_EQUAL(i.getRange().x, range.x);
	BOOST_CHECK_EQUAL(i.getRange().y, range.y);
	
	// step to the top of the next line
	++i;
	pos = *i;
	BOOST_CHECK_EQUAL(pos.x, 2u);
	BOOST_CHECK_EQUAL(pos.y, 2u);
}

BOOST_AUTO_TEST_CASE(tiling_ortho_view) {
	// note: this behavior is gridmode-independent
	sfext::Tiling<sfext::GridMode::Orthogonal> tiling{{48.f, 32.f}};
	sf::View view{{200, 200, 300, 200}};
	tiling.setView(view);
	
	BOOST_CHECK_CLOSE(48.f, tiling.getTileSize().x, 0.0001f);
	BOOST_CHECK_CLOSE(32.f, tiling.getTileSize().y, 0.0001f);
	
	tiling.setTileSize({32.f, 64.f});
	BOOST_CHECK_CLOSE(32.f, tiling.getTileSize().x, 0.0001f);
	BOOST_CHECK_CLOSE(64.f, tiling.getTileSize().y, 0.0001f);
}

BOOST_AUTO_TEST_CASE(tiling_ortho_boundary) {
	sfext::Tiling<sfext::GridMode::Orthogonal> tiling{{32.f, 28.f}};
	sf::View view{{200.f, 200.f, 300.f, 200.f}};
	tiling.setView(view);

	auto range = tiling.getRange();
	// note: default padding necessary to fill entire view
	BOOST_CHECK_EQUAL(range.x, 12);	// ceil(300/32) + 2 (default padding)
	BOOST_CHECK_EQUAL(range.y, 10);	// ceil(200/28) + 2 (default padding)
	
	auto topleft = tiling.getTopleft();
	auto bottomleft = tiling.getBottomleft();
	BOOST_CHECK_EQUAL(topleft.x, bottomleft.x);
	BOOST_CHECK_EQUAL(topleft.y + range.y, bottomleft.y);
}

BOOST_AUTO_TEST_CASE(tiling_ortho_padding) {
	sfext::Tiling<sfext::GridMode::Orthogonal> tiling{{32.f, 28.f}};
	sf::View view{{200.f, 200.f, 300.f, 200.f}};
	tiling.setView(view);
	auto range = tiling.getRange();
	auto topleft = tiling.getTopleft();
	auto bottomleft = tiling.getBottomleft();
	tiling.setPadding({12u, 7u});
	
	BOOST_CHECK_EQUAL(tiling.getRange().x, range.x + 24u);
	BOOST_CHECK_EQUAL(tiling.getRange().y, range.y + 14u);
	BOOST_CHECK_EQUAL(tiling.getTopleft().x, topleft.x - 12u);
	BOOST_CHECK_EQUAL(tiling.getTopleft().y, topleft.y - 7u);
	BOOST_CHECK_EQUAL(topleft.x, bottomleft.x);
	BOOST_CHECK_EQUAL(topleft.y + range.y, bottomleft.y);
}

BOOST_AUTO_TEST_CASE(tiling_ortho_iteration) {
	sfext::Tiling<sfext::GridMode::Orthogonal> tiling{{32.f, 28.f}};
	sf::View view{{200.f, 200.f, 300.f, 200.f}};
	tiling.setView(view);

	auto range = tiling.getRange();
	auto n = 0u;
	std::for_each(begin(tiling), end(tiling), [&n](sf::Vector2u const & pos) {
		++n;
	});
	BOOST_CHECK_EQUAL(n, range.x * range.y);
}

BOOST_AUTO_TEST_CASE(tiling_orthogonal_toscreen) {
	sfext::Tiling<sfext::GridMode::Orthogonal> tiling{{32.f, 28.f}};
	
	auto spos = tiling.toScreen({12.f, 9.5f});
	BOOST_CHECK_CLOSE(spos.x, 384.f, 0.0001f);
	BOOST_CHECK_CLOSE(spos.y, 266.f, 0.0001f);
}

BOOST_AUTO_TEST_CASE(tiling_orthogonal_fromscreen) {
	sfext::Tiling<sfext::GridMode::Orthogonal> tiling{{32.f, 28.f}};
	
	auto wpos = tiling.fromScreen({300.f, 140.f});
	BOOST_CHECK_CLOSE(wpos.x, 9.375f, 0.0001f);
	BOOST_CHECK_CLOSE(wpos.y, 5.f, 0.0001f);
}

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tiling_isodiamond_iterator_step) {
	sfext::TilingIterator<sfext::GridMode::IsoDiamond> i{{2, 1}, {2, 3}};
	sf::Vector2i range;
	range = i.getRange();
	
	// step to the right (view-related)
	++i;
	auto pos = *i;
	BOOST_CHECK_EQUAL(pos.x, 3u);
	BOOST_CHECK_EQUAL(pos.y, 0u);
	BOOST_CHECK_EQUAL(i.getRange().x, range.x);
	BOOST_CHECK_EQUAL(i.getRange().y, range.y);
	
	// "zig"-step to the top of the next line (view-related)
	++i;
	pos = *i;
	BOOST_CHECK_EQUAL(pos.x, 3u);
	BOOST_CHECK_EQUAL(pos.y, 1u);
	
	// step to the right (view-related)
	++i;
	pos = *i;
	BOOST_CHECK_EQUAL(pos.x, 4u);
	BOOST_CHECK_EQUAL(pos.y, 0u);
	
	// "zag"-step to the top of the next line (view-related)
	++i;
	pos = *i;
	BOOST_CHECK_EQUAL(pos.x, 3u);
	BOOST_CHECK_EQUAL(pos.y, 2u);
}

BOOST_AUTO_TEST_CASE(tiling_isodiamond_boundary) {
	sfext::Tiling<sfext::GridMode::IsoDiamond> tiling{{32.f, 28.f}};
	sf::View view{{200.f, 200.f, 300.f, 200.f}};
	tiling.setView(view);

	auto range = tiling.getRange();
	// note: default padding necessary to fill entire view
	BOOST_CHECK_EQUAL(range.x, 14);	// ceil(300/32) + 4 (default padding)
	BOOST_CHECK_EQUAL(range.y, 24);	// (ceil(200/28) + 4 (default padding)) * 2 (iso height)
	
	auto center = sf::Vector2u{tiling.fromScreen(view.getCenter())};
	auto bottomleft = tiling.getBottomleft();
	BOOST_CHECK_EQUAL(center.x - range.x + range.y / 2, bottomleft.x);
	BOOST_CHECK_EQUAL(center.y + range.y / 2, bottomleft.y);
}

BOOST_AUTO_TEST_CASE(tiling_isodiamond_toscreen) {
	sfext::Tiling<sfext::GridMode::IsoDiamond> tiling{{32.f, 14.f}};
	
	auto spos = tiling.toScreen({12.f, 9.5f});
	BOOST_CHECK_CLOSE(spos.x, 40.f, 0.0001f);
	BOOST_CHECK_CLOSE(spos.y, 150.5f, 0.0001f);
}

BOOST_AUTO_TEST_CASE(tiling_isodiamond_fromscreen) {
	sfext::Tiling<sfext::GridMode::IsoDiamond> tiling{{32.f, 14.f}};
	
	auto wpos = tiling.fromScreen({300.f, 140.f});
	BOOST_CHECK_CLOSE(wpos.x, 19.375f, 0.0001f);
	BOOST_CHECK_CLOSE(wpos.y, 0.625f, 0.0001f);
}

