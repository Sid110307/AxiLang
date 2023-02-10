#pragma once

#include <iostream>
#include <vector>
#include <tuple>

// Courtesy of https://github.com/ignatz/pythonic/blob/master/pythonic/enumerate.h

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
