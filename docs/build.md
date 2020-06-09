@page build Build Instructions

@brief How to build Option++ and integrate it with your project

@section single_header Single-header Version

To use the single-header version of the library, just download the
[optionpp.hpp](https://github.com/gkikola/optionpp/raw/v2/single_header/optionpp/optionpp.hpp)
header file and place it inside an `optionpp` directory somewhere
within your project directory (a subdirectory called 'third_party' or
something similar is a typical location).

Make sure your compiler, IDE, or build system is configured to search
for includes in the subdirectory containing the `optionpp`
directory. Then `#include &lt;optionpp/optionpp.hpp>` where needed.

There is one more thing to do: in order to avoid multiple definitions,
the single-header version looks for a macro called `OPTIONPP_MAIN` in
order to conditionally compile class method definitions. This means
that in exactly *one* of your source files where you `#include
&lt;optionpp/optionpp.hpp>`, you need to add `#define OPTIONPP_MAIN`
*before* the `#include` statement.


@section requirements Build Requirements

To build the library, you will need a compiler supporting C++11, along
with [CMake](https://cmake.org/) version 3.10 or higher. To easily
clone the repository, a `git` installation is also recommended.


@section build_unix Unix-like Environments

First clone the repository with `git clone
https://github.com/gkikola/optionpp.git`. Then, from the root of the
main project directory, run
```
mkdir build
cmake ..
make
```
This will create several files:
* liboptionpp.so - The actual library
* test - Unit test executable
* example_* - Example programs from docs/examples/

To compile the library only, you can use `make optionpp`.


@section build_windows Windows

@subsection build_vs2019 Visual Studio 2019

Use `git` to clone the repository:
`git clone https://github.com/gkikola/optionpp.git`.

Create a `build` directory within the root project directory. Open a
command prompt, navigate to the `build` directory, and run `cmake ..`
to create the Visual C++ project files.

Open the solution file `OPTIONPP.sln` in Visual Studio. In the menu,
select Build > Build Solution. This will build several projects:
* optionpp.dll - The actual library
* test.exe - Unit test execution
* example_*.exe - Example programs from docs\\examples\\

Under the default Debug configuration, the resulting library and
executable files will be located in the `build\Debug` directory.

@subsection build_vs2019_trouble Troubleshooting

If Visual Studio gives you the error message "Error HRESULT E_FAIL has
been returned from a call to a COM component" try this:
1. Close Visual Studio.
2. Delete the hidden `.vs` directory from the `build` directory.
3. Reopen the solution or one of the project files.
4. Try building the solution again.
