# Option++

Option++ is a C++ library for reading command-line program
options. Option++ provides an easy way to read, validate, and process
command-line options and it can also print detailed program help
information.

Option++ supports both long and short option names (e.g., `myprogram
--version` or `myprogram -abcd file.txt`). Each program option may
take a mandatory or optional argument (e.g., `myprogram --color=red`
or `myprogram -C red` or even `myprogram -Cred`). A double dash, `--`,
may be used by itself to indicate the end of the options, so that
remaining command-line arguments do not get parsed by Option++.


## Build instructions

### Requirements

In addition to a compiler supporting C++11, you will also need CMake
version 3.10 or higher.


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
* liboptionpp.so - The actual library
* run_tests - Unit test executable
* example_* - Example programs from docs/examples/
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
* optionpp - The actual library
* run_tests - Unit test execution
* example_* - Example programs from docs\examples\

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
