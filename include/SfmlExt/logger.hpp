#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace sfext {

/// Base class for a logging mechanism
/**
 * In order to use the predefined operator<< functions, your logger class must
 * be derived from this base class and implement the operator() method.
 */
struct Logger {
	/// Appends a line to the logger
	/**
	 * @param line comming in
	 */
	virtual void operator()(std::string const & line) = 0;
};

// ---------------------------------------------------------------------------

/// Wrapper around std::vector<std::string> with a fixed number of elements
/**
 * If the container is filled with the given number of elements and another
 * one is added, the first one is dropped.
 */
class LogBuffer {
	private:
		using container = std::vector<std::string>;
		using const_iterator = container::const_iterator;
		
		/// buffer for the last n elements
		container buffer;
		
		/// number of elements
		std::size_t num_elements;
		
	public:
		/// Create a new LogBuffer
		/**
		 * @param num_elements to store
		 */
		LogBuffer(std::size_t num_elements);
		
		/// Add a new line
		/**
		 * This might drop the first one.
		 * @param line to add
		 */
		void addLine(std::string const & line);
		
		/// Returns reference to the last item
		/**
		 * This method can be used to e.g. add characters to the last line.
		 * The behavior is undefined if the container is empty.
		 * @return reference to last item
		 */
		std::string& getLast();
		
		/// Check whether the container is empty
		/**
		 * @return true if empty
		 */
		bool isEmpty() const;
		
		/// Get const_iterator to the first element
		/**
		 * @return const_iterator to beginning
		 */
		const_iterator begin() const;
		
		/// Get const_iterator to the end (no valid element)
		/**
		 * @return const_iterator to end (no valid element)
		 */
		const_iterator end() const;
};

// ---------------------------------------------------------------------------
// multiple operator<< overloads

Logger& operator<<(Logger& left, std::string const & str);
Logger& operator<<(Logger& left, const char * str);
Logger& operator<<(Logger& left, sf::Time const & value);
Logger& operator<<(Logger& left, sf::Color const & value);
Logger& operator<<(Logger& left, sf::Keyboard::Key value);
Logger& operator<<(Logger& left, sf::Mouse::Button value);
Logger& operator<<(Logger& left, sf::Joystick::Axis value);

template <typename T>
Logger& operator<<(Logger& left, sf::Vector2<T> const & value);

template <typename T>
Logger& operator<<(Logger& left, sf::Vector3<T> const & value);

template <typename T>
Logger& operator<<(Logger& left, sf::Rect<T> const & value);

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, Logger&>::type
operator<<(Logger& left, T value);

} // ::sfext

#include <SfmlExt/details/logger.inl>
