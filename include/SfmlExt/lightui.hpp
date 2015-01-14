#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>
#include <Thor/Input/Action.hpp>
#include <Thor/Input/ActionMap.hpp>
#include <Thor/Input/EventSystem.hpp>

namespace sfext {

/// Implemented gui actions
enum class Action {
	Activate,	// triggered, when a widget is activated
	Up,			// triggered, when changing focus to next widget
	Down		// triggered, when changing focus to previous widget
};

/// Base class for all widgets
/**
 * In order to use lightui, you need to implement your widgets. This offers
 * much flexibility and customization possibilites.
 */
class Widget
		: public sf::Drawable {
		
	private:
		/// Determines whether a widget is visible or not
		bool visible;
		
	public:
		/// Default ctor
		Widget();
		
		/// Virtual default dtor
		virtual ~Widget();
		
		/// Getter for visiblity
		/**
		 * @return true if widget is visible
		 */
		bool isVisible() const;
		
		/// Setter for visibility
		/**
		 * @param visible Determines visibility
		 */
		void setVisible(bool visible);
		
		/// Offers customization about how the widget behaves on focus changes
		/**
		 * Needs to be implemented by your widget class!
		 * @param focused Describes whether the widget is now focused or not
		 */
		virtual void setFocus(bool focused) = 0;
		
		/// Offers customization about how the widget is placed
		/**
		 * Needs to be implemented by your widget class!
		 * @param pos Screen position to place the widget at
		 */
		virtual void setPosition(sf::Vector2f const & pos) = 0;
};

/// Gui container object
/**
 * The container is used as context and factory for a lightui-based
 * application.
 */
class Gui final
		: public sf::Drawable {
		
	private:
		/// callback signature for activate calls
		using Callback = std::function<void()>;
		
		/// list of all bound widgets
		std::vector<std::unique_ptr<Widget>> widgets;
		/// index of the currently focus widget (or -1 at start)
		int focus;
		
		/// maps input actions to gui actions (see Action enum)
		thor::ActionMap<Action> actions;
		/// "activate" callback per widget
		std::unordered_map<Widget const *, Callback> callbacks;
		
		// -- workaround against too fast navigation
		/// describes whether handling navigation is currently delayed
		bool delay_navi;
		/// counts from 0 up to navi_delay while navigation is delayed
		sf::Time navi_tick;
		/// describes how many time the navigation is delayed
		sf::Time const & navi_delay;
		
		/// Handling changing focus
		/**
		 * The currently focused widget (if the container has already focused)
		 * is unfocused before the new one is focused
		 * @param focus Index of widget which gains the focus
		 */
		void changeFocus(int focus);
		
	public:
		/// Create a gui container with a specified navigation delay time
		/**
		 * Because navigation would be handled too fast, the workaround is
		 * needed to delay navigation handling.
		 * @param navi_delay Describes how many time the navigation is delayed
		 */
		Gui(sf::Time const & navi_delay);
		
		/// Factory method for creating new widgets
		/**
		 * Creates a new widget, moves it's ownership to the container and
		 * returns a reference to the widget. If the created widget is the
		 * first widget in this container, it will be focused automatically.
		 * Template argument W describes the type of the actual widget class.
		 * This should match one of your widget sub-classes. The variadic
		 * argument list Args contains multiple arguments which are forwarded
		 * to W's ctor.
		 * @param args Forwarded to W's ctor
		 * @return reference to the widget
		 */
		template <typename W, typename ...Args>
		W& create(Args&&... args);
		
		/// Force container to set focus to the given widget
		/**
		 * Forces the focus onto the given widget, if it belongs to this
		 * container. This might be useful if you want to focus e.g. the 2nd
		 * option on default.
		 * @param widget Widget to set focus at
		 */
		template <typename W>
		void setFocus(W& widget);
		
		/// Bind an gui action to an input action
		/**
		 * Binds an gui `action` to an `input` action.
		 * @param action Gui Action to bind
		 * @param action Input action to bind with
		 */
		void bindAction(Action action, thor::Action input);
		
		/// Bind an activation callback for a widget
		/**
		 * Binds an activation callback for the given widget, if it belongs
		 * to this container.
		 * @param widget Widget to bind the callback for
		 * @param lambda Activation callback to bind to the widget
		 */
		void bindCallback(Widget const & widget, Callback lambda);
		
		/// Notify the container about an event
		/**
		 * The container will store all events until `update()` is called.
		 * @param event Event to store
		 */
		void notify(sf::Event const & event);
		
		/// Update container
		/**
		 * Performs actions for all bound events. After this, the stored events
		 * are discarded. Navigation delay is handled here.
		 * @param elapsed Time duration since the last frame.
		 */
		void update(sf::Time const & elapsed);
		
		/// Draws the container's widgets
		/**
		 * Invokes drawing all widgets to the given target
		 * @param target Render target to draw to
		 * @param states Render states to use while drawing
		 */
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

} // ::sfext

// include implementation details
#include <SfmlExt/details/lightui.inl>
