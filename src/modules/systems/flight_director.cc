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

// Standard:
#include <cstddef>

// Qt:
#include <QtXml/QDomElement>

// Xefis:
#include <xefis/config/all.h>
#include <xefis/utility/qdom.h>
#include <xefis/utility/numeric.h>

// Local:
#include "flight_director.h"


FlightDirector::FlightDirector (Xefis::ModuleManager* module_manager, QDomElement const& config):
	Module (module_manager),
	_magnetic_heading_pid (1.0, 0.1, 0.0, 0.0),
	_magnetic_track_pid (1.0, 0.1, 0.0, 0.0),
	_altitude_pid (1.0, 0.1, 0.0, 0.0),
	_vertical_speed_pid (1.0, 0.1, 0.0, 0.0),
	_fpa_pid (1.0, 0.1, 0.0, 0.0)
{
	for (QDomElement& e: config)
	{
		if (e == "properties")
		{
			parse_properties (e, {
				{ "enabled", _enabled, true },
				{ "lateral-mode", _lateral_mode, true },
				{ "vertical-mode", _vertical_mode, true },
				{ "pitch-limit", _pitch_limit, true },
				{ "roll-limit", _roll_limit, true },
				{ "selected-magnetic-heading", _selected_magnetic_heading, true },
				{ "selected-magnetic-track", _selected_magnetic_track, true },
				{ "selected-altitude", _selected_altitude, true },
				{ "selected-vertical-speed", _selected_vertical_speed, true },
				{ "selected-fpa", _selected_fpa, true },
				{ "measured-magnetic-heading", _measured_magnetic_heading, true },
				{ "measured-magnetic-track", _measured_magnetic_track, true },
				{ "measured-altitude", _measured_altitude, true },
				{ "measured-vertical-speed", _measured_vertical_speed, true },
				{ "measured-fpa", _measured_fpa, true },
				{ "output-pitch", _output_pitch, true },
				{ "output-roll", _output_roll, true },
				{ "vertical-mode-hint", _vertical_mode_hint, false },
				{ "lateral-mode-hint", _lateral_mode_hint, false }
			});
		}
	}

	for (auto* pid: { &_magnetic_heading_pid, &_magnetic_track_pid })
	{
		pid->set_i_limit ({ -0.05f, +0.05f });
		pid->set_winding (true);
	}
	for (auto* pid: { &_altitude_pid, &_vertical_speed_pid, &_fpa_pid })
		pid->set_i_limit ({ -0.05f, +0.05f });
}


void
FlightDirector::data_updated()
{
	// Don't process if dt is too small:
	_dt += update_dt();
	if (_dt < 0.005_s)
		return;

	if (*_enabled)
	{
		double const altitude_output_scale = 0.10;
		double const vertical_speed_output_scale = 0.01;
		double const rl = (*_roll_limit).deg();
		double const pl = (*_pitch_limit).deg();
		Range<float> roll_limit (-rl, +rl);
		Range<float> pitch_limit (-pl, +pl);

		_magnetic_heading_pid.set_target (renormalize ((*_selected_magnetic_heading).deg(), 0.f, 360.f, -1.f, +1.f));
		_magnetic_heading_pid.process (renormalize ((*_measured_magnetic_heading).deg(), 0.f, 360.f, -1.f, +1.f), _dt.s());

		_magnetic_track_pid.set_target (renormalize ((*_selected_magnetic_track).deg(), 0.f, 360.f, -1.f, +1.f));
		_magnetic_track_pid.process (renormalize ((*_measured_magnetic_track).deg(), 0.f, 360.f, -1.f, +1.f), _dt.s());

		_altitude_pid.set_target ((*_selected_altitude).ft());
		_altitude_pid.process ((*_measured_altitude).ft(), _dt.s());

		_vertical_speed_pid.set_target ((*_selected_vertical_speed).fpm());
		_vertical_speed_pid.process ((*_measured_vertical_speed).fpm(), _dt.s());

		_fpa_pid.set_target ((*_selected_fpa).deg());
		_fpa_pid.process ((*_measured_fpa).deg(), _dt.s());

		switch (static_cast<VerticalMode> (*_vertical_mode))
		{
			case VerticalDisabled:
				_output_pitch.write (0_deg);
				break;

			case AltitudeHold:
				_output_pitch.write (1_deg * limit<float> (altitude_output_scale * _altitude_pid.output(), pitch_limit));
				break;

			case VerticalSpeed:
				_output_pitch.write (1_deg * limit<float> (vertical_speed_output_scale * _vertical_speed_pid.output(), pitch_limit));
				break;

			case FlightPathAngle:
				_output_pitch.write (1_deg * limit<float> (_fpa_pid.output(), pitch_limit));
				break;
		}

		switch (static_cast<LateralMode> (*_lateral_mode))
		{
			case LateralDisabled:
				_output_roll.write (0_deg);
				break;

			case FollowHeading:
				_output_roll.write (1_deg * limit<float> (_magnetic_heading_pid.output() * 180.f, roll_limit));
				break;

			case FollowTrack:
				_output_roll.write (1_deg * limit<float> (_magnetic_track_pid.output() * 180.f, roll_limit));
				break;
		}
	}
	else
	{
		_output_pitch.write (0_deg);
		_output_roll.write (0_deg);
	}

	// Mode hints:
	if (!_vertical_mode_hint.is_singular())
	{
		switch (static_cast<VerticalMode> (*_vertical_mode))
		{
			case VerticalDisabled:
				_vertical_mode_hint.write ("");
				break;

			case AltitudeHold:
				_vertical_mode_hint.write ("ALT HOLD");
				break;

			case VerticalSpeed:
				_vertical_mode_hint.write ("V/SPD");
				break;

			case FlightPathAngle:
				_vertical_mode_hint.write ("FPA");
				break;
		}
	}

	if (!_lateral_mode_hint.is_singular())
	{
		switch (static_cast<LateralMode> (*_lateral_mode))
		{
			case LateralDisabled:
				_lateral_mode_hint.write ("");
				break;

			case FollowHeading:
				_lateral_mode_hint.write ("HDG HOLD");
				break;

			case FollowTrack:
				_lateral_mode_hint.write ("TRK HOLD");
				break;
		}
	}

	_dt = Time::epoch();
}
