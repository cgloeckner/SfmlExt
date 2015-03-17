namespace sfext {

template <typename Context>
template <typename ...Args>
Application<Context>::Application(Context& context, Args&&... args)
	: window{std::forward<Args>(args)...}
	, context{context}
	, pending{nullptr}
	, states{} {
}

template <typename Context>
template <typename S, typename ...Args>
void Application<Context>::emplace(Args&&... args) {
	pending = state_ptr{new S{*this, context, std::forward<Args>(args)...}};
}

template <typename Context>
template <typename S>
void Application<Context>::push(std::unique_ptr<S>& ptr) {
	pending = std::move(ptr);
	ptr = nullptr;
}

template <typename Context>
sf::RenderWindow& Application<Context>::getWindow() {
	return window;
}

template <typename Context>
void Application<Context>::run() {
	unsigned short frames = 0u;
	unsigned short time = 0u;
	sf::Clock clock;
	
	while (window.isOpen()) {
		// handle pending state
		if (pending != nullptr) {
			states.push_back(std::move(pending));
			pending = nullptr;
		}
		auto& current = *states.back();
		
		// propagate input events
		sf::Event event;
		while (window.pollEvent(event)) {
			current.handle(event);
		}
		
		// handle quitting state
		if (current.hasQuit()) {
			states.pop_back();
			if (states.empty()) {
				window.close();
			}
			continue;
		}
		
		// update framerate counter
		++frames;
		auto elapsed = clock.restart();
		time += elapsed.asMilliseconds();
		if (time >= 1000u) {
			current.onFramerateUpdate(frames);
			time -= 1000u;
			frames = 0u;
		}
		
		// update state
		current.update(elapsed);
		
		// render state
		window.clear(sf::Color::Black);
		window.draw(current);
		window.display();
	}
}

// ---------------------------------------------------------------------------

template <typename Context>
State<Context>::State(Application<Context>& application, Context& context)
	: application{application}
	, context{context}
	, _quit{false} {
}

template <typename Context>
State<Context>::~State() {
}

template <typename Context>
void State<Context>::quit() {
	_quit = true;
}

template <typename Context>
Context& State<Context>::getContext() {
	return context;
}

template <typename Context>
Context const & State<Context>::getContext() const {
	return context;
}

template <typename Context>
Application<Context>& State<Context>::getApplication() {
	return application;
}

template <typename Context>
Application<Context> const & State<Context>::getApplication() const {
	return application;
}

template <typename Context>
bool State<Context>::hasQuit() const {
	return _quit;
}

template <typename Context>
void State<Context>::onFramerateUpdate(float framerate) {
}

}
