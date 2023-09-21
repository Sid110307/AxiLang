#include "include/api.h"

AxiDraw::AxiDraw()
{
    Py_Initialize();
    std::string version = boost::python::extract<std::string>(boost::python::import("platform").attr(
            "python_version")());

    Log(Log::Type::DEBUG, "Using Python " + version + " (from " + PYTHON_EXECUTABLE + ").");

    if (!PyImport_ImportModule("pyaxidraw"))
    {
        Log(Log::Type::FATAL, "The library \"pyaxidraw\" is not installed. Please install it with "
                              "'\033]8;;https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip"
                              "\033\\pip install https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip\033]8;;\033\\'");
    }

    try
    {
        axiDraw = boost::python::import("pyaxidraw.axidraw").attr("AxiDraw")();
        Log(Log::Type::DEBUG, "AxiDraw API initialized.");
    }
    catch (boost::python::error_already_set &e)
    {
        PyErr_Print();
        Log(Log::Type::FATAL, "Could not initialize AxiDraw API.");
    }
}

#pragma region General

void AxiDraw::setAcceleration(double acceleration)
{
    axiDraw.attr("options").attr("accel") = acceleration;
    Log(Log::Type::DEBUG, "Set accel to " + std::to_string(acceleration) + ".");
}

void AxiDraw::setPenUpPosition(double position)
{
    axiDraw.attr("options").attr("pen_pos_up") = position;
    Log(Log::Type::DEBUG, "Set pen_pos_up to " + std::to_string(position) + ".");
}

void AxiDraw::setPenDownPosition(double position)
{
    axiDraw.attr("options").attr("pen_pos_down") = position;
    Log(Log::Type::DEBUG, "Set pen_pos_down to " + std::to_string(position) + ".");
}

void AxiDraw::setPenUpDelay(double delay)
{
    axiDraw.attr("options").attr("pen_delay_up") = delay;
    Log(Log::Type::DEBUG, "Set pen_delay_up to " + std::to_string(delay) + ".");
}

void AxiDraw::setPenDownDelay(double delay)
{
    axiDraw.attr("options").attr("pen_delay_down") = delay;
    Log(Log::Type::DEBUG, "Set pen_delay_down to " + std::to_string(delay) + ".");
}

void AxiDraw::setPenUpSpeed(double speed)
{
    axiDraw.attr("options").attr("speed_penup") = speed;
    Log(Log::Type::DEBUG, "Set speed_penup to " + std::to_string(speed) + ".");
}

void AxiDraw::setPenDownSpeed(double speed)
{
    axiDraw.attr("options").attr("speed_pendown") = speed;
    Log(Log::Type::DEBUG, "Set speed_pendown to " + std::to_string(speed) + ".");
}

void AxiDraw::setPenUpRate(double rate)
{
    axiDraw.attr("options").attr("pen_rate_raise") = rate;
    Log(Log::Type::DEBUG, "Set pen_rate_raise to " + std::to_string(rate) + ".");
}

void AxiDraw::setPenDownRate(double rate)
{
    axiDraw.attr("options").attr("pen_rate_lower") = rate;
    Log(Log::Type::DEBUG, "Set pen_rate_lower to " + std::to_string(rate) + ".");
}

void AxiDraw::setModel(int model)
{
    axiDraw.attr("options").attr("model") = model;
    Log(Log::Type::DEBUG, "Set model to " + std::to_string(model) + ".");
}

void AxiDraw::setPort(const std::string &port)
{
    axiDraw.attr("options").attr("port") = port != "auto" ? boost::python::str(port) : boost::python::object();
    Log(Log::Type::DEBUG, "Set port to " + port + ".");
}

std::string AxiDraw::getMode()
{
    std::string mode = boost::python::extract<std::string>(axiDraw.attr("options").attr("mode"));
    Log(Log::Type::DEBUG, "  Mode is " + mode + ".");

    return mode;
}

#pragma endregion
#pragma region Interactive

void AxiDraw::modeInteractive()
{
    axiDraw.attr("interactive")();
    Log(Log::Type::DEBUG, "Mode is set to interactive.");
}

void AxiDraw::setUnits(int units)
{
    axiDraw.attr("options").attr("units") = units;
    Log(Log::Type::DEBUG, "Set units to " + std::to_string(units) + ".");
}

