#pragma once
#include <algorithm>

namespace sfext {

template <typename T>
Menu<T>::Menu()
	: widgets{}
	, focus{0u}
	, binding{} {
}

template <typename T>
typename Menu<T>::container::iterator Menu<T>::at(T key) {
	return std::find_if(begin(widgets), end(widgets), [&key](pair const & pair) {
		return (pair.first == key);
	});
}

template <typename T>
void Menu<T>::draw(sf::RenderTarget& target, sf::RenderStates states) const {
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

template <typename T>
void Menu<T>::changeFocus(T key, bool forward) {
	auto i = at(key);
	// skip invisibe widget(s)
	auto j = i;
	while (!j->second->isVisible()) {
		if (forward) {
			++j;
			if (j == end(widgets)) {
				j = begin(widgets);
			}
		} else {
			if (j == begin(widgets)) {
				j = std::prev(end(widgets));
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
	auto prev = at(focus);
	if (prev != end(widgets)) {
		prev->second->setFocus(false);
	}
	j->second->setFocus(true);
	focus = j->first;
}

template <typename T>
template <typename W, typename ...Args>
W& Menu<T>::acquire(T key, Args&&... args) {
	// create widget
	std::unique_ptr<Widget> widget{new W{std::forward<Args>(args)...}};
	auto raw_ptr = dynamic_cast<W*>(widget.get());
	// obtain ownership to gui container
	widgets.emplace_back(key, std::move(widget));
	if (widgets.size() == 1u) {
		// focus first widget
		changeFocus(key);
	}
	return *raw_ptr;
}

template <typename T>
template <typename W>
W& Menu<T>::query(T key) {
	auto i = at(key);
	return dynamic_cast<W&>(*(i->second));
}

template <typename T>
void Menu<T>::release(T key) {
	auto i = at(key);
	if (i != end(widgets)) {
		widgets.erase(i);
	}
}

template <typename T>
void Menu<T>::setFocus(T key) {
	auto i = at(key);
	if (i != end(widgets)) {
		changeFocus(i->first);
	}
}

template <typename T>
template <typename W>
void Menu<T>::setFocus(W const & widget) {
	auto i = std::find_if(begin(widgets), end(widgets), [&widget](pair const & pair) {
		return (pair.second.get() == &widget);
	});
	if (i != end(widgets)) {
		changeFocus(i->first);
	}
}

template <typename T>
void Menu<T>::bind(MenuAction const & action, thor::Action const & input) {
	binding[action] = input;
}

template <typename T>
void Menu<T>::handle(sf::Event const & event) {
	binding.pushEvent(event);
}

template <typename T>
void Menu<T>::update() {
	if (!widgets.empty()) {
		auto i = at(focus);
		// handle activation
		if (binding.isActive(MenuAction::Activate)) {
			i->second->handle(MenuAction::Activate);
		}
		
		// handle alternate
		if (binding.isActive(MenuAction::AlternatePrev)) {
			i->second->handle(MenuAction::AlternatePrev);
		}
		if (binding.isActive(MenuAction::AlternateNext)) {
			i->second->handle(MenuAction::AlternateNext);
		}
		
		// handle navigation
		if (binding.isActive(MenuAction::NavigatePrev)) {
			if (i == begin(widgets)) {
				i = std::prev(end(widgets));
			} else {
				--i;
			}
			changeFocus(i->first, false);
		}
		if (binding.isActive(MenuAction::NavigateNext)) {
			++i;
			if (i == end(widgets)) {
				i = begin(widgets);
			}
			changeFocus(i->first);
		}
	}
	
	// reset input
	binding.clearEvents();
}

} // ::sfext
