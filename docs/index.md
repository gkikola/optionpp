@mainpage Option++

C++ library for reading command-line program options.

@tableofcontents

- @ref start "Getting Started"
- @ref example_programs "Example Programs"


# Introduction

Option++ provides an easy way to read, validate, and process
command-line options. It can also generate a detailed, properly
formatted help message.

There is a single-header version of the library provided, or you can
build and link to the shared library.


# Features

TODO


# Option Format

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
With short options, an argument need not be preceded by another
character:
```
mycompiler -DMY_DEFINE
```

'`--`' can be provided by itself as an argument to indicate that all
remaining command-line arguments should not be interpreted as options:
```
myprogram --this-is-an-option -- --this-is-not
```


# Example Program

Here is a simple program that demonstrates how you can use Option++:

@include{lineno} basic.cpp

See the @ref example_programs "Examples" page for many more examples.


# Copyright

Copyright &copy; 2017-2020 Greg Kikola. License BSL-1.0:
[Boost Software License
version 1.0](https://www.boost.org/LICENSE_1_0.txt).

Option++ is free software: you are free to change and redistribute
it. There is NO WARRANTY, to the extent permitted by law.
