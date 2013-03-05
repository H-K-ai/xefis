/* vim:ts=4
 *
 * Copyleft 2012…2013  Michał Gawron
 * Marduk Unix Labs, http://mulabs.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Visit http://www.gnu.org/licenses/gpl-3.0.html for more information on licensing.
 */

#ifndef XEFIS__CONFIG__TYPES_H__INCLUDED
#define XEFIS__CONFIG__TYPES_H__INCLUDED

// Standard:
#include <stdint.h>
#include <string>

// Qt:
#include <QtCore/QString>

// Lib:
#include <lib/si/all.h>


using namespace SI;

typedef float Knots;
typedef float KnotsPerSecond;
typedef float Feet;
typedef float FeetPerMinute;
typedef float Seconds;
typedef float InHg;


inline std::string
operator"" _str (const char* string, size_t)
{
    return std::string (string);
}


inline QString
operator"" _qstr (const char* string, size_t)
{
    return QString (string);
}

#endif

