#include "include/api.h"

AxiDraw::AxiDraw()
{
	Py_Initialize();

	if (!PyImport_ImportModule("pyaxidraw"))
	{
		Log(Log::Type::Fatal, "The library 'pyaxidraw' is not installed.\nPlease install it with "
							  "'\033]8;;https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip"
							  "\033\\pip install https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip\033]8;;\033\\'");
	}

	try
	{
		axiDraw = py::import("pyaxidraw.axidraw").attr("AxiDraw")();
	}
	catch (py::error_already_set &e)
	{
		PyErr_Print();

		Log(Log::Type::Fatal, "Could not initialize AxiDraw API.");
	}

	Log(Log::Type::Debug, "AxiDraw API initialized.");
}

#pragma region General

void AxiDraw::setAcceleration(double acceleration)
{
	axiDraw.attr("options").attr("accel") = acceleration;
	Log(Log::Type::Debug, "Set accel to " + std::to_string(acceleration) + ".");
}

void AxiDraw::setPenUpPosition(double position)
{
	axiDraw.attr("options").attr("pen_pos_up") = position;
	Log(Log::Type::Debug, "Set pen_pos_up to " + std::to_string(position) + ".");
}

void AxiDraw::setPenDownPosition(double position)
{
	axiDraw.attr("options").attr("pen_pos_down") = position;
	Log(Log::Type::Debug, "Set pen_pos_down to " + std::to_string(position) + ".");
}

void AxiDraw::setPenUpDelay(double delay)
{
	axiDraw.attr("options").attr("pen_delay_up") = delay;
	Log(Log::Type::Debug, "Set pen_delay_up to " + std::to_string(delay) + ".");
}

void AxiDraw::setPenDownDelay(double delay)
{
	axiDraw.attr("options").attr("pen_delay_down") = delay;
	Log(Log::Type::Debug, "Set pen_delay_down to " + std::to_string(delay) + ".");
}

void AxiDraw::setPenUpSpeed(double speed)
{
	axiDraw.attr("options").attr("speed_penup") = speed;
	Log(Log::Type::Debug, "Set speed_penup to " + std::to_string(speed) + ".");
}

void AxiDraw::setPenDownSpeed(double speed)
{
	axiDraw.attr("options").attr("speed_pendown") = speed;
	Log(Log::Type::Debug, "Set speed_pendown to " + std::to_string(speed) + ".");
}

void AxiDraw::setPenUpRate(double rate)
{
	axiDraw.attr("options").attr("pen_rate_raise") = rate;
	Log(Log::Type::Debug, "Set pen_rate_raise to " + std::to_string(rate) + ".");
}

void AxiDraw::setPenDownRate(double rate)
{
	axiDraw.attr("options").attr("pen_rate_lower") = rate;
	Log(Log::Type::Debug, "Set pen_rate_lower to " + std::to_string(rate) + ".");
}

void AxiDraw::setModel(int model)
{
	axiDraw.attr("options").attr("model") = model;
	Log(Log::Type::Debug, "Set model to " + std::to_string(model) + ".");
}

void AxiDraw::setPort(const std::string &port)
{
	axiDraw.attr("options").attr("port") = port != "auto" ? py::str(port) : py::object();
	Log(Log::Type::Debug, "Set port to " + port + ".");
}

std::string AxiDraw::getMode()
{
	std::string mode = py::extract<std::string>(axiDraw.attr("options").attr("mode"));
	Log(Log::Type::Debug, "Mode is " + mode + ".");

	return mode;
}

#pragma endregion
#pragma region Interactive

void AxiDraw::modeInteractive()
{
	axiDraw.attr("interactive")();
	Log(Log::Type::Debug, "Mode is set to interactive.");
}

void AxiDraw::setUnits(int units)
{
	axiDraw.attr("options").attr("units") = units;
	Log(Log::Type::Debug, "Set units to " + std::to_string(units) + ".");
}

void AxiDraw::connect()
{
	if (!axiDraw.attr("connect")()) Log(Log::Type::Fatal, "Could not connect to AxiDraw.");
	else Log(Log::Type::Debug, "Connected to AxiDraw.");
}

void AxiDraw::disconnect()
{
	axiDraw.attr("disconnect")();
	Log(Log::Type::Debug, "Disconnected from AxiDraw.");
}

void AxiDraw::updateOptions()
{
	axiDraw.attr("update")();
	Log(Log::Type::Debug, "Updated options.");
}

void AxiDraw::penUp()
{
	if (!axiDraw.attr("current_pen")())
	{
		axiDraw.attr("penup")();
		Log(Log::Type::Debug, "Pen is up.");
	}
}

void AxiDraw::penDown()
{
	if (axiDraw.attr("current_pen")())
	{
		axiDraw.attr("pendown")();
		Log(Log::Type::Debug, "Pen is down.");
	}
}

void AxiDraw::penToggle()
{
	axiDraw.attr("current_pen")() ? penDown() : penUp();
	Log(Log::Type::Debug, std::string("Pen toggled to ") + (axiDraw.attr("current_pen")() ? "down" : "up") + ".");
}

void AxiDraw::home()
{
	axiDraw.attr("moveto")(0, 0);
	Log(Log::Type::Debug, "Moved to home.");
}

void AxiDraw::goTo(double x, double y)
{
	axiDraw.attr("moveto")(x, y);
	Log(Log::Type::Debug, "Moved to (" + std::to_string(x) + ", " + std::to_string(y) + ").");
}

void AxiDraw::goToRelative(double x, double y)
{
	axiDraw.attr("move")(x, y);
	Log(Log::Type::Debug, "Moved to (" + std::to_string(x) + ", " + std::to_string(y) + ") relatively.");
}

void AxiDraw::draw(std::vector<std::pair<double, double>> path)
{
	for (auto point: path)
	{
		if (point == path.front())
		{
			axiDraw.attr("moveto")(point.first, point.second);
			Log(Log::Type::Debug,
				"Moved to (" + std::to_string(point.first) + ", " + std::to_string(point.second) + ").");
		} else
		{
			axiDraw.attr("lineto")(point.first, point.second);
			Log(Log::Type::Debug,
				"Drew line to (" + std::to_string(point.first) + ", " + std::to_string(point.second) + ").");
		}
	}
}

void AxiDraw::wait(double ms)
{
	axiDraw.attr("delay")(ms);
	Log(Log::Type::Debug, "Waited for " + std::to_string(ms) + " ms.");
}

std::pair<double, double> AxiDraw::getPosition()
{
	const py::object &pos = axiDraw.attr("current_pos");
	std::pair<double, double> position = {py::extract<double>(pos[0])(), py::extract<double>(pos[1])()};

	Log(Log::Type::Debug,
		"Current position is (" + std::to_string(position.first) + ", " + std::to_string(position.second) + ").");
	return position;
}

bool AxiDraw::getPen()
{
	bool pen = py::extract<bool>(axiDraw.attr("current_pen")());

	Log(Log::Type::Info, std::string("Pen status is ") + (pen ? "down" : "up") + ".");
	return pen;
}

#pragma endregion
#pragma region Plot

void AxiDraw::modePlot(const std::string &filename)
{
	axiDraw.attr("plot_setup")(filename);
	Log(Log::Type::Debug, "Mode is set to plot.");
}

void AxiDraw::runPlot()
{
	axiDraw.attr("plot_run")();
	Log(Log::Type::Debug, "Running plot.");
}

#pragma endregion
