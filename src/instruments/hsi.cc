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
#include <QtWidgets/QLayout>

// Xefis:
#include <xefis/config/all.h>
#include <xefis/application/services.h>
#include <xefis/utility/numeric.h>
#include <xefis/utility/text_painter.h>
#include <xefis/utility/qdom.h>

// Local:
#include "hsi.h"


HSI::HSI (QDomElement const& config, QWidget* parent):
	Instrument (parent)
{
	for (QDomElement& e: config)
	{
		if (e == "properties")
		{
			parse_properties (e, {
				{ "gs", _gs_kt, false },
				{ "tas", _tas_kt, false },
				{ "orientation-heading", _heading_deg, false },
				{ "autopilot-visible", _autopilot_visible, false },
				{ "track", _track_deg, false },
				{ "autopilot-setting-heading", _autopilot_heading_setting_deg, false },
				{ "position-latitude", _position_lat_deg, false },
				{ "position-longitude", _position_lng_deg, false },
				{ "position-sea-level-radius", _position_sea_level_radius_ft, false }
			});
		}
	}

	_hsi_widget = new HSIWidget (this);

	QVBoxLayout* layout = new QVBoxLayout (this);
	layout->setMargin (0);
	layout->setSpacing (0);
	layout->addWidget (_hsi_widget);
}


void
HSI::read()
{
	estimate_track();

	bool autopilot_visible = _autopilot_visible.valid() && *_autopilot_visible;

	_hsi_widget->set_heading_visible (_heading_deg.valid());
	if (_heading_deg.valid())
		_hsi_widget->set_heading (*_heading_deg);

	_hsi_widget->set_ap_heading_visible (autopilot_visible && _autopilot_heading_setting_deg.valid());
	if (_autopilot_heading_setting_deg.valid())
		_hsi_widget->set_ap_heading (*_autopilot_heading_setting_deg);

	_hsi_widget->set_track_visible (_track_deg.valid());
	if (_track_deg.valid())
		_hsi_widget->set_track (*_track_deg);

	_hsi_widget->set_ground_speed_visible (_gs_kt.valid());
	if (_gs_kt.valid())
		_hsi_widget->set_ground_speed (*_gs_kt);

	_hsi_widget->set_true_air_speed_visible (_tas_kt.valid());
	if (_tas_kt.valid())
		_hsi_widget->set_true_air_speed (*_tas_kt);
}


void
HSI::estimate_track()
{
	if (_position_lat_deg.is_singular() || _position_lng_deg.is_singular())
	{
		_hsi_widget->set_track_estimation_visible (false);
		return;
	}

	LatLng current_position (*_position_lat_deg, *_position_lng_deg);

	if (!_positions_valid)
	{
		std::fill (_positions.begin(), _positions.end(), current_position);
		_positions_valid = true;
	}

	// Estimate only if the distance between last and current positions is > 0.02nm.
	Miles epsilon = 0.02; // TODO _nm
	if (haversine_nm (_positions[0], current_position) > epsilon)
	{
		// Shift data in _positions:
		for (unsigned int i = _positions.size() - 1; i > 0; i--)
			_positions[i] = _positions[i - 1];
		_positions[0] = current_position;
	}

	double len10 = haversine_nm (_positions[1], _positions[0]);
	Degrees alpha = -180.0 + great_arcs_angle (_positions[2], _positions[1], _positions[0]);
	Degrees beta_per_mile = alpha / len10;

	if (!std::isinf (beta_per_mile) && !std::isnan (beta_per_mile))
	{
		bool visible = haversine_nm (_positions[2], _positions[0]) > 2.f * epsilon;
		if (visible)
			beta_per_mile = _track_estimation_smoother.process (beta_per_mile);

		_hsi_widget->set_range (5.f);
		_hsi_widget->set_track_estimation_visible (visible);
		_hsi_widget->set_track_estimation_lookahead (2.f);
		_hsi_widget->set_track_deviation (bound (beta_per_mile, -180.0, +180.0));
	}
}

