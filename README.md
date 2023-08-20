# AxiLang v0.9.5 (unofficial)

> A scripting language for controlling the [AxiDraw](https://axiDraw.com/) plotter.

## Table of Contents

- [Requirements](#requirements)
- [Usage](#usage)
- [Syntax](#syntax)
- [Examples](#examples)
- [License](#license)
- [Changelog](#changelog)

## Requirements

- An [AxiDraw](https://shop.evilmadscientist.com/productsmenu/890) plotter
- [CMake 3.2+](https://cmake.org/download/)
- [Python 3.7+](https://www.python.org/downloads/)
- [pyaxidraw](https://axiDraw.com/doc/py_api/#installation) (Just
  run `pip install https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip`)
- [Boost](https://www.boost.org/users/download/)
- [cURL](https://curl.se/download.html)

## Usage

- Clone the repository

```bash
$ git clone https://github.com/Sid110307/AxiLang.git
```

- Configure the project

```bash
$ cd AxiLang
$ cmake -S . -B bin
```

- Build the executable

```bash
$ cmake --build bin --clean-first --target all -j4
```

- Run the executable

```bash
$ ./bin/axilang
```

## Examples

- [Raise and lower pen](tests/pen.axi)
- [Plot an SVG from an internet URL](tests/vectorUrl.axi)
- [Draw a square](tests/square.axi)

Check the [tests](tests) directory for more examples.

## Syntax

The file extension for AxiLang files is `.axi`.

### Comments

Line comments:

```matlab
% This is a comment
```

Block comments:

```matlab
%=
  This is a very long paragraph, so I would
  need to use multiple lines.
=%
```

### Setting the mode

The mode can be set to either `plot` or `interactive`.

```matlab
MODE P
% Or
MODE I
```

### Setting options

Options can be set using the `OPTS` and `END_OPTS` keywords.

```matlab
OPTS
  ACCEL 75      % Acceleration rate factor (1 - 100). Default: 75
  PENU_POS 60   % Height of pen when raised (0 - 100). Default: 60
  PEND_POS 30   % Height of pen when lowered (0 - 100). Default: 30
  PENU_DELAY 0  % Optional delay after pen is raised (ms). Default: 0
  PEND_DELAY 0  % Optional delay after pen is lowered (ms). Default: 0
  PENU_SPEED 75 % Maximum transit speed, when pen is up (1 - 100). Default: 75
  PEND_SPEED 25 % Maximum plotting speed, when pen is down (1 - 100). Default: 25
  PENU_RATE 75  % Rate of raising pen (1 - 100). Default: 75
  PEND_RATE 50  % Rate of lowering pen (1 - 100). Default: 50
  MODEL 1       % AxiDraw Model (1 - 6).
                  % 1: AxiDraw V2 or V3 (Default).
                  % 2: AxiDraw V3/A3 or SE/A3.
                  % 3: AxiDraw V3 XLX.
                  % 4: AxiDraw MiniKit.
                  % 5: AxiDraw SE/A1.
                  % 6: AxiDraw SE/A2.
  PORT "auto"   % Serial port or named AxiDraw to use. "auto" (Default) will plot to first unit found.
  UNITS 1       % Units to use (0 - 2).
                  % 0: Inches (Default). 1: Millimeters. 2: Pixels.
                  % Can only be set in interactive mode.
END_OPTS
```

### Plot mode

First, the mode must be set to `plot`.

```matlab
MODE P
```

Then, the file path (or internet URL) of the SVG file must be specified (options can be set before this).

```matlab
SETPLOT "tests/square.svg"
% Or
SETPLOT "https://example.com/square.svg"
```

Finally, the plot command must be executed.

```matlab
PLOT
```

### Interactive mode

First, the mode must be set to `interactive`.

```matlab
MODE I
```

Then, the AxiDraw must be connected.

```matlab
CONNECT
```

The following commands can be used in interactive mode:

- `PENUP` - Raise the pen.
- `PENDOWN` - Lower the pen.
- `PENTOGGLE` - Toggle the pen state.
- `HOME` - Move the pen to the home position (0, 0).
- `GOTO <X> <Y>` - Move the pen to the specified position (X, Y).
- `GOTO_REL <X> <Y>` - Move the pen to the specified position (X, Y) relative to the current position.
- `DRAW <X1> <Y1> <X2> <Y2> ... <Xn> <Yn>` - Draw a path from (X1, Y1) through the coordinates in between to (Xn, Yn).
  If only 1 pair of coordinates are provided, then the pen will only move to that position.
- `WAIT <TIME>` - Wait for the specified time (in milliseconds).
- `GETPOS` - Print the current position of the pen.
- `GETPEN` - Print the current state of the pen (up or down).

For updating the options anytime in between, use the `UOPTS` and `END_UOPTS` keywords, with the same options as `OPTS`
and `END_OPTS`.

```matlab
UOPTS
  % ...
END_UOPTS
```

Finally, the AxiDraw must be disconnected.

```matlab
DISCONNECT
```

## Command Line Options

| Option        | Simplified form | Arguments  | Description                       |
|---------------|-----------------|------------|-----------------------------------|
| --help        | -h              |            | Print the help message and exit   |
| --version     | -v              |            | Print the version number and exit |
| --debug       | -d              |            | Show extra info while running     |
| --file        | -f              | `filename` | Input file path                   |
| --interactive | -i              |            | Start an interactive interpreter  |

## License

[MIT License](LICENSE)

## Changelog

Just read the title and description of the commits. I'm too lazy to write a changelog.

## Future plans

Check the [TODO.md](TODO.md) file for what I plan to add in the future.
