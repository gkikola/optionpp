@mainpage Option++

C++ library for reading command-line program options. Written by [Greg
Kikola](https://www.gregkikola.com/).

@tableofcontents

- @ref start "Tutorial"
- @ref example_programs "Example Programs"
- @ref build "Build Instructions"


@section intro Introduction

Option++ provides an easy way to read, validate, and process
command-line options. It can also generate a detailed, properly
formatted help message.

There is a single-header version of the library provided, or you can
build and link to the shared library.


@section features Features

- Supports the usual Unix and GNU/Linux conventions
  - Use long (`--option`) and short (`-o`) option names
  - Double dash by itself (`--`) indicates end of options
  - Options may have mandatory or optional arguments
- Can parse arguments passed to `main` directly, or can read options
  from a string
- Allows you to bind variables directly to options
- Input validation for numerical arguments
- Can automatically generate a help message
- Options can be separated into groups for better organization
- Easily iterate over all parsed data
  - If desired, can iterate over only non-option arguments
  - Can also iterate over only option arguments
- The strings that are used as option prefixes can be customized
- Exception-based error handling


@section option_format Option Format

Both long and short option names are supported. For example:
```
myprogram --verbose -af file.txt
```
Each option may be set to take an optional or mandatory argument:
```
myprogram --output=file.txt
```
The equals sign is not required:
```
myprogram -o file.txt
```
With short options, an argument may be written immediately after the
option name:
```
mycompiler -DMY_DEFINE
```

'`--`' can be provided by itself as an argument to indicate that all
remaining command-line arguments should not be interpreted as options:
```
myprogram --this-is-an-option -- --this-is-not
```


@section intro_program Example Program

Here is a simple program that demonstrates how you can use Option++:

@include{lineno} basic.cpp

See the @ref example_programs "Examples" page for many more examples.


@section copyright Copyright

Copyright &copy; 2017-2020 Greg Kikola. License BSL-1.0:
[Boost Software License
version 1.0](https://www.boost.org/LICENSE_1_0.txt).

Option++ is free software: you are free to change and redistribute
it. There is NO WARRANTY, to the extent permitted by law.
