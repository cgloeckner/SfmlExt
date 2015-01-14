#pragma once

namespace sfext {

template <typename W, typename ...Args>
W& Gui::create(Args&&... args) {
	// create widget
	std::unique_ptr<Widget> widget{new W{std::forward<Args>(args)...}};
	auto raw = dynamic_cast<W*>(widget.get());
	// obtain ownership to gui container
	widgets.push_back(std::move(widget));
	if (focus == -1) {
		changeFocus(0);
	}
	return *raw;
}

template <typename W>
void Gui::setFocus(W& widget) {
	// search widget
	auto raw = dynamic_cast<Widget*>(&widget);
	int i = 0;
	for (; i < widgets.size(); ++i) {
		if (widgets[i].get() == raw) {
			// change focus
			changeFocus(i);
			return;
		}
	}
}

} // ::sfext
