/* vim:ts=4
 *
 * Copyleft 2008…2017  Michał Gawron
 * Marduk Unix Labs, http://mulabs.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Visit http://www.gnu.org/licenses/gpl-3.0.html for more information on licensing.
 */

#ifndef XEFIS__CONFIGS__CTHULHU_GCS__LOOP_H__INCLUDED
#define XEFIS__CONFIGS__CTHULHU_GCS__LOOP_H__INCLUDED

// Standard:
#include <cstddef>

// Xefis:
#include <xefis/config/all.h>
#include <xefis/core/v2/processing_loop.h>
#include <xefis/core/v2/machine.h>
#include <xefis/core/xefis.h>
#include <xefis/modules/instruments/adi.h>
#include <xefis/modules/instruments/hsi.h>
#include <xefis/modules/instruments/horizontal_trim.h>
#include <xefis/modules/io/link.h>
#include <xefis/modules/systems/adc.h>
#include <xefis/support/airframe/airframe.h>

// Cthulhu:
#include <configs/cthulhu_shared/link_io.h>
#include <configs/cthulhu_shared/link_protocol.h>

// Local:
#include "warthog_stick.h"


class Loop: public xf::ProcessingLoop
{
  public:
	/*
	 * Input modules
	 */

	WarthogStick*		joystick_input;
	JoystickInput*		throttle_input;
	JoystickInput*		pedals_input;

	/*
	 * Communication modules
	 */

	Link*				link_tx;
	Link*				link_rx;

	/*
	 * Computers
	 */

	AirDataComputer*	adc;

	/*
	 * Instruments
	 */

	ADI*				adi;
	HSI*				hsi_aux;
	HorizontalTrim*		horizontal_trim;

  public:
	// Ctor
	explicit
	Loop (xf::Machine*, xf::Xefis*);

  private:
	std::unique_ptr<xf::NavaidStorage>	_navaid_storage;
	std::unique_ptr<xf::Airframe>		_airframe;
	std::unique_ptr<xf::WorkPerformer>	_work_performer;
};

#endif
