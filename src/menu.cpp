#include <SfmlExt/menu.hpp>

#include <iostream>

namespace sfext {

Widget::Widget()
	: visible{true} {
}

Widget::~Widget() {
}

void Widget::setVisible(bool visible) {
	this->visible = visible;
}

bool Widget::isVisible() const {
	return visible;
}

void Widget::handle(sf::Uint32 unicode) {
	// not implemented by most widgets
}

// ---------------------------------------------------------------------------

void Button::onActivate() {
}

void Button::handle(MenuAction action) {
	if (action == MenuAction::Activate) {
		onActivate();
		if (activate != nullptr) {
			activate();
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
	if (action == MenuAction::Activate) {
		if (activate != nullptr) {
			activate();
		}
	} else if (action == MenuAction::AlternatePrev || action == MenuAction::AlternateNext) {
		if (change != nullptr) {
			change();
		}
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

Input::Input()
	: Widget{}
	, whitelist{}
	, blacklist{}
	, typing{nullptr} {
}

void Input::onTyping() {
}

bool Input::isAllowed(sf::Uint32 unicode) const {
	auto i = std::find(begin(blacklist), end(blacklist), unicode);
	if (i != blacklist.end()) {
		// symbol is explicitly forbidden
		return false;
	}
	if (whitelist.empty()) {
		// symbol is implicitly allowed by empty whitelist
		return true;
	}
	i = std::find(begin(whitelist), end(whitelist), unicode);
	if (i == whitelist.end()) {
		// symbol is implicitly forbidden by non-empty whitelist
		return false;
	}
	// symbol is explicitly allowed by non-empty whitelist
	return true;
}

void Input::handle(sfext::MenuAction action) {
	// nothing to do
}

void Input::handle(sf::Uint32 unicode) {
	auto text = getString();
	bool allowed = true;
	if (unicode == '\b') {
		// handle backspace
		text = text.substring(0u, text.getSize() - 1u);
	} else {
		allowed = isAllowed(unicode);
		if (allowed) {
			// add character
			text += unicode;
		}
	}
	if (allowed) {
		setString(text);
	}
	if (typing != nullptr) {
		typing(unicode, allowed);
	}
}

} // ::sfext
