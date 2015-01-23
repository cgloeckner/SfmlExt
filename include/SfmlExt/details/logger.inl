namespace sfext {

template <typename T>
Logger& operator<<(Logger& left, sf::Vector2<T> const & value) {
	std::string tmp = "<" + std::to_string(value.x) + ","
		+ std::to_string(value.y) + ">";
	return left << tmp;
}

template <typename T>
Logger& operator<<(Logger& left, sf::Vector3<T> const & value) {
	std::string tmp = "<" + std::to_string(value.x) + ","
		+ std::to_string(value.y) + "," + std::to_string(value.z) + ">";
	return left << tmp;
}

template <typename T>
Logger& operator<<(Logger& left, sf::Rect<T> const & value) {
	std::string tmp = "<" + std::to_string(value.left) + ","
		+ std::to_string(value.top) + ";" + std::to_string(value.width)
		+ "x" + std::to_string(value.height) + ">";
	return left << tmp;
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, Logger&>::type
operator<<(Logger& left, T value) {
	return left << std::to_string(value);
}

}
