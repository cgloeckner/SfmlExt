#include <lightui.hpp>

namespace sfext {

Widget::Widget()
	: visible{true} {
}

Widget::~Widget() {
}

bool Widget::isVisible() const {
	return visible;
}

void Widget::setVisible(bool visible) {
	this->visible = visible;
}

// ---------------------------------------------------------------------------

Gui::Gui(sf::Time const & navi_delay)
	: widgets{}
	, focus{-1}
	, actions{}
	, delay_navi{false}
	, navi_tick{sf::Time::Zero}
	, navi_delay{navi_delay} {
}

void Gui::changeFocus(int focus) {
	// check target widget's visibility
	auto& target = widgets.at(focus);
	if (target->isVisible()) {
		// change focus
		if (this->focus > -1) {
			widgets.at(this->focus)->setFocus(false);
		}
		target->setFocus(true);
		this->focus = focus;
	} else {
		// determine direction (up or down)
		int tmp = focus;
		bool forward{true};
		if (focus < this->focus) {
			forward = false;
		}
		do {
			// to go next index and check bounds
			if (forward) {
				++focus;
				if (focus >= widgets.size()) {
					focus = 0;
				}
			} else {
				--focus;
				if (focus < 0) {
					focus = widgets.size() - 1;
				}
			}
			// pick widget
			auto& next = widgets.at(focus);
			if (focus != this->focus && next->isVisible()) {
				// change focus
				if (this->focus > -1) {
					widgets.at(this->focus)->setFocus(false);
				}
				next->setFocus(true);
				this->focus = focus;
				return;
			}
		} while (tmp != focus);
		// no suitable successor found - focus remains unchanged
	}
}

void Gui::bindAction(Action action, thor::Action input) {
	actions[action] = input;
}

void Gui::bindCallback(Widget const & widget, Callback lambda) {
	callbacks[&widget] = lambda;
}

void Gui::notify(sf::Event const & event) {
	actions.pushEvent(event);
}

void Gui::update(sf::Time const & elapsed) {
	if (delay_navi) {
		navi_tick += elapsed;
		if (navi_tick >= navi_delay) {
			navi_tick = sf::Time::Zero;
			delay_navi = false;
		}
	}
	
	if (widgets.size() > 0 && actions.isActive(Action::Activate)) {
		// activate widget
		if (focus > -1) {
			auto& w = widgets.at(focus);
			// check visibility
			if (w->isVisible()) {
				auto i = callbacks.find(w.get());
				if (i != callbacks.end()) {
					i->second();
				}
			}
		}
	}
	if (widgets.size() > 1 && navi_tick == sf::Time::Zero) {
		// handle changing focus
		if (actions.isActive(Action::Down)) {
			// focus next widget
			delay_navi = true;
			auto new_focus = focus + 1;
			if (new_focus >= widgets.size()) {
				new_focus = 0;
			}
			changeFocus(new_focus);
		}
		if (actions.isActive(Action::Up)) {
			// focus previous widget
			delay_navi = true;
			auto new_focus = focus - 1;
			if (new_focus < 0) {
				new_focus = widgets.size() - 1;
			}
			changeFocus(new_focus);
		}
	}
	actions.clearEvents();
}

void Gui::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto const & w: widgets) {
		// check visibility
		if (!w->isVisible()) {
			continue;
		}
		// draw widget
		auto d = dynamic_cast<sf::Drawable*>(w.get());
		target.draw(*d, states);
	}
}

} // ::sfext
