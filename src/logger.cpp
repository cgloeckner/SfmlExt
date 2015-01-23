#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <Thor/Input/InputNames.hpp>

#include <SfmlExt/logger.hpp>

namespace sfext {

LogBuffer::LogBuffer(std::size_t num_elements)
	: buffer{}
	, num_elements{num_elements} {
}

void LogBuffer::addLine(std::string const & line) {
	buffer.push_back(line);
	
	// consolidate
	if (buffer.size() > num_elements) {
		auto i = buffer.begin();
		auto j = std::next(buffer.begin());
		auto k = buffer.end();
		while (i != k && j != k) {
			*i = std::move(*j);
			++i;
			++j;
		}
		buffer.pop_back();
	}
}

std::string& LogBuffer::getLast() {
	return buffer.back();
}

bool LogBuffer::isEmpty() const {
	return buffer.empty();
}

LogBuffer::const_iterator LogBuffer::begin() const {
	return buffer.begin();
}

LogBuffer::const_iterator LogBuffer::end() const {
	return buffer.end();
}

// ---------------------------------------------------------------------------

Logger& operator<<(Logger& left, std::string const & str) {
	if (str.empty()) {
		return left;
	}
	
	// split into lines
	std::vector<std::string> buffer;
	boost::algorithm::split(buffer, str, boost::algorithm::is_any_of("\n"));
	
	bool completed = (str.back() == '\n');
	if (buffer.back().empty()) {
		buffer.pop_back();
	}
	std::size_t i = 0u;
	auto size = buffer.size();
	
	// add lines to logger
	for (auto& line: buffer) {
		if (i < size - 1 || completed) {
			left(line + '\n');
		} else {
			// last line without '\n'
			left(line);
		}
		++i;
	};
	
	return left;
}

Logger& operator<<(Logger& left, const char * str) {
	return left << std::string(str);
}

Logger& operator<<(Logger& left, sf::Time const & value) {
	std::string tmp = std::to_string(value.asMilliseconds()) + "ms";
	return left << tmp;
}

Logger& operator<<(Logger& left, sf::Color const & value) {
	std::string tmp = "(" + std::to_string(value.r) + ","
		+ std::to_string(value.g) + "," + std::to_string(value.b) + ","
		+ std::to_string(value.a) + ")";
	return left << tmp;
}

Logger& operator<<(Logger& left, sf::Keyboard::Key value) {
	return left << thor::toString(value);
}

Logger& operator<<(Logger& left, sf::Mouse::Button value) {
	return left << thor::toString(value);
}

Logger& operator<<(Logger& left, sf::Joystick::Axis value) {
	return left << thor::toString(value);
}

}
