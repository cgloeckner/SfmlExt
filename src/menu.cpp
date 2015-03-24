#include <SfmlExt/menu.hpp>

#include <iostream>

namespace sfext {

Widget::Widget()
	: visible{true}
	, func(nullptr) {
}

Widget::~Widget() {
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

} // ::sfext
