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

#ifndef XEFIS__UTILITY__RESOURCE_H__INCLUDED
#define XEFIS__UTILITY__RESOURCE_H__INCLUDED

// Standard:
#include <cstddef>
#include <functional>

// Xefis:
#include <xefis/config/all.h>


namespace Xefis {

/**
 * Define an object which executes given function upon destruction.
 * Useful substitude for "finally" construct, nonexistent in C++.
 */
class Resource
{
  public:
	typedef std::function<void()> Callback;

  public:
	// Ctor
	Resource() noexcept = default;

	// Ctor
	Resource (Callback callback) noexcept;

	// Copy ctor
	Resource (Resource const&) = delete;

	// Move ctor
	Resource (Resource&& other) noexcept;

	// Copy operator
	Resource&
	operator= (Resource const&) = delete;

	// Move operator
	Resource&
	operator= (Resource&& other) noexcept;

	/**
	 * Assign new resource. Release previous resource.
	 */
	Resource&
	operator= (Callback callback) noexcept;

	/**
	 * Releases resouce (calls the callback).
	 */
	~Resource();

	/**
	 * Manually release resource, before this is deleted.
	 */
	void
	release();

	/**
	 * Forget about the resource (don't release it).
	 */
	void
	forget();

  private:
	Callback _callback;
};


inline
Resource::Resource (Callback callback) noexcept:
	_callback (callback)
{ }


inline Resource&
Resource::operator= (Resource&& other) noexcept
{
	release();
	_callback = other._callback;
	other.forget();
	return *this;
}


inline Resource&
Resource::operator= (Callback callback) noexcept
{
	release();
	_callback = callback;
	return *this;
}


inline
Resource::~Resource()
{
	release();
}


inline void
Resource::release()
{
	if (_callback)
		_callback();
	forget();
}


inline void
Resource::forget()
{
	_callback = nullptr;
}

} // namespace Xefis

#endif

