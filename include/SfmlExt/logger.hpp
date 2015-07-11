#pragma once
#include <ostream>
#include <ctime>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Thor/Input.hpp>
#include <Thor/Graphics.hpp>

namespace sfext {

// helper to get current date and time as string

struct {
	inline std::string operator()() const {
		// create a string with the current date and time
		time_t rawtime;
		struct tm* timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 80, "%X %x ", timeinfo);
		return std::string{buffer};
	}
} now;

// ----------------------------------------------------------------------------
// some type traits to detect whether T can be converted using thor::toString

template <typename T>
struct is_thor_primitive
	: std::false_type {
};

template <>
struct is_thor_primitive<sf::Color>
	: std::true_type {
};

template <>
struct is_thor_primitive<sf::Keyboard::Key>
	: std::true_type {
};

template <>
struct is_thor_primitive<sf::Mouse::Button>
	: std::true_type {
};

template <>
struct is_thor_primitive<sf::Joystick::Axis>
	: std::true_type {
};

template <typename T>
struct is_thor_primitive<sf::Vector2<T>>
	: std::true_type {
};

template <typename T>
struct is_thor_primitive<sf::Vector3<T>>
	: std::true_type {
};

template <typename T>
struct is_thor_primitive<sf::Rect<T>>
	: std::true_type {
};

// ----------------------------------------------------------------------------
// some type traits to detect whether T can be converted using sfext::toString

template <typename T>
struct is_sfext_primitive
	: std::false_type {
};

template <>
struct is_sfext_primitive<sf::Time>
	: std::true_type {
};

template <>
struct is_sfext_primitive<sf::VideoMode>
	: std::true_type {
};

template <>
struct is_sfext_primitive<decltype(now)>
	: std::true_type {
};

// ----------------------------------------------------------------------------
// conversion helpers

inline std::string toString(sf::Time const & t) {
	return std::to_string(t.asMilliseconds()) + "ms";
}

inline std::string toString(sf::VideoMode const & m) {
	return std::to_string(m.width) + "x" + std::to_string(m.height) + "x"
		+ std::to_string(m.bitsPerPixel);
}

inline std::string toString(decltype(now) const & n) {
	return n();
}

// ----------------------------------------------------------------------------
// enable streaming various types to std::ostream

template <typename T>
typename std::enable_if<is_thor_primitive<T>::value, std::ostream&>::type
operator<<(std::ostream& lhs, T const & rhs) {
	return lhs << thor::toString(rhs);
}

template <typename T>
typename std::enable_if<is_sfext_primitive<T>::value, std::ostream&>::type
operator<<(std::ostream& lhs, T const & rhs) {
	return lhs << toString(rhs);
}

// ----------------------------------------------------------------------------
// actual logging wrapper

class Logger {
	template <typename T>
	friend Logger& operator<<(Logger& lhs, T const & rhs);

	private:
		std::vector<std::ostream*> streams;
		
	public:
		inline void add(std::ostream& stream) {
			streams.push_back(&stream);
		}
		
		inline void flush() {
			for (auto& ptr: streams) {
				ptr->flush();
			}
		}
};

template <typename T>
Logger& operator<<(Logger& lhs, T const & rhs) {
	for (auto& ptr: lhs.streams) {
		(*ptr) << rhs;
	}
	return lhs;
}

} // ::sfext
