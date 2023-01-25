#include "include/api.h"

AxiDraw::AxiDraw()
{
	Py_Initialize();

	try
	{
		axidraw = py::import("pyaxidraw").attr("axidraw").attr("AxiDraw")();
	}
	catch (py::error_already_set const &)
	{
		PyErr_Print();
		std::cerr << "[\033[1;31mERROR\033[0m]: The library 'pyaxidraw' is not installed\n"
				  << "Please install it with 'pip install https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip'"
				  << std::endl;
		exit(EXIT_FAILURE);
	}
}

AxiDraw::~AxiDraw()
{
	Py_Finalize();
}

static std::string download_file(const std::string &url)
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

	std::string http_response;
	boost::asio::streambuf response_buffer;
	boost::asio::read_until(socket, response_buffer, "\r\n");
	std::istream response_stream(&response_buffer);
	std::getline(response_stream, http_response);

	if (http_response.find("200 OK") == std::string::npos)
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: Could not download file from '" << url << "'." << std::endl;
		exit(EXIT_FAILURE);
	}

	fs::path temp = fs::temp_directory_path() / fs::unique_path();
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

void AxiDraw::set_acceleration(double acceleration)
{
	axidraw.attr("options").attr("accel") = acceleration;
}

void AxiDraw::set_pen_up_position(double position)
{
	axidraw.attr("options").attr("pen_pos_up") = position;
}

void AxiDraw::set_pen_down_position(double position)
{
	axidraw.attr("options").attr("pen_pos_down") = position;
}

void AxiDraw::set_pen_up_delay(double delay)
{
	axidraw.attr("options").attr("pen_delay_up") = delay;
}

void AxiDraw::set_pen_down_delay(double delay)
{
	axidraw.attr("options").attr("pen_delay_down") = delay;
}

void AxiDraw::set_pen_up_speed(double speed)
{
	axidraw.attr("options").attr("speed_penup") = speed;
}

void AxiDraw::set_pen_down_speed(double speed)
{
	axidraw.attr("options").attr("speed_pendown") = speed;
}

void AxiDraw::set_pen_up_rate(double rate)
{
	axidraw.attr("options").attr("pen_rate_raise") = rate;
}

void AxiDraw::set_pen_down_rate(double rate)
{
	axidraw.attr("options").attr("pen_rate_lower") = rate;
}

void AxiDraw::set_model(int model)
{
	axidraw.attr("options").attr("model") = model;
}

void AxiDraw::set_port(const std::string &port)
{
	axidraw.attr("options").attr("port") = port;
}

#pragma endregion
#pragma region Interactive

void AxiDraw::mode_interactive()
{
	axidraw.attr("interactive")();
}

void AxiDraw::set_units(int units)
{
	axidraw.attr("options").attr("units") = units;
}

void AxiDraw::connect()
{
	if (!axidraw.attr("connect")()) std::cerr << "[\033[1;31mERROR\033[0m]: Could not connect to AxiDraw!" << std::endl;
}

void AxiDraw::disconnect()
{
	axidraw.attr("disconnect")();
}

void AxiDraw::update_options()
{
	axidraw.attr("update")();
}

void AxiDraw::pen_up()
{
	axidraw.attr("penup")();
}

void AxiDraw::pen_down()
{
	axidraw.attr("pendown")();
}

void AxiDraw::pen_toggle()
{
	axidraw.attr("current_pen")() ? pen_down() : pen_up();
}

void AxiDraw::home()
{
	axidraw.attr("moveto")(0, 0);
}

void AxiDraw::go_to(double x, double y)
{
	axidraw.attr("moveto")(x, y);
}

void AxiDraw::go_to_relative(double x, double y)
{
	axidraw.attr("move")(x, y);
}

void AxiDraw::draw_path(std::vector<std::pair<double, double>> path)
{
	for (auto point: path)
	{
		if (point == path.front()) axidraw.attr("moveto")(point.first, point.second);
		else axidraw.attr("lineto")(point.first, point.second);
	}
}

void AxiDraw::wait(double ms)
{
	axidraw.attr("delay")(ms);
}

std::vector<double> AxiDraw::get_position()
{
	const py::object &pos = axidraw.attr("current_pos");
	return {py::extract<double>(pos[0])(), py::extract<double>(pos[1])()};
}

bool AxiDraw::get_pen_status()
{
	return axidraw.attr("current_pen")();
}

#pragma endregion
#pragma region Plot

void AxiDraw::mode_plot(const std::string &filename)
{
	axidraw.attr("plot_setup")(
			std::regex_match(filename, std::regex("https?://.*")) ? download_file(filename) : filename);
}

void AxiDraw::run_plot()
{
	axidraw.attr("plot_run")();
}

#pragma endregion

