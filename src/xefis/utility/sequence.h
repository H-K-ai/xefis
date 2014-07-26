/* vim:ts=4
 *
 * Copyleft 2012…2014  Michał Gawron
 * Marduk Unix Labs, http://mulabs.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Visit http://www.gnu.org/licenses/gpl-3.0.html for more information on licensing.
 */

#ifndef XEFIS__UTILITY__SEQUENCE_H__INCLUDED
#define XEFIS__UTILITY__SEQUENCE_H__INCLUDED

// Standard:
#include <cstddef>
#include <stdexcept>
#include <map>

// Xefis:
#include <xefis/config/all.h>


namespace Xefis {

/**
 * Find two adjacent iterators a and b that satisfy a <= value && value <= b.
 * Result might be two identical iterators, or two end iterators.
 */
template<class ConstIterator, class Value, class Accessor>
	inline std::pair<ConstIterator, ConstIterator>
	extended_adjacent_find (ConstIterator begin, ConstIterator end, Value value, Accessor access)
	{
		typedef decltype (*begin) DerefIterator;

		if (begin == end)
			return { end, end };

		auto predicate = [&](DerefIterator a, DerefIterator b) {
			return access (a) <= value && value <= access (b);
		};

		ConstIterator it = std::adjacent_find (begin, end, predicate);

		if (it == end)
		{
			if (value < access (*begin))
				return { begin, begin };
			else
			{
				ConstIterator pre_end = end;
				--pre_end;
				return { pre_end, pre_end };
			}
		}
		else
		{
			ConstIterator ne = it;
			return { it, ++ne };
		}
	}


template<class Container, typename ConstIterator>
	inline typename Container::iterator
	remove_constness (Container& container, ConstIterator iterator)
	{
		return container.erase (iterator, iterator);
	}

} // namespace Xefis

#endif

