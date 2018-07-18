/* vim:ts=4
 *
 * Copyleft 2012…2016  Michał Gawron
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
#include <QtWidgets/QBoxLayout>

// Xefis:
#include <xefis/config/all.h>
#include <xefis/core/machine.h>
#include <xefis/core/module.h>

// Local:
#include "configurator_widget.h"


namespace xf {

ConfiguratorWidget::ConfiguratorWidget (Machine& machine, QWidget* parent):
	QWidget (parent),
	_machine (machine)
{
	_module_configurator = new ModuleConfigurator (_machine, this);
	_data_recorder = new DataRecorder (this);

	_tabs = new QTabWidget (this);
	_tabs->addTab (_module_configurator, "Module &configuration");
	_tabs->addTab (_data_recorder, "&Data recorder");

	QVBoxLayout* layout = new QVBoxLayout (this);
	layout->setMargin (0);
	layout->setSpacing (WidgetSpacing);
	layout->addWidget (_tabs);
}

} // namespace xf

