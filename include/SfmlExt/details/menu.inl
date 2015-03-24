#pragma once

namespace sfext {

template <typename T, typename Compare>
Menu<T, Compare>::Menu()
	: widgets{}
	, focus{0u}
	, binding{} {
}

template <typename T, typename Compare>
void Menu<T, Compare>::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto const & pair: widgets) {
		auto& w = *(pair.second);
		// check visibility
		if (!w.isVisible()) {
			continue;
		}
		// draw widget
		target.draw(w, states);
	}
}

template <typename T, typename Compare>
void Menu<T, Compare>::changeFocus(T key, bool forward) {
	auto i = widgets.find(key);
	if (i == widgets.end()) {
		return;
	}
	// skip invisibe widget(s)
	auto j = i;
	while (!j->second->isVisible()) {
		if (forward) {
			++j;
			if (j == widgets.end()) {
				j = widgets.begin();
			}
		} else {
			if (j == widgets.begin()) {
				// workaround: iterator vs. reverse_iterator
				auto back = widgets.rbegin();
				j = widgets.find(back->first);
			} else {
				--j;
			}
		}
		if (j == i) {
			// not visible widget found
			return;
		}
	}
	// change focus
	auto prev = widgets.find(focus);
	if (prev != widgets.end()) {
		prev->second->setFocus(false);
	}
	j->second->setFocus(true);
	focus = j->first;
}

template <typename T, typename Compare>
template <typename W, typename ...Args>
W& Menu<T, Compare>::acquire(T key, Args&&... args) {
	// create widget
	std::unique_ptr<Widget> widget{new W{std::forward<Args>(args)...}};
	auto raw = dynamic_cast<W*>(widget.get());
	// obtain ownership to gui container
	widgets[key] = std::move(widget);
	if (widgets.size() == 1u) {
		// focus first widget
		changeFocus(key);
	}
	return *raw;
}

template <typename T, typename Compare>
template <typename W>
W& Menu<T, Compare>::query(T key) {
	return dynamic_cast<W&>(*(widgets[key]));
}

template <typename T, typename Compare>
void Menu<T, Compare>::release(T key) {
	auto i = widgets.find(key);
	if (i != widgets.end()) {
		widgets.erase(i);
	}
}

template <typename T, typename Compare>
void Menu<T, Compare>::setFocus(T key) {
	// search widget
	auto i = widgets.find(key);
	if (i != widgets.end()) {
		changeFocus(i->first);
	}
}

template <typename T, typename Compare>
void Menu<T, Compare>::bind(MenuAction const & action, thor::Action const & input) {
	binding[action] = input;
}

template <typename T, typename Compare>
void Menu<T, Compare>::handle(sf::Event const & event) {
	binding.pushEvent(event);
}

template <typename T, typename Compare>
void Menu<T, Compare>::update() {
	if (!widgets.empty()) {
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
		if (binding.isActive(MenuAction::NavigatePrev)) {
			auto i = widgets.find(focus);
			if (i == widgets.begin()) {
				changeFocus(widgets.rbegin()->first);
			} else {
				--i;
				changeFocus(i->first, false);
			}
		}
		if (binding.isActive(MenuAction::NavigateNext)) {
			auto i = widgets.find(focus);
			++i;
			if (i == widgets.end()) {
				i = widgets.begin();
			}
			changeFocus(i->first);
		}
	}
	
	// reset input
	binding.clearEvents();
}

} // ::sfext