void AxiDraw::connect()
{
    if (!axiDraw.attr("connect")()) Log(Log::Type::FATAL, "Could not connect to AxiDraw.");
    else Log(Log::Type::DEBUG, "Connected to AxiDraw.");
}

void AxiDraw::disconnect()
{
    axiDraw.attr("disconnect")();
    Log(Log::Type::DEBUG, "Disconnected from AxiDraw.");
}

void AxiDraw::updateOptions()
{
    axiDraw.attr("update")();
    Log(Log::Type::DEBUG, "Updated options.");
}

void AxiDraw::penUp()
{
    if (!axiDraw.attr("current_pen")())
    {
        axiDraw.attr("penup")();
        Log(Log::Type::DEBUG, "Pen is up.");
    }
}

void AxiDraw::penDown()
{
    if (axiDraw.attr("current_pen")())
    {
        axiDraw.attr("pendown")();
        Log(Log::Type::DEBUG, "Pen is down.");
    }
}

void AxiDraw::penToggle()
{
    axiDraw.attr("current_pen")() ? penDown() : penUp();
    Log(Log::Type::DEBUG, std::string("Pen toggled to ") + (axiDraw.attr("current_pen")() ? "down" : "up") + ".");
}

void AxiDraw::home()
{
    axiDraw.attr("moveto")(0, 0);
    Log(Log::Type::DEBUG, "Moved to home.");
}

void AxiDraw::goTo(double x, double y)
{
    axiDraw.attr("moveto")(x, y);
    Log(Log::Type::DEBUG, "Moved to (" + std::to_string(x) + ", " + std::to_string(y) + ").");
}

void AxiDraw::goToRelative(double x, double y)
{
    axiDraw.attr("move")(x, y);
    Log(Log::Type::DEBUG, "Moved to (" + std::to_string(x) + ", " + std::to_string(y) + ") relatively.");
}

void AxiDraw::draw(std::vector<std::pair<double, double>> path)
{
    for (auto point: path)
    {
        if (point == path.front())
        {
            axiDraw.attr("moveto")(point.first, point.second);
            Log(Log::Type::DEBUG,
                "Moved to (" + std::to_string(point.first) + ", " + std::to_string(point.second) + ").");
        } else
        {
            axiDraw.attr("lineto")(point.first, point.second);
            Log(Log::Type::DEBUG,
                "Drew line to (" + std::to_string(point.first) + ", " + std::to_string(point.second) + ").");
        }
    }
}

void AxiDraw::wait(double ms)
{
    axiDraw.attr("delay")(ms);
    Log(Log::Type::DEBUG, "Waited for " + std::to_string(ms) + " ms.");
}

std::pair<double, double> AxiDraw::getPosition()
{
    const boost::python::object &pos = axiDraw.attr("current_pos");
    std::pair<double, double> position = {boost::python::extract<double>(pos[0])(),
                                          boost::python::extract<double>(pos[1])()};

    Log(Log::Type::DEBUG,
        "Current position is (" + std::to_string(position.first) + ", " + std::to_string(position.second) + ").");
    return position;
}

bool AxiDraw::getPen()
{
    bool pen = boost::python::extract<bool>(axiDraw.attr("current_pen")());

    Log(Log::Type::INFO, std::string("Pen status is ") + (pen ? "down" : "up") + ".");
    return pen;
}

#pragma endregion
#pragma region Plot

void AxiDraw::modePlot(const std::string &filename)
{
    std::stringstream output;
    std::streambuf *outputBuffer = std::cout.rdbuf();
    std::cout.rdbuf(output.rdbuf());

    if (!axiDraw.attr("plot_setup")(filename)) Log(Log::Type::FATAL, "Could not connect to AxiDraw.");
    else Log(Log::Type::DEBUG, "Mode is set to plot.");

    std::cout.rdbuf(outputBuffer);
    std::string outputString = output.str();
    if (outputString.find("Failed to connect to AxiDraw.") != std::string::npos)
        Log(Log::Type::FATAL, "Could not connect to AxiDraw.");
}

void AxiDraw::runPlot()
{
    if (!axiDraw.attr("plot_run")()) Log(Log::Type::FATAL, "Could not run plot.");
    else Log(Log::Type::DEBUG, "Running plot.");
}

#pragma endregion
