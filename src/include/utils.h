#pragma once

#include "../include/lexer.h"

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

	Log(Type type, std::string message, FileState fs = {0})
	{
		if (!fs.tokens.empty())
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
				if (!fs.tokens.empty())
					std::cerr << "[\033[1;31mERROR\033[0m]: On line " << fs.lineNums[fs.lineNums.size() - 1] << ".\n  "
							  << fs.lines[fs.lines.size() - 1] << "\n  " << padding << "\033[1;31m" << carets << "\n  "
							  << message << "\033[0m" << std::endl;
				else std::cerr << "[\033[1;31mERROR\033[0m]: " << message << std::endl;
				exit(EXIT_FAILURE);
			}
			case Type::Warning:
			{
				if (!fs.tokens.empty())
					std::cerr << "[\033[1;33mWARNING\033[0m]: On line " << fs.lineNums[fs.lineNums.size() - 1]
							  << ".\n  " << fs.lines[fs.lines.size() - 1] << "\n  " << padding << "\033[1;33m" << carets
							  << "\n  " << message << "\033[0m" << std::endl;
				else std::cerr << "[\033[1;33mWARNING\033[0m]: " << message << std::endl;
				break;
			}
			case Type::Debug:
			{
				std::cout << "[\033[1;34mDEBUG\033[0m]: " << __FUNCTION__ << "(): " << message << std::endl;
				break;
			}
			case Type::Info:
			{
				std::cout << "[\033[1;36mINFO\033[0m]: " << message << std::endl;
				break;
			}
		}
	}

private:
	std::string padding, carets;
};
