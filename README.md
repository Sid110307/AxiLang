# AxiLang v0.2.0 (unofficial)

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

- [Raise and Lower Pen](tests/pen.axi)
- [Hello World](tests/helloWorld.axi)
- [A Square](tests/square.axi)

Check the [tests](tests) directory for more examples.

## Syntax

> TODO

## License

[MIT License](LICENSE)

## Changelog

Just read the title and description of the commits. I'm too lazy to write a changelog.
