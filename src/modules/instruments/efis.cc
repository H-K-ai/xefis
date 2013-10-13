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
#include <xefis/core/services.h>
#include <xefis/utility/numeric.h>
#include <xefis/utility/painter.h>
#include <xefis/utility/qdom.h>

// Local:
#include "efis.h"


XEFIS_REGISTER_MODULE_CLASS ("instruments/efis", EFIS);


EFIS::EFIS (Xefis::ModuleManager* module_manager, QDomElement const& config):
	Instrument (module_manager, config)
{
	for (QDomElement& e: config)
	{
		if (e == "settings")
		{
			parse_settings (e, {
				{ "speed-ladder.line-every", _speed_ladder_line_every, false },
				{ "speed-ladder.number-every", _speed_ladder_number_every, false },
				{ "speed-ladder.extent", _speed_ladder_extent, false },
				{ "speed-ladder.minimum", _speed_ladder_minimum, false },
				{ "speed-ladder.maximum", _speed_ladder_maximum, false },
				{ "altitude-ladder.line-every", _altitude_ladder_line_every, false },
				{ "altitude-ladder.number-every", _altitude_ladder_number_every, false },
				{ "altitude-ladder.emphasis-every", _altitude_ladder_emphasis_every, false },
				{ "altitude-ladder.bold-every", _altitude_ladder_bold_every, false },
				{ "altitude-ladder.extent", _altitude_ladder_extent, false },
				{ "altitude.landing.warning.hi", _altitude_landing_warning_hi, false },
				{ "altitude.landing.warning.lo", _altitude_landing_warning_lo, false },
			});
		}
		else if (e == "properties")
		{
			parse_properties (e, {
				{ "speed.ias.serviceable", _speed_ias_serviceable, false },
				{ "speed.ias", _speed_ias, false },
				{ "speed.ias.lookahead", _speed_ias_lookahead, false },
				{ "speed.ias.minimum", _speed_ias_minimum, false },
				{ "speed.ias.minimum-maneuver", _speed_ias_minimum_maneuver, false },
				{ "speed.ias.maximum-maneuver", _speed_ias_maximum_maneuver, false },
				{ "speed.ias.maximum", _speed_ias_maximum, false },
				{ "speed.mach", _speed_mach, false },
				{ "speed.v1", _speed_v1, false },
				{ "speed.vr", _speed_vr, false },
				{ "speed.vref", _speed_vref, false },
				{ "orientation.serviceable", _orientation_serviceable, false },
				{ "orientation.pitch", _orientation_pitch, false },
				{ "orientation.roll", _orientation_roll, false },
				{ "orientation.heading.magnetic", _orientation_heading_magnetic, false },
				{ "orientation.heading.true", _orientation_heading_true, false },
				{ "orientation.heading.numbers-visible", _orientation_heading_numbers_visible, false },
				{ "track.lateral.magnetic", _track_lateral_magnetic, false },
				{ "track.lateral.true", _track_lateral_true, false },
				{ "track.vertical", _track_vertical, false },
				{ "slip-skid", _slip_skid, false },
				{ "fpv.visible", _fpv_visible, false },
				{ "aoa.alpha", _aoa_alpha, false },
				{ "aoa.warning-threshold", _aoa_warning_threshold, false },
				{ "aoa.critical", _aoa_critical, false },
				{ "aoa.critical.visible", _aoa_critical_visible, false },
				{ "altitude.amsl.serviceable", _altitude_amsl_serviceable, false },
				{ "altitude.amsl", _altitude_amsl, false },
				{ "altitude.amsl.lookahead", _altitude_amsl_lookahead, false },
				{ "altitude.agl.serviceable", _altitude_agl_serviceable, false },
				{ "altitude.agl", _altitude_agl, false },
				{ "altitude.minimums.type", _altitude_minimums_type, false },
				{ "altitude.minimums.setting", _altitude_minimums_setting, false },
				{ "altitude.minimums.amsl", _altitude_minimums_amsl, false },
				{ "altitude.landing.amsl", _altitude_landing_amsl, false },
				{ "vertical-speed.serviceable", _vertical_speed_serviceable, false },
				{ "vertical-speed", _vertical_speed, false },
				{ "vertical-speed.variometer", _vertical_speed_variometer, false },
				{ "pressure.qnh", _pressure_qnh, false },
				{ "pressure.display-hpa", _pressure_display_hpa, false },
				{ "pressure.use-std", _pressure_use_std, false },
				{ "flight-director.serviceable", _flight_director_serviceable, false },
				{ "flight-director.cmd.visible", _flight_director_cmd_visible, false },
				{ "flight-director.cmd.altitude", _flight_director_cmd_altitude, false },
				{ "flight-director.cmd.altitude.acquired", _flight_director_cmd_altitude_acquired, false },
				{ "flight-director.cmd.ias", _flight_director_cmd_ias, false },
				{ "flight-director.cmd.vertical-speed", _flight_director_cmd_vertical_speed, false },
				{ "flight-director.guidance.visible", _flight_director_guidance_visible, false },
				{ "flight-director.guidance.pitch", _flight_director_guidance_pitch, false },
				{ "flight-director.guidance.roll", _flight_director_guidance_roll, false },
				{ "control-stick.visible", _control_stick_visible, false },
				{ "control-stick.pitch", _control_stick_pitch, false },
				{ "control-stick.roll", _control_stick_roll, false },
				{ "approach.reference-visible", _approach_reference_visible, false },
				{ "approach.type-hint", _approach_type_hint, false },
				{ "approach.localizer-id", _approach_localizer_id, false },
				{ "approach.dme-distance", _approach_dme_distance, false },
				{ "flight-path.deviation.vertical.serviceable", _flight_path_deviation_vertical_serviceable, false },
				{ "flight-path.deviation.vertical", _flight_path_deviation_vertical, false },
				{ "flight-path.deviation.lateral.serviceable", _flight_path_deviation_lateral_serviceable, false },
				{ "flight-path.deviation.lateral", _flight_path_deviation_lateral, false },
				{ "flight-path.deviation.use-ils-style", _flight_path_deviation_use_ils_style, false },
				{ "flight-mode.hint.visible", _flight_mode_hint_visible, false },
				{ "flight-mode.hint", _flight_mode_hint, false },
				{ "flight-mode.fma.visible", _flight_mode_fma_visible, false },
				{ "flight-mode.fma.speed-hint", _flight_mode_fma_speed_hint, false },
				{ "flight-mode.fma.speed-small-hint", _flight_mode_fma_speed_small_hint, false },
				{ "flight-mode.fma.lateral-hint", _flight_mode_fma_lateral_hint, false },
				{ "flight-mode.fma.lateral-small-hint", _flight_mode_fma_lateral_small_hint, false },
				{ "flight-mode.fma.vertical-hint", _flight_mode_fma_vertical_hint, false },
				{ "flight-mode.fma.vertical-small-hint", _flight_mode_fma_vertical_small_hint, false },
				{ "tcas.resolution-advisory.pitch.minimum", _tcas_resolution_advisory_pitch_minimum, false },
				{ "tcas.resolution-advisory.pitch.maximum", _tcas_resolution_advisory_pitch_maximum, false },
				{ "tcas.resolution-advisory.vertical-speed.minimum", _tcas_resolution_advisory_vertical_speed_minimum, false },
				{ "tcas.resolution-advisory.vertical-speed.maximum", _tcas_resolution_advisory_vertical_speed_maximum, false },
				{ "warning.novspd-flag", _warning_novspd_flag, false },
				{ "warning.ldgalt-flag", _warning_ldgalt_flag, false },
				{ "warning.pitch-disagree", _warning_pitch_disagree, false },
				{ "warning.roll-disagree", _warning_roll_disagree, false },
				{ "warning.ias-disagree", _warning_ias_disagree, false },
				{ "warning.altitude-disagree", _warning_altitude_disagree, false },
				{ "warning.roll", _warning_roll, false },
				{ "warning.slip-skid", _warning_slip_skid, false },
				{ "style.old", _style_old, false },
				{ "style.show-metric", _style_show_metric, false },
			});
		}
	}

	_efis_widget = new EFISWidget (this, work_performer());

	QVBoxLayout* layout = new QVBoxLayout (this);
	layout->setMargin (0);
	layout->setSpacing (0);
	layout->addWidget (_efis_widget);

	_fpv_computer.set_callback (std::bind (&EFIS::compute_fpv, this));
	_fpv_computer.observe ({
		&_orientation_heading_magnetic,
		&_orientation_heading_true,
		&_orientation_pitch,
		&_orientation_roll,
		&_track_lateral_magnetic,
		&_track_lateral_true,
		&_track_vertical,
		&_fpv_visible,
	});
}


