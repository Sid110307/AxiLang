#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <regex>

#include <boost/python.hpp>
#include <boost/filesystem.hpp>

namespace py = boost::python;
namespace fs = boost::filesystem;

class AxiDraw
{
public:
	AxiDraw();
	~AxiDraw() = default;

#pragma region General
	void setAcceleration(double);

	void setPenUpPosition(double);
	void setPenDownPosition(double);

	void setPenUpDelay(double);
	void setPenDownDelay(double);

	void setPenUpSpeed(double);
	void setPenDownSpeed(double);

	void setPenUpRate(double);
	void setPenDownRate(double);

	void setModel(int);
	void setPort(const std::string &);

	std::string getMode();
#pragma endregion

#pragma region Interactive
	void modeInteractive();
	void setUnits(int);

	void connect();
	void disconnect();
	void updateOptions();

	void penUp();
	void penDown();
	void penToggle();

	void home();
	void goTo(double, double);
	void goToRelative(double, double);

	void draw(std::vector<std::pair<double, double>> path);
	void wait(double);

	std::pair<double, double> getPosition();
	bool getPen();

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
	void modePlot(const std::string &);
	void runPlot();

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

	#pragma region Enums
	enum Units
	{
		Inches = 0,
		Centimeters = 1,
		Millimeters = 2,
	};

	enum Models
	{
		V2_V3_SEA4 = 1,
		V3A3_SEA3 = 2,
		V3_XLX = 3,
		MiniKit = 4,
		SEA1 = 5,
		SEA2 = 6,
		V3B6 = 7,
	};
#pragma endregion

private:
	py::object axiDraw;
};
