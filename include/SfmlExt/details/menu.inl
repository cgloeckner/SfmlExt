#pragma once

namespace sfext {

template <typename W, typename ...Args>
W& Menu::create(Args&&... args) {
	// create widget
	std::unique_ptr<Widget> widget{new W{std::forward<Args>(args)...}};
	auto raw = dynamic_cast<W*>(widget.get());
	// obtain ownership to gui container
	widgets.push_back(std::move(widget));
	if (widgets.size() == 1u) {
		// focus first widget
		changeFocus(0u);
	}
	return *raw;
}

template <typename W>
void Menu::setFocus(W& widget) {
	// search widget
	auto raw = dynamic_cast<Widget*>(&widget);
	std::size_t i = 0u;
	for (; i < widgets.size(); ++i) {
		if (widgets[i].get() == raw) {
			// change focus
			changeFocus(i);
			return;
		}
	}
}

} // ::sfext