void
EFIS::read()
{
	_fpv_computer.data_updated (update_time());

	_efis_widget->set_speed_ladder_line_every (_speed_ladder_line_every);
	_efis_widget->set_speed_ladder_number_every (_speed_ladder_number_every);
	_efis_widget->set_speed_ladder_extent (_speed_ladder_extent);
	_efis_widget->set_speed_ladder_minimum (_speed_ladder_minimum);
	_efis_widget->set_speed_ladder_maximum (_speed_ladder_maximum);

	_efis_widget->set_altitude_ladder_line_every (_altitude_ladder_line_every);
	_efis_widget->set_altitude_ladder_number_every (_altitude_ladder_number_every);
	_efis_widget->set_altitude_ladder_emphasis_every (_altitude_ladder_emphasis_every);
	_efis_widget->set_altitude_ladder_bold_every (_altitude_ladder_bold_every);
	_efis_widget->set_altitude_ladder_extent (_altitude_ladder_extent);

	_efis_widget->set_landing_altitude_warning_hi (_altitude_landing_warning_hi);
	_efis_widget->set_landing_altitude_warning_lo (_altitude_landing_warning_lo);

	_efis_widget->set_heading_numbers_visible (_orientation_heading_numbers_visible.read (false));

	_efis_widget->set_speed_visible (_speed_ias.valid());
	_efis_widget->set_speed_failure (!_speed_ias_serviceable.read (true));
	if (_speed_ias.valid())
		_efis_widget->set_speed (*_speed_ias);

	_efis_widget->set_speed_tendency_visible (_speed_ias_lookahead.valid());
	if (_speed_ias_lookahead.valid())
		_efis_widget->set_speed_tendency (*_speed_ias_lookahead);

	_efis_widget->set_minimum_speed_visible (_speed_ias_minimum.valid());
	if (_speed_ias_minimum.valid())
		_efis_widget->set_minimum_speed (*_speed_ias_minimum);

	_efis_widget->set_minimum_maneuver_speed_visible (_speed_ias_minimum_maneuver.valid());
	if (_speed_ias_minimum_maneuver.valid())
		_efis_widget->set_minimum_maneuver_speed (*_speed_ias_minimum_maneuver);

	_efis_widget->set_maximum_maneuver_speed_visible (_speed_ias_maximum_maneuver.valid());
	if (_speed_ias_maximum_maneuver.valid())
		_efis_widget->set_maximum_maneuver_speed (*_speed_ias_maximum_maneuver);

	_efis_widget->set_maximum_speed_visible (_speed_ias_maximum.valid());
	if (_speed_ias_maximum.valid())
		_efis_widget->set_maximum_speed (*_speed_ias_maximum);

	_efis_widget->set_mach_visible (_speed_mach.valid());
	if (_speed_mach.valid())
		_efis_widget->set_mach (*_speed_mach);

	_efis_widget->set_pitch_visible (_orientation_pitch.valid());
	if (_orientation_pitch.valid())
		_efis_widget->set_pitch (*_orientation_pitch);

	_efis_widget->set_roll_visible (_orientation_roll.valid());
	if (_orientation_roll.valid())
		_efis_widget->set_roll (*_orientation_roll);

	_efis_widget->set_attitude_failure (!_orientation_serviceable.read (true));

	if (_aoa_alpha.valid() && _aoa_critical.valid() && _aoa_critical_visible.read (false))
	{
		_efis_widget->set_aoa_alpha (*_aoa_alpha);
		_efis_widget->set_critical_aoa (*_aoa_critical);
		if (_aoa_warning_threshold.valid() && _aoa_alpha.valid() && _aoa_critical.valid())
			_efis_widget->set_critical_aoa_visible (*_aoa_critical - *_aoa_alpha <= *_aoa_warning_threshold);
		else
			_efis_widget->set_critical_aoa_visible (false);
	}
	else
		_efis_widget->set_critical_aoa_visible (false);

	_efis_widget->set_heading_visible (_orientation_heading_magnetic.valid());
	if (_orientation_heading_magnetic.valid())
		_efis_widget->set_heading (*_orientation_heading_magnetic);

	_efis_widget->set_slip_skid_visible (_slip_skid.valid());
	if (_slip_skid.valid())
		_efis_widget->set_slip_skid (*_slip_skid);

	_efis_widget->set_altitude_visible (_altitude_amsl.valid());
	_efis_widget->set_altitude_failure (!_altitude_amsl_serviceable.read (true));
	if (_altitude_amsl.valid())
		_efis_widget->set_altitude (*_altitude_amsl);

	_efis_widget->set_altitude_tendency_visible (_altitude_amsl_lookahead.valid());
	if (_altitude_amsl_lookahead.valid())
		_efis_widget->set_altitude_tendency (*_altitude_amsl_lookahead);

	_efis_widget->set_altitude_agl_visible (_altitude_agl.valid());
	_efis_widget->set_radar_altimeter_failure (!_altitude_agl_serviceable.read (true));
	if (_altitude_agl.valid())
		_efis_widget->set_altitude_agl (*_altitude_agl);

	if (_altitude_minimums_setting.valid())
		_efis_widget->set_altitude_minimums_setting (*_altitude_minimums_setting);
	if (_altitude_minimums_amsl.valid())
		_efis_widget->set_altitude_minimums_amsl (*_altitude_minimums_amsl);
	_efis_widget->set_minimums_altitude_visible (_altitude_minimums_setting.valid() && _altitude_minimums_amsl.valid());

	if (_altitude_landing_amsl.valid())
		_efis_widget->set_landing_altitude_amsl (*_altitude_landing_amsl);
	_efis_widget->set_landing_altitude_visible (_altitude_landing_amsl.valid());

	if (_pressure_use_std.valid())
		_efis_widget->set_standard_pressure (*_pressure_use_std);
	else
		_efis_widget->set_standard_pressure (false);

	_efis_widget->set_minimums_type (QString::fromStdString (_altitude_minimums_type.read ("")));

	_efis_widget->set_pressure_visible (_pressure_qnh.valid());
	if (_pressure_qnh.valid())
		_efis_widget->set_pressure (*_pressure_qnh);

	if (_pressure_display_hpa.valid())
		_efis_widget->set_pressure_display_hpa (*_pressure_display_hpa);

	_efis_widget->set_vertical_speed_visible (_vertical_speed.valid());
	_efis_widget->set_vertical_speed_failure (!_vertical_speed_serviceable.read (true));
	if (_vertical_speed.valid())
		_efis_widget->set_vertical_speed (*_vertical_speed);

	_efis_widget->set_variometer_visible (_vertical_speed_variometer.valid());
	if (_vertical_speed_variometer.valid())
		_efis_widget->set_variometer_rate (*_vertical_speed_variometer);

	bool cmd_visible = _flight_director_cmd_visible.read (false);

	_efis_widget->set_cmd_altitude_visible (cmd_visible && _flight_director_cmd_altitude.valid());
	if (_flight_director_cmd_altitude.valid())
		_efis_widget->set_cmd_altitude (*_flight_director_cmd_altitude);
	_efis_widget->set_cmd_altitude_acquired (_flight_director_cmd_altitude_acquired.valid() && *_flight_director_cmd_altitude_acquired);

	_efis_widget->set_cmd_speed_visible (cmd_visible && _flight_director_cmd_ias.valid());
	if (_flight_director_cmd_ias.valid())
		_efis_widget->set_cmd_speed (*_flight_director_cmd_ias);

	_efis_widget->set_cmd_vertical_speed_visible (cmd_visible && _flight_director_cmd_vertical_speed.valid());
	if (_flight_director_cmd_vertical_speed.valid())
		_efis_widget->set_cmd_vertical_speed (*_flight_director_cmd_vertical_speed);

	bool flight_director_guidance_visible = _flight_director_guidance_visible.read (false);

	_efis_widget->set_flight_director_failure (!_flight_director_serviceable.read (true));

	_efis_widget->set_flight_director_pitch_visible (flight_director_guidance_visible && _flight_director_guidance_pitch.valid());
	if (_flight_director_guidance_pitch.valid())
		_efis_widget->set_flight_director_pitch (*_flight_director_guidance_pitch);

	_efis_widget->set_flight_director_roll_visible (flight_director_guidance_visible && _flight_director_guidance_roll.valid());
	if (_flight_director_guidance_roll.valid())
		_efis_widget->set_flight_director_roll (*_flight_director_guidance_roll);

	bool control_stick_visible = _control_stick_visible.valid() && *_control_stick_visible;

	_efis_widget->set_control_stick_visible (control_stick_visible && _control_stick_pitch.valid() && _control_stick_roll.valid());

	if (_control_stick_pitch.valid())
		_efis_widget->set_control_stick_pitch (*_control_stick_pitch);

	if (_control_stick_roll.valid())
		_efis_widget->set_control_stick_roll (*_control_stick_roll);

	if (_approach_reference_visible.valid() && *_approach_reference_visible)
	{
		_efis_widget->set_approach_hint (_approach_type_hint.read ("").c_str());
		_efis_widget->set_approach_reference_visible (true);
		if (_altitude_agl.valid())
		{
			_efis_widget->set_runway_visible (_flight_path_deviation_lateral.valid() && *_altitude_agl <= 1000_ft);
			_efis_widget->set_runway_position (Xefis::limit<Length> (*_altitude_agl, 0_ft, 250_ft) / 250_ft * 25_deg);
		}
	}
	else
	{
		_efis_widget->set_approach_reference_visible (false);
		_efis_widget->set_approach_hint ("");
	}

	_efis_widget->set_dme_distance_visible (_approach_dme_distance.valid());
	if (_approach_dme_distance.valid())
		_efis_widget->set_dme_distance (*_approach_dme_distance);

	if (_approach_localizer_id.valid() && _orientation_heading_true.valid() && _orientation_heading_magnetic.valid())
	{
		Xefis::Navaid const* navaid = navaid_storage()->find_by_id (Xefis::Navaid::LOC, (*_approach_localizer_id).c_str());
		if (navaid)
		{
			_efis_widget->set_localizer_id ((*_approach_localizer_id).c_str());
			_efis_widget->set_localizer_magnetic_bearing (*_orientation_heading_magnetic - *_orientation_heading_true + navaid->true_bearing());
		}
		else
		{
			_efis_widget->set_localizer_id ((*_approach_localizer_id).c_str());
			_efis_widget->set_localizer_magnetic_bearing (0_deg);
		}
		_efis_widget->set_localizer_info_visible (true);
	}
	else
		_efis_widget->set_localizer_info_visible (false);

	_efis_widget->set_novspd_flag (_warning_novspd_flag.read (false));
	_efis_widget->set_ldgalt_flag (_warning_ldgalt_flag.read (false));

	if (_speed_v1.valid())
		_efis_widget->add_speed_bug ("V1", *_speed_v1);
	else
		_efis_widget->remove_speed_bug ("V1");

	if (_speed_vr.valid())
		_efis_widget->add_speed_bug ("VR", *_speed_vr);
	else
		_efis_widget->remove_speed_bug ("VR");

	if (_speed_vref.valid())
		_efis_widget->add_speed_bug ("REF", *_speed_vref);
	else
		_efis_widget->remove_speed_bug ("REF");

	_efis_widget->set_vertical_deviation_visible (_flight_path_deviation_vertical.valid());
	if (_flight_path_deviation_vertical.valid())
		_efis_widget->set_vertical_deviation (*_flight_path_deviation_vertical);

	_efis_widget->set_vertical_deviation_failure (!_flight_path_deviation_vertical_serviceable.read (true));

	_efis_widget->set_lateral_deviation_visible (_flight_path_deviation_lateral.valid());
	if (_flight_path_deviation_lateral.valid())
		_efis_widget->set_lateral_deviation (*_flight_path_deviation_lateral);

	_efis_widget->set_lateral_deviation_failure (!_flight_path_deviation_lateral_serviceable.read (true));

	_efis_widget->set_deviation_uses_ils_style (_flight_path_deviation_use_ils_style.read (false));

	_efis_widget->set_control_hint_visible (_flight_mode_hint_visible.read (false));
	_efis_widget->set_control_hint (_flight_mode_hint.read ("").c_str());

	_efis_widget->set_fma_visible (_flight_mode_fma_visible.read (false));
	_efis_widget->set_fma_speed_hint (_flight_mode_fma_speed_hint.read ("").c_str());
	_efis_widget->set_fma_speed_small_hint (_flight_mode_fma_speed_small_hint.read ("").c_str());
	_efis_widget->set_fma_lateral_hint (_flight_mode_fma_lateral_hint.read ("").c_str());
	_efis_widget->set_fma_lateral_small_hint (_flight_mode_fma_lateral_small_hint.read ("").c_str());
	_efis_widget->set_fma_vertical_hint (_flight_mode_fma_vertical_hint.read ("").c_str());
	_efis_widget->set_fma_vertical_small_hint (_flight_mode_fma_vertical_small_hint.read ("").c_str());

	_efis_widget->set_tcas_ra_pitch_minimum (_tcas_resolution_advisory_pitch_minimum.get_optional());
	_efis_widget->set_tcas_ra_pitch_maximum (_tcas_resolution_advisory_pitch_maximum.get_optional());
	_efis_widget->set_tcas_ra_vertical_speed_minimum (_tcas_resolution_advisory_vertical_speed_minimum.get_optional());
	_efis_widget->set_tcas_ra_vertical_speed_maximum (_tcas_resolution_advisory_vertical_speed_maximum.get_optional());

	_efis_widget->set_pitch_disagree (_warning_pitch_disagree.read (false));
	_efis_widget->set_roll_disagree (_warning_roll_disagree.read (false));
	_efis_widget->set_ias_disagree (_warning_ias_disagree.read (false));
	_efis_widget->set_altitude_disagree (_warning_altitude_disagree.read (false));
	_efis_widget->set_roll_warning (_warning_roll.read (false));
	_efis_widget->set_slip_skid_warning (_warning_slip_skid.read (false));

	_efis_widget->set_old_style (_style_old.read (false));
	_efis_widget->set_show_metric (_style_show_metric.read (false));
}


