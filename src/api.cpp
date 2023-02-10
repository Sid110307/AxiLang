#include "include/api.h"

AxiDraw::AxiDraw()
{
	Py_Initialize();

	if (!PyImport_ImportModule("pyaxidraw"))
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: The library 'pyaxidraw' is not installed.\n"
				  << "Please install it with '\033]8;;https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip"
				  << "\033\\pip install https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip\033]8;;\033\\'"
				  << std::endl;
		exit(EXIT_FAILURE);
	}

	try
	{
		axiDraw = py::import("pyaxidraw.axidraw").attr("AxiDraw")();
	}
	catch (py::error_already_set &e)
	{
		PyErr_Print();

		std::cerr << "[\033[1;31mERROR\033[0m]: Could not initialize AxiDraw API." << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "[\033[1;30mDEBUG\033[0m]: AxiDraw API initialized." << std::endl;
}

#pragma region General

void AxiDraw::setAcceleration(double acceleration)
{
	axiDraw.attr("options").attr("accel") = acceleration;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set accel to " << acceleration << "." << std::endl;
}

void AxiDraw::setPenUpPosition(double position)
{
	axiDraw.attr("options").attr("pen_pos_up") = position;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set pen_pos_up to " << position << "." << std::endl;
}

void AxiDraw::setPenDownPosition(double position)
{
	axiDraw.attr("options").attr("pen_pos_down") = position;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set pen_pos_down to " << position << "." << std::endl;
}

void AxiDraw::setPenUpDelay(double delay)
{
	axiDraw.attr("options").attr("pen_delay_up") = delay;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set pen_delay_up to " << delay << "." << std::endl;
}

void AxiDraw::setPenDownDelay(double delay)
{
	axiDraw.attr("options").attr("pen_delay_down") = delay;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set pen_delay_down to " << delay << "." << std::endl;
}

void AxiDraw::setPenUpSpeed(double speed)
{
	axiDraw.attr("options").attr("speed_penup") = speed;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set speed_penup to " << speed << "." << std::endl;
}

void AxiDraw::setPenDownSpeed(double speed)
{
	axiDraw.attr("options").attr("speed_pendown") = speed;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set speed_pendown to " << speed << "." << std::endl;
}

void AxiDraw::setPenUpRate(double rate)
{
	axiDraw.attr("options").attr("pen_rate_raise") = rate;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set pen_rate_raise to " << rate << "." << std::endl;
}

void AxiDraw::setPenDownRate(double rate)
{
	axiDraw.attr("options").attr("pen_rate_lower") = rate;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set pen_rate_lower to " << rate << "." << std::endl;
}

void AxiDraw::setModel(int model)
{
	axiDraw.attr("options").attr("model") = model;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set model to " << model << "." << std::endl;
}

void AxiDraw::setPort(const std::string &port)
{
	axiDraw.attr("options").attr("port") = port == "auto" ? py::object() : py::str(port);
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set port to " << port << "." << std::endl;
}

std::string AxiDraw::getMode()
{
	std::string mode = py::extract<std::string>(axiDraw.attr("options").attr("mode"));

	std::cout << "[\033[1;30mDEBUG\033[0m]: Mode is " << mode << "." << std::endl;
	return mode;
}

#pragma endregion
#pragma region Interactive

void AxiDraw::modeInteractive()
{
	axiDraw.attr("interactive")();
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set mode to interactive." << std::endl;
}

void AxiDraw::setUnits(int units)
{
	axiDraw.attr("options").attr("units") = units;
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set units to " << units << "." << std::endl;
}

void AxiDraw::connect()
{
	if (!axiDraw.attr("connect")()) std::cerr << "[\033[1;31mERROR\033[0m]: Could not connect to AxiDraw!" << std::endl;
	else std::cout << "[\033[1;30mDEBUG\033[0m]: Connected to AxiDraw." << std::endl;
}

void AxiDraw::disconnect()
{
	axiDraw.attr("disconnect")();
	std::cout << "[\033[1;30mDEBUG\033[0m]: Disconnected from AxiDraw." << std::endl;
}

void AxiDraw::updateOptions()
{
	axiDraw.attr("update")();

	std::cout << "[\033[1;30mDEBUG\033[0m]: Updated options." << std::endl;
}

void AxiDraw::penUp()
{
	if (!axiDraw.attr("current_pen")())
	{
		axiDraw.attr("penup")();
		std::cout << "[\033[1;30mDEBUG\033[0m]: Pen is up." << std::endl;
	}
}

void AxiDraw::penDown()
{
	if (axiDraw.attr("current_pen")())
	{
		axiDraw.attr("pendown")();
		std::cout << "[\033[1;30mDEBUG\033[0m]: Pen is down." << std::endl;
	}
}

void AxiDraw::penToggle()
{
	axiDraw.attr("current_pen")() ? penDown() : penUp();
	std::cout << "[\033[1;30mDEBUG\033[0m]: Pen toggled to " << (axiDraw.attr("current_pen")() ? "down" : "up") << "."
			  << std::endl;
}

void AxiDraw::home()
{
	axiDraw.attr("moveto")(0, 0);
	std::cout << "[\033[1;30mDEBUG\033[0m]: Moved to home." << std::endl;
}

void AxiDraw::goTo(double x, double y)
{
	axiDraw.attr("moveto")(x, y);
	std::cout << "[\033[1;30mDEBUG\033[0m]: Moved to (" << x << ", " << y << ")." << std::endl;
}

void AxiDraw::goToRelative(double x, double y)
{
	axiDraw.attr("move")(x, y);
	std::cout << "[\033[1;30mDEBUG\033[0m]: Moved to (" << x << ", " << y << ") relative to current position."
			  << std::endl;
}

void AxiDraw::draw(std::vector<std::pair<double, double>> path)
{
	for (auto point: path)
	{
		if (point == path.front())
		{
			axiDraw.attr("moveto")(point.first, point.second);
			std::cout << "[\033[1;30mDEBUG\033[0m]: Moved to (" << point.first << ", " << point.second << ")."
					  << std::endl;
		} else
		{
			axiDraw.attr("lineto")(point.first, point.second);
			std::cout << "[\033[1;30mDEBUG\033[0m]: Drew line to (" << point.first << ", " << point.second << ")."
					  << std::endl;
		}
	}
}

void AxiDraw::wait(double ms)
{
	axiDraw.attr("delay")(ms);
	std::cout << "[\033[1;30mDEBUG\033[0m]: Waited for " << ms << " ms." << std::endl;
}

std::pair<double, double> AxiDraw::getPosition()
{
	const py::object &pos = axiDraw.attr("current_pos");
	std::pair<double, double> position = {py::extract<double>(pos[0])(), py::extract<double>(pos[1])()};

	std::cout << "[\033[1;30mDEBUG\033[0m]: Current position is (" << position.first << ", " << position.second << ")."
			  << std::endl;
	return position;
}

bool AxiDraw::getPen()
{
	bool pen = py::extract<bool>(axiDraw.attr("current_pen")());

	std::cout << "[\033[1;30mDEBUG\033[0m]: Pen status is " << (pen ? "down" : "up") << "." << std::endl;
	return pen;
}

#pragma endregion
#pragma region Plot

void AxiDraw::modePlot(const std::string &filename)
{
	axiDraw.attr("plot_setup")(filename);
	std::cout << "[\033[1;30mDEBUG\033[0m]: Set mode to plot." << std::endl;
}

void AxiDraw::runPlot()
{
	axiDraw.attr("plot_run")();
	std::cout << "[\033[1;30mDEBUG\033[0m]: Ran plot." << std::endl;
}

#pragma endregion
