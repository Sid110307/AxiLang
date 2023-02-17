#pragma once

#include <iostream>
#include <experimental/source_location>

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
	std::string typeToCStr();
};

struct FileState
{
	std::vector<Token> tokens;
	std::vector<std::string> lines;
	std::vector<int> lineNums;
	std::vector<int> linePositions;

	bool isEmpty() const { return tokens.empty() || lines.empty() || lineNums.empty() || linePositions.empty(); }
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
		Iterator(typename T::iterator it, size_t counter = 0) : it(it), counter(counter) {}

		Iterator operator++()
		{
			return Iterator(++it, ++counter);
		}

		bool operator!=(Iterator other)
		{
			return it != other.it;
		}

		[[maybe_unused]] typename T::iterator::value_type item()
		{
			return *it;
		}

		valueType operator*()
		{
			return valueType{counter, *it};
		}

		[[maybe_unused]] size_t index()
		{
			return counter;
		}

	private:
		typename T::iterator it;
		size_t counter;
	};

	EnumerateImplementation(T &t) : container(t) {}

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

class String : public std::string
{
public:
	enum Case
	{
		Upper,
		Lower,
		Title
	};

	String() : std::string() {}
	[[maybe_unused]] String(const std::string &str) : std::string(str) {}
	[[maybe_unused]] String(const char *str) : std::string(str) {}

	String &lTrim()
	{
		erase(begin(), std::find_if(begin(), end(), [](int ch) { return !std::isspace(ch); }));
		return *this;
	}

	String &rTrim()
	{
		erase(std::find_if(rbegin(), rend(), [](int ch) { return !std::isspace(ch); }).base(), end());
		return *this;
	}

	String &trim()
	{
		return lTrim().rTrim();
	}

	String &sanitize()
	{
		erase(std::remove_if(begin(), end(), [](unsigned char c) { return !std::isprint(c) && c != '"' && c != ' '; }),
			  end());
		return *this;
	}

	String &changeCase(Case stringCase)
	{
		switch (stringCase)
		{
			case Upper:
			{
				std::transform(begin(), end(), begin(), [](unsigned char c) { return std::toupper(c); });
				break;
			}
			case Lower:
			{
				std::transform(begin(), end(), begin(), [](unsigned char c) { return std::tolower(c); });
				break;
			}
			case Title:
			{
				bool isSpace = true;
				for (auto &c: *this)
				{
					if (isSpace)
					{
						c = std::toupper(c);
						isSpace = false;
					} else c == ' ' ? isSpace = true : c = std::tolower(c);
				}
				break;
			}
		}

		return *this;
	}

	String &getCase(Case stringCase)
	{
		String copy = *this;
		return copy.changeCase(stringCase);
	}
};

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

	Log(Type type, String message, FileState fs = {}, bool shouldExitOnError = true,
		std::string functionName = std::experimental::source_location::current().function_name())
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
