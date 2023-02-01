#include "include/api.h"

AxiDraw::AxiDraw()
{
	Py_Initialize();

	try
	{
		axiDraw = py::import("pyaxidraw").attr("axiDraw").attr("AxiDraw")();
	}
	catch (py::error_already_set const &)
	{
		PyErr_Print();
		std::cerr << "[\033[1;31mERROR\033[0m]: The library 'pyaxidraw' is not installed.\n"
				  << "Please install it with '\033]8;;https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip"
				  << "\033\\pip install https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip\033]8;;\033\\'"
				  << std::endl;
		exit(EXIT_FAILURE);
	}
}

AxiDraw::~AxiDraw()
{
	Py_Finalize();
}

static std::string downloadFile(const std::string &url)
{
	std::string host, path;
	std::size_t protocolEnd = url.find("://");

	if (protocolEnd != std::string::npos)
	{
		std::size_t hostStart = protocolEnd + 3;
		std::size_t pathStart = url.find('/', hostStart);

		if (pathStart != std::string::npos)
		{
			host = url.substr(hostStart, pathStart - hostStart);
			path = url.substr(pathStart);
		} else
		{
			host = url.substr(hostStart);
			path = "/";
		}
	} else
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: Invalid URL '" << url << "'." << std::endl;
		exit(EXIT_FAILURE);
	}

	boost::asio::io_service ios;

	boost::asio::ip::tcp::socket socket(ios);
	boost::asio::ip::tcp::resolver resolver(ios);
	boost::asio::ip::tcp::resolver::query query(host, "http");
	boost::asio::connect(socket, resolver.resolve(query));

	std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
	boost::asio::write(socket, boost::asio::buffer(request));

	std::string httpResponse;
	boost::asio::streambuf responseBuffer;
	boost::asio::read_until(socket, responseBuffer, "\r\n");
	std::istream responseStream(&responseBuffer);
	std::getline(responseStream, httpResponse);

	if (httpResponse.find("200 OK") == std::string::npos)
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: Could not download file from '" << url << "'." << std::endl;
		exit(EXIT_FAILURE);
	}

	fs::path temp = fs::unique_path();
	std::ofstream file;
	file.open(temp.string(), std::ios::out | std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: Could not create temporary file for plot." << std::endl;
		exit(EXIT_FAILURE);
	}

	return temp.string();
}

#pragma region General

void AxiDraw::setAcceleration(double acceleration)
{
	axiDraw.attr("options").attr("accel") = acceleration;
}

void AxiDraw::setPenUpPosition(double position)
{
	axiDraw.attr("options").attr("pen_pos_up") = position;
}

void AxiDraw::setPenDownPosition(double position)
{
	axiDraw.attr("options").attr("pen_pos_down") = position;
}

void AxiDraw::setPenUpDelay(double delay)
{
	axiDraw.attr("options").attr("pen_delay_up") = delay;
}

void AxiDraw::setPenDownDelay(double delay)
{
	axiDraw.attr("options").attr("pen_delay_down") = delay;
}

void AxiDraw::setPenUpSpeed(double speed)
{
	axiDraw.attr("options").attr("speed_penup") = speed;
}

void AxiDraw::setPenDownSpeed(double speed)
{
	axiDraw.attr("options").attr("speed_pendown") = speed;
}

void AxiDraw::setPenUpRate(double rate)
{
	axiDraw.attr("options").attr("pen_rate_raise") = rate;
}

void AxiDraw::setPenDownRate(double rate)
{
	axiDraw.attr("options").attr("pen_rate_lower") = rate;
}

void AxiDraw::setModel(int model)
{
	axiDraw.attr("options").attr("model") = model;
}

void AxiDraw::setPort(const std::string &port)
{
	axiDraw.attr("options").attr("port") = port;
}

#pragma endregion
#pragma region Interactive

void AxiDraw::modeInteractive()
{
	axiDraw.attr("interactive")();
}

void AxiDraw::setUnits(int units)
{
	axiDraw.attr("options").attr("units") = units;
}

void AxiDraw::connect()
{
	if (!axiDraw.attr("connect")()) std::cerr << "[\033[1;31mERROR\033[0m]: Could not connect to AxiDraw!" << std::endl;
}

void AxiDraw::disconnect()
{
	axiDraw.attr("disconnect")();
}

void AxiDraw::updateOptions()
{
	axiDraw.attr("update")();
}

void AxiDraw::penUp()
{
	axiDraw.attr("penup")();
}

void AxiDraw::penDown()
{
	axiDraw.attr("pendown")();
}

void AxiDraw::penToggle()
{
	axiDraw.attr("current_pen")() ? penDown() : penUp();
}

void AxiDraw::home()
{
	axiDraw.attr("moveto")(0, 0);
}

void AxiDraw::goTo(double x, double y)
{
	axiDraw.attr("moveto")(x, y);
}

void AxiDraw::goToRelative(double x, double y)
{
	axiDraw.attr("move")(x, y);
}

void AxiDraw::drawPath(std::vector<std::pair<double, double>> path)
{
	for (auto point: path)
	{
		if (point == path.front()) axiDraw.attr("moveto")(point.first, point.second);
		else axiDraw.attr("lineto")(point.first, point.second);
	}
}

void AxiDraw::wait(double ms)
{
	axiDraw.attr("delay")(ms);
}

std::vector<double> AxiDraw::getPosition()
{
	const py::object &pos = axiDraw.attr("current_pos");
	return {py::extract<double>(pos[0])(), py::extract<double>(pos[1])()};
}

bool AxiDraw::getPenStatus()
{
	return axiDraw.attr("current_pen")();
}

#pragma endregion
#pragma region Plot

void AxiDraw::modePlot(const std::string &filename)
{
	axiDraw.attr("plot_setup")(
			std::regex_match(filename, std::regex("https?://.*")) ? downloadFile(filename) : filename);
}

void AxiDraw::runPlot()
{
	axiDraw.attr("plot_run")();
}

#pragma endregion
