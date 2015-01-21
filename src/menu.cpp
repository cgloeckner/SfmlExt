#include <SfmlExt/menu.hpp>

#include <iostream>

namespace sfext {

Widget::Widget()
	: visible{true}
	, func(nullptr) {
}

void Widget::bind(Widget::Callback func) {
	this->func = func;
}

void Widget::setVisible(bool visible) {
	this->visible = visible;
}

bool Widget::isVisible() const {
	return visible;
}

// ---------------------------------------------------------------------------

void Button::onActivate() {
}

void Button::handle(MenuAction action) {
	if (action == MenuAction::Activate) {
		onActivate();
		if (func != nullptr) {
			func();
		}
	}
}

// ---------------------------------------------------------------------------

Select::Select()
	: Widget{}
	, index{0u} {
}

void Select::handle(MenuAction action) {
	if (empty()) {
		return;
	}
	if (action == MenuAction::AlternatePrev) {
		if (index > 0u) {
			--index;
		} else {
			return;
		}
	} else if (action == MenuAction::AlternateNext) {
		if (index < size() - 1) {
			++index;
		} else {
			return;
		}
	}
	onChanged();
	if (func != nullptr) {
		func();
	}
}

void Select::setIndex(std::size_t index) {
	this->index = index;
	onChanged();
}

std::size_t Select::getIndex() const {
	return index;
}

// ---------------------------------------------------------------------------

Menu::Menu()
	: widgets{}
	, focus{0u}
	, binding{} {
}

void Menu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto const & w: widgets) {
		// check visibility
		if (!w->isVisible()) {
			continue;
		}
		// draw widget
		target.draw(*w, states);
	}
}

void Menu::changeFocus(std::size_t index) {
	if (widgets.empty() || index >= widgets.size()) {
		// no widget available
		return;
	}
	auto target = widgets[index].get();
	bool forward = (index > focus);
	auto initial = index;
	while (!target->isVisible()) {
		// prev/next widget
		if (forward) {
			if (index >= widgets.size() - 1) {
				index = 0u;
			} else {
				++index;
			}
		} else {
			if (index == 0u) {
				index = widgets.size() - 1;
			} else {
				--index;
			}
		}
		if (index == initial) {
			// no widget available
			return;
		}
		// fetch widget
		target = widgets[index].get();
	}
	// change focus
	widgets[focus]->setFocus(false);
	target->setFocus(true);
	focus = index;
}

void Menu::bind(MenuAction const & action, thor::Action const & input) {
	binding[action] = input;
}

void Menu::handle(sf::Event const & event) {
	binding.pushEvent(event);
}

void Menu::update() {
	if (widgets.size() > 0) {
		// handle activation
		if (binding.isActive(MenuAction::Activate)) {
			widgets[focus]->handle(MenuAction::Activate);
		}
		
		// handle alternate
		if (binding.isActive(MenuAction::AlternatePrev)) {
			widgets[focus]->handle(MenuAction::AlternatePrev);
		}
		if (binding.isActive(MenuAction::AlternateNext)) {
			widgets[focus]->handle(MenuAction::AlternateNext);
		}
		
		// handle navigation
		if (widgets.size() > 1) {
			if (binding.isActive(MenuAction::NavigatePrev)) {
				if (focus == 0u) {
					changeFocus(widgets.size() - 1);
				} else {
					changeFocus(focus - 1);
				}
			}
			if (binding.isActive(MenuAction::NavigateNext)) {
				if (focus >= widgets.size()) {
					changeFocus(0u);
				} else {
					changeFocus(focus + 1);
				}
			}
		}
	}
	
	// reset input
	binding.clearEvents();
}

} // ::sfext
