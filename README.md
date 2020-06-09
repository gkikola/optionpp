# Option++

- [Homepage](https://www.gregkikola.com/projects/optionpp/)
- [Documentation](https://www.gregkikola.com/projects/optionpp/docs/)
- [Examples](https://www.gregkikola.com/projects/optionpp/docs/example_programs.html)
- [Tutorial](https://www.gregkikola.com/projects/optionpp/docs/start.html)

## Introduction

Option++ is a C++ library for reading command-line program
options. Option++ provides an easy way to read, validate, and process
command-line arguments and it can also print detailed program help
information.

A [single-header version](single_header/optionpp/optionpp.hpp) of the
library is available. If you use the single-header file, you must
define the macro `OPTIONPP_MAIN` in exactly one source file before
including the header.


## Features

- Supports the usual Unix and GNU/Linux conventions
  - Use long (`--option`) and short (`-o`) option names
  - Double dash by itself (`--`) indicates end of options
  - Options may have mandatory or optional arguments
- Can parse arguments passed to `main` directly, or can read options
  from a string
- Allows you to bind variables to particular options
- Input validation for numerical arguments
- Can automatically generate a help message
- Options can be separated into groups for better organization
- Easily iterate over all parsed data
- Exception-based error handling


## Build instructions

### Requirements

In addition to a compiler supporting C++11, you will also need
[CMake](https://cmake.org/) version 3.10 or higher.


### Unix-like Environments

First clone the repository with `git clone
https://github.com/gkikola/optionpp.git`. Then, from the root of the
main project directory, run

```
mkdir build
cmake ..
make
```

This will create several files:

- liboptionpp.so: The actual library
- run_tests: Unit test executable
- example_*: Example programs from docs/examples/

To compile the library only, you can use `make optionpp`.


### Windows

#### Visual Studio 2019

Use `git` to clone the repository:
`git clone https://github.com/gkikola/optionpp.git`.

Create a `build` directory within the root project directory. Open a
command prompt, navigate to the `build` directory, and run `cmake ..`
to create the Visual C++ project files.

Open the solution file `OPTIONPP.sln` in Visual Studio. In the menu,
select Build > Build Solution. This will build several projects:

- optionpp: The actual library
- run_tests: Unit test execution
- example_*: Example programs from docs\examples\

Under the default Debug configuration, the resulting library and
executable files will be located in the `build\Debug` directory.

#### Troubleshooting

If Visual Studio gives you the error message "Error HRESULT E_FAIL has
been returned from a call to a COM component" try this:
1. Close Visual Studio.
2. Delete the hidden `.vs` directory from the `build` directory.
3. Reopen the solution or one of the project files.
4. Try building the solution again.


## Copyright

Copyright &copy; 2017-2020 Greg Kikola. License BSL-1.0: [Boost
Software License version 1.0](https://www.boost.org/LICENSE_1_0.txt).

Option++ is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law. See the file
[LICENSE.txt](LICENSE.txt) for more details.
