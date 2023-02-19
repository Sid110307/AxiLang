#pragma once

#include <iostream>
#include <map>
#include <vector>

#include <boost/assert/source_location.hpp>

#pragma region DataStructures

struct Token
{
	enum Type
	{
		// General commands
		Mode,
		Opts,
		EndOpts,
		UOpts,
		EndUOpts,

		// Modes
		PlotMode,
		InteractiveMode,

		// General options
		Acceleration,
		PenUpPosition,
		PenDownPosition,
		PenUpDelay,
		PenDownDelay,
		PenUpSpeed,
		PenDownSpeed,
		PenUpRate,
		PenDownRate,
		Model,
		Port,

		// Interactive options
		Units,

		// Interactive commands
		Connect,
		Disconnect,
		PenUp,
		PenDown,
		PenToggle,
		Home,
		GoTo,
		GoToRelative,
		Draw,
		Wait,
		GetPos,
		GetPen,

		// Plot commands
		SetPlot,
		Plot,

		// Data types
		Number,
		String,

		// Other
		Unknown,
		EndOfFile
	};

	Type type;
	std::string value;

	Token(Type type, std::string value) : type(type), value(std::move(value)) {}
	[[nodiscard]] std::string typeToCStr() const;
};

struct FileState
{
	std::vector<Token> tokens;
	std::vector<std::string> lines;
	std::vector<int> lineNums;
	std::vector<int> linePositions;

	[[nodiscard]] bool isEmpty() const
	{
		return tokens.empty() || lines.empty() || lineNums.empty() || linePositions.empty();
	}
};

#pragma endregion
#pragma region Enumerator

// Enumerator implementation courtesy of https://github.com/ignatz/pythonic
template<typename T>
class EnumerateImplementation
{
public:
	struct Item
	{
		size_t index;
		typename T::value_type &item;
	};

	typedef Item valueType;

	struct Iterator
	{
		explicit Iterator(typename T::iterator it, size_t counter = 0) : it(it), counter(counter) {}

		Iterator operator++()
		{
			return Iterator(++it, ++counter);
		}

		bool operator!=(Iterator other)
		{
			return it != other.it;
		}

		typename T::iterator::value_type item()
		{
			return *it;
		}

		valueType operator*()
		{
			return valueType{counter, *it};
		}

		size_t index()
		{
			return counter;
		}

	private:
		typename T::iterator it;
		size_t counter;
	};

	explicit EnumerateImplementation(T &t) : container(t) {}

	Iterator begin()
	{
		return Iterator(container.begin());
	}

	Iterator end()
	{
		return Iterator(container.end());
	}

private:
	T &container;
};

template<typename T>
EnumerateImplementation<T> enumerate(T &t)
{
	return EnumerateImplementation<T>(t);
}

#pragma endregion
#pragma region StringUtils

inline std::string sanitize(std::string const &str)
{
	static const std::map<char, std::string> lookupTable = {
			{'\r',   "\\r"},
			{'\n',   "\\n"},
			{'\t',   "\\t"},
			{'\v',   "\\v"},
			{'\f',   "\\f"},
			{'\a',   "\\a"},
			{'\b',   "\\b"},
			{'\0',   "\\0"},
			{'\033', "\\033"},
			{'\\',   "\\\\"},
			{'\'',   "\\'"},
	};

	std::string cleanedText;
	cleanedText.reserve(str.size());

	for (char c: str)
	{
		auto it = lookupTable.find(c);

		if (it != lookupTable.end()) cleanedText += it->second;
		else if (std::isprint(static_cast<unsigned char>(c))) cleanedText += c;
		else cleanedText += "\\x" + std::to_string(static_cast<unsigned char>(c));
	}

	auto trim = [](std::string const &str)
	{
		std::string result = str;
		result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch) { return !std::isspace(ch); }).base(),
					 result.end());
		result.erase(result.begin(),
					 std::find_if(result.begin(), result.end(), [](int ch) { return !std::isspace(ch); }));

		return result;
	};

	return trim(cleanedText);
}

inline std::string toLower(std::string const &str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });

	return result;
}

#pragma endregion
#pragma region Logger

class Log
{
public:
	enum class Type
	{
		Fatal,
		Error,
		Warning,
		Debug,
		Info
	};

	Log(Type type, const std::string &message, FileState fs = {}, bool shouldExitOnError = true,
		const std::string &functionName = boost::source_location().function_name())
	{
		if (!fs.isEmpty())
		{
			padding = std::string(
					fs.linePositions[fs.linePositions.size() - 1] - fs.tokens[fs.tokens.size() - 1].value.length(),
					' ');
			carets = std::string(fs.tokens[fs.tokens.size() - 1].value.length(), '^');
		}

		switch (type)
		{
			case Type::Fatal:
			{
				std::cerr << "[\033[1;31mFATAL\033[0m]: " << message << std::endl;
				exit(EXIT_FAILURE);
			}
			case Type::Error:
			{
				if (!fs.isEmpty())
					std::cerr << "[\033[1;31mERROR\033[0m]: On line " << fs.lineNums[fs.lineNums.size() - 1] << ".\n  "
							  << fs.lines[fs.lines.size() - 1] << "\n  " << padding << "\033[1;31m" << carets << "\n  "
							  << message << "\033[0m" << std::endl;
				else std::cerr << "[\033[1;31mERROR\033[0m]: " << message << std::endl;

				if (shouldExitOnError) exit(EXIT_FAILURE);
				break;
			}
			case Type::Warning:
			{
				if (!fs.isEmpty())
					std::cerr << "[\033[1;33mWARNING\033[0m]: On line " << fs.lineNums[fs.lineNums.size() - 1]
							  << ".\n  " << fs.lines[fs.lines.size() - 1] << "\n  " << padding << "\033[1;33m" << carets
							  << "\n  " << message << "\033[0m" << std::endl;
				else std::cerr << "[\033[1;33mWARNING\033[0m]: " << message << std::endl;
				break;
			}
			case Type::Debug:
			{
				if (debug)
					std::cout << "[\033[1;34mDEBUG\033[0m]: " << message << " (\033[37m" << functionName << "()\033[0m)"
							  << std::endl;
				break;
			}
			case Type::Info:
			{
				std::cout << "[\033[1;36mINFO\033[0m]: " << message << std::endl;
				break;
			}
		}
	}

	void enableDebug()
	{
		debug = 1;
	}

private:
	std::string padding, carets;
	int debug = 0;
};

#pragma endregion
