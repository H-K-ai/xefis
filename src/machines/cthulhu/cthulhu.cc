/* vim:ts=4
 *
 * Copyleft 2008…2013  Michał Gawron
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

// Xefis:
#include <xefis/config/all.h>
#include <xefis/core/module.h>
#include <xefis/core/property.h>
#include <xefis/modules/helpers/mixer.h>
#include <xefis/modules/io/chr_um6.h>
#include <xefis/modules/io/ets_airspeed.h>
#include <xefis/modules/io/gps.h>
#include <xefis/modules/io/joystick.h>
#include <xefis/modules/systems/adc.h>
#include <xefis/modules/systems/afcs.h>
#include <xefis/modules/systems/flaps_control.h>
#include <xefis/support/airframe/airframe.h>
#include <xefis/support/bus/i2c.h>
#include <xefis/support/bus/serial_port.h>
#include <xefis/support/devices/ht16k33.h>
#include <xefis/utility/qdom.h>

// Local:
#include "cthulhu.h"


class WarthogStick: public JoystickInput
{
  public:
	/*
	 * Output
	 */

	xf::PropertyOut<double>&	pitch_axis	= axis (3);
	xf::PropertyOut<double>&	roll_axis	= axis (4);
	xf::PropertyOut<bool>&		fire_button	= button (5);

  public:
	using JoystickInput::JoystickInput;
};


class MyLoop: public xf::ProcessingLoop
{
  public:
	using ProcessingLoop::ProcessingLoop;

	void
	execute_cycle() override
	{
		ProcessingLoop::execute_cycle();
	}
};


Cthulhu::Cthulhu (xf::Xefis* xefis):
	Machine (xefis)
{
	xf::Airframe airframe (xefis, xf::load_xml (QFile ("machines/cthulhu/xmls/airframe.xml")));

	auto* loop = make_processing_loop<MyLoop> (100_Hz);

	auto* flaps_control = loop->load_module<FlapsControl> (airframe);
	flaps_control->setting_angular_velocity = 2.5_deg / 1_s;
	flaps_control->setting_control_extents = { 0.0, 0.5 };
	auto* adc = loop->load_module<AirDataComputer> (&airframe);
	auto* afcs = loop->load_module<AFCS>();
	auto* mixer = loop->load_module<Mixer<si::Angle>> ("mixer");

	xf::SerialPort::Configuration chrum6_sp_config;
	chrum6_sp_config.set_device_path ("/dev/ttyS0");
	chrum6_sp_config.set_baud_rate (115200);
	chrum6_sp_config.set_data_bits (8);
	chrum6_sp_config.set_stop_bits (1);
	chrum6_sp_config.set_parity_bit (xf::SerialPort::Parity::None);

	xf::SerialPort chrum6_serial_port;
	chrum6_serial_port.set_configuration (chrum6_sp_config);

	//auto* chrum6 = loop->load_module<CHRUM6> (std::move (chrum6_serial_port), "chrum6");

	xf::i2c::Device i2c_device_for_ets_airspeed;
	i2c_device_for_ets_airspeed.bus().set_bus_number (10);
	i2c_device_for_ets_airspeed.set_address (xf::i2c::Address (0x75));

	auto* ets_airspeed = loop->load_module<ETSAirspeed> (std::move (i2c_device_for_ets_airspeed), "ets-airspeed");

	xf::SerialPort::Configuration gps_serial_config;
	gps_serial_config.set_device_path ("/dev/ttyS1");
	gps_serial_config.set_baud_rate (9600);
	gps_serial_config.set_data_bits (8);
	gps_serial_config.set_stop_bits (1);
	gps_serial_config.set_parity_bit (xf::SerialPort::Parity::None);
	gps_serial_config.set_hardware_flow_control (false);

	xf::SerialPort::Configuration xbee_config;
	xbee_config.set_device_path ("/dev/ttyS1");
	xbee_config.set_baud_rate (9600);
	xbee_config.set_data_bits (8);
	xbee_config.set_stop_bits (1);
	xbee_config.set_parity_bit (xf::SerialPort::Parity::None);
	xbee_config.set_hardware_flow_control (false);

	auto* gps = loop->load_module<GPS> (xefis->system(), gps_serial_config, "gps");

	QDomElement joystick_config = xf::load_xml (QFile ("machines/cthulhu/xmls/joystick-hotas-stick.xml"));
	QDomElement throttle_config = xf::load_xml (QFile ("machines/cthulhu/xmls/joystick-hotas-throttle.xml"));
	QDomElement pedals_config = xf::load_xml (QFile ("machines/cthulhu/xmls/joystick-saitek-pedals.xml"));

	auto* joystick_input = loop->load_module<WarthogStick> (joystick_config, "stick");
	auto* throttle_input = loop->load_module<JoystickInput> (throttle_config, "throttle");
	auto* pedals_input = loop->load_module<JoystickInput> (pedals_config, "pedals");

	loop->start();
}


void
Cthulhu::setup_ht16k33s()
{
	xf::i2c::Device i2c_device;
	i2c_device.bus().set_bus_number (10);
	i2c_device.set_address (xf::i2c::Address (0x11));

	xf::PropertyOut<bool> switch_prop { "switch-prop" };
	xf::PropertyOut<si::Angle> angle_prop { "angle-prop" };
	xf::PropertyOut<bool> led_prop { "led-prop" };

	xf::HT16K33 chip (std::move (i2c_device));
	chip.add_single_switch (switch_prop, 0, 1);
	chip.add_numeric_display<si::Angle, si::Degree> (angle_prop, xf::HT16K33::NumericDisplay<si::Angle, si::Degree>::DigitRows { 0, 1, 2, 3 });
	chip.add_single_led (led_prop, 0, 2);
}