void
EFIS::compute_fpv()
{
	Xefis::PropertyAngle* heading = nullptr;
	Xefis::PropertyAngle* track_lateral = nullptr;
	if (_orientation_heading_magnetic.valid() && _track_lateral_magnetic.valid())
	{
		heading = &_orientation_heading_magnetic;
		track_lateral = &_track_lateral_magnetic;
	}
	else if (_orientation_heading_true.valid() && _track_lateral_true.valid())
	{
		heading = &_orientation_heading_true;
		track_lateral = &_track_lateral_true;
	}

	if (_orientation_pitch.valid() && _orientation_roll.valid() && _track_vertical.valid() && heading && track_lateral)
	{
		Angle vdiff = Xefis::floored_mod (*_orientation_pitch - *_track_vertical, -180_deg, +180_deg);
		Angle hdiff = Xefis::floored_mod (**heading - **track_lateral, -180_deg, +180_deg);
		Angle roll = *_orientation_roll;

		Angle alpha = vdiff * std::cos (roll) + hdiff * std::sin (roll);
		Angle beta = -vdiff * std::sin (roll) + hdiff * std::cos (roll);

		_efis_widget->set_flight_path_alpha (alpha);
		_efis_widget->set_flight_path_beta (beta);
		_efis_widget->set_flight_path_marker_visible (_fpv_visible.read (false));
		_efis_widget->set_flight_path_marker_failure (false);
	}
	else
	{
		_efis_widget->set_flight_path_marker_visible (false);
		_efis_widget->set_flight_path_marker_failure (_fpv_visible.read (false));
	}
}

