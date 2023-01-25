#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <regex>

#include <boost/python.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>

namespace py = boost::python;
namespace asio = boost::asio;
namespace fs = boost::filesystem;

class AxiDraw
{
public:
	AxiDraw();
	~AxiDraw();

#pragma region General
	void set_acceleration(double);

	void set_pen_up_position(double);
	void set_pen_down_position(double);

	void set_pen_up_delay(double);
	void set_pen_down_delay(double);

	void set_pen_up_speed(double);
	void set_pen_down_speed(double);

	void set_pen_up_rate(double);
	void set_pen_down_rate(double);

	void set_model(int);
	void set_port(const std::string &);
#pragma endregion

#pragma region Interactive
	void mode_interactive();
	void set_units(int);

	void connect();
	void disconnect();
	void update_options();

	void pen_up();
	void pen_down();
	void pen_toggle();

	void home();
	void go_to(double, double);
	void go_to_relative(double, double);

	void draw_path(std::vector<std::pair<double, double>>);
	void wait(double);

	std::vector<double> get_position();
	bool get_pen_status();

	/*
	+--------------+----------------------------------------------------+
	| Option       | Description                                        |
	+--------------+----------------------------------------------------+
	| usb_command  | Issue a direct command to the EBB.                 |
	| usb_query    | Issue a direct query to the EBB.                   |
	+--------------+----------------------------------------------------+
	*/
#pragma endregion

#pragma region Plot
	void mode_plot(const std::string &);
	void run_plot();

	/*
	+--------------+----------------------------------------------------+
	| Option       | Description                                        |
	+--------------+----------------------------------------------------+
	| mode         | Specify general mode of operation.                 |
	| manual_cmd   | Specify which "manual" mode command to use.        |
	| walk_dist    | Distance to move for manual walk commands.         |
	| layer        | Specify which layer(s) to plot in the layers mode. |
	| copies       | Specify the number of copies to plot.              |
	| page_delay   | Specify delay between pages, for multiple copies.  |
	| auto_rotate  | Enable auto-rotate when plotting.                  |
	| preview      | Perform offline simulation of plot only.           |
	| rendering    | Render motion when using preview.                  |
	| reordering   | Optimize plot order before plotting.               |
	| random_start | Randomize start positions of closed paths.         |
	| report_time  | Report time and distance after the plot.           |
	| digest       | Return plot digest instead of full SVG             |
	| webhook      | Enable webhook alerts.                             |
	| webhook_url  | URL for webhook alerts.                            |
	+--------------+----------------------------------------------------+
	*/
#pragma endregion

private:
	py::object axidraw;
};
