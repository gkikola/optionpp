@page start Tutorial

@brief A guide to setting up and using Option++

@tableofcontents

@section downloading Obtaining Option++

First you need to download the library. There are two versions: a
single-header version and a standard version.

@subsection dl_single_header Single header version

For the single-header version, just download
[optionpp.hpp](https://raw.githubusercontent.com/gkikola/optionpp/master/single_header/optionpp/optionpp.hpp)
and put it in a directory named `optionpp` somewhere within your
project directory.

For the single-header version only, you must add

```
#define OPTIONPP_MAIN
```

to *exactly one* of the source files where you `#include
&lt;optionpp/optionpp.hpp>`. This is necessary to avoid multiple
definition errors.

Note: the single-header version is automatically generated from a
script by stitching together all the source and header files in the
library. During this process the files are stripped of comments and
the result is not particularly readable or easy to navigate. As a
result, if you want to read the code or modify Option++ itself, you
should look in the `src` and `include` directories for the original
files.


@subsection standard Standard version

Binaries are not yet provided, so the easiest way to obtain the
library is to clone the repository and build it locally. Instructions
on how to do that can be found on the @ref build "Build Instructions"
page.

The header files are located in the `include` directory of the
repository.


@section verify Verifying Your Setup

To make sure everything is set up correctly, try building the simple
program below. Note that if you are using the single-header version,
you will need to add `#define OPTIONPP_MAIN` at the top of the file.

@include{lineno} minimal.cpp

This program does very little, but compiling it successfully will help
verify that things are working as they should. To see more examples,
look on the @ref example_programs "Example Programs" page.


@section usage Library Usage

Now let's use Option++ to do some command-line parsing.

@subsection include The Include

To use the library, we need to `#include` the header file
`optionpp/optionpp.hpp` (for the single-header version of the library,
this will be a big file, but for the standard version it is a small
header that simply includes the other needed headers).

@subsection setup_options Setting Up the Options

`optionpp::parser` is the primary class we will be working with, so
let's construct an instance of this class:

```
optionpp::parser my_parser;
```

First the `parser` needs to know what the valid program options
are. For example, we can add a `--help` option like this:

```
my_parser.add_option().long_name("help");
```

The `add_option` method returns a reference to a newly-created
`option` object, to which we assigned the long option name
`help`. Now, when the parser encounters `--help` on the command line,
it will know that this is a valid option for our program.

`--help` is an example of a long option. In many cases, we want our
options to also accept a short option name. For example, we can add a
short name `-?` to our help option by doing

```
my_parser["help"].short_name('?');
```

In fact, this line would work even if we hadn't previously created the
`help` option. So the subscript operator provides another way for us
to add options.

The `option` methods like `long_name` and `short_name` also return
references to their class instance, which allows us to chain calls
together. For example, let's add a `--verbose` option with a short
name of `-v` and we'll give it a description that will appear in our
help message:

```
my_parser["verbose"].short_name('v').description("Increase verbosity");
```

@subsection arguments Options with Arguments

We can also add options that take arguments. The `option::argument`
method takes a string giving the name of the argument (this is used in
the help message) and an optional `bool` value that indicates whether
the argument is mandatory or optional. For example,

```
my_parser["file"].short_name('f').argument("FILE", true);
```

would add a `--file` option with short name `-f` that takes a
mandatory argument called "FILE" (argument names are usually
uppercase). This tells the parser to always look for a string argument
following the `--file` option.

@subsection parse The parse Methods

Once we have registered all of our program's options, we can run the
parser to read and evaluate the arguments that were sent to our
program:

```
int main(int argc, char* argv[]) {
  optionpp::parser my_parser;
  // Set up options here...

  optionpp::parser_result result;
  try {
    result = my_parser.parse(argc, argv);
  } catch(const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  // ...
}
```

There is an overloaded form of the `parse` method that takes a string,
so you could use Option++ to process arbitrary strings from somewhere
other than the command line, like in an initialization file.


@subsection parser_result The Results

`parse` returns a `parser_result` containing the parsed data. A
`parser_result` is a container of `parsed_entry`s.

Every command line argument is turned into a `parsed_entry` (usually;
in some cases, two arguments might be combined to one entry or one
argument might be split into several entries). Each parsed data entry
is an instance of a `struct` with several fields that are set by the
parser:
* `original_text` contains the original text that was entered on the command line,
* `is_option` is `true` if the entry represents an option and `false`
  if it is just a normal command-line argument,
* if the entry is for an option, `long_name` and `short_name` are the
  long and short option names and `argument` is the argument that was
  given, if any.

For example, in our program we might iterate over the results and
process them like this:

```
bool verbose = false;
std::string file;
for (const auto& entry : result) {
  if (!entry.is_option) {
    // Do something with non-option argument...
  } else {
    switch (entry.short_name) {
    case '?':
      std::cout << "Available options:\n";
      my_parser.print_help(std::cout) << std::endl;
      break;
    case 'v':
      verbose = true;
      break;
    case 'f':
      file = entry.argument;
      break;
    // ...
    }
  }
}
```

@subsection bind Binding Variables to Options

We can actually avoid the `switch` statement in the code above by
using another feature of Option++ that allows us to bind variables
directly to options. When the parser reads a particular option, it
sets the bound variable automatically and even handles type conversion
and validation.

For example, suppose our program, which we'll call `solve`, was using
an iterative method to solve numerical problems like finding roots of
nonlinear equations. We might want the user to be able to specify the
number of iterations of the algorithm to perform. We can accept the
number of iterations as an option argument. Since we can't perform a
negative number of iterations, we'll use an unsigned variable. We can
bind that variable to a `--num-iterations` option like this:

```
unsigned int num_iterations = 100; // Default number of iterations
my_parser["num-iterations"].short_name('i').bind_uint(&num_iterations)
  .description("Number of iterations to perform").argument("ITERS");
```

If the user calls our program with the command

```
solve data.txt --num-iterations=200
```

then, after we call `my_parser.parse`, the parser will set the
`num_iterations` variable to 200.

For options that don't take arguments, we can use `bind_bool` to bind
a boolean variable. If the option is present, the variable is set to
true, and if it is not the variable is set to false.


@subsection non_option_iterator Iterating Over Non-Option Arguments

Using variable binding, we don't really need to examine the option
entries themselves. We can iterate over only the non-option arguments
using a `non_option_iterator` or a `non_option_const_iterator` (there
is also an `option_iterator` and an `option_const_iterator` if we
wanted to do the opposite and iterate over only option arguments). Our
loop might look something like this:

```
for (const auto& entry : optionpp::non_option_iterator(result)) {
  std::cout << "Received argument '" << entry.original_text << "'\n";
}
```

If the user entered

```
solve -v data1.txt --num_iterations=50 data2.txt
```

we would get the output

```
Received argument 'data1.txt'
Received argument 'data2.txt'
```

@subsection print_help The Help Message

Another thing we can do with Option++ is to respond to the `--help`
option by displaying a useful summary of all the options
available. This is easily done using `parser::print_help`. This method
will print a properly formatted list of `options` onto an output
stream:

```
bool show_help = false;
my_parser["help"].short_name('?').bind_bool(&show_help)
  .description("Display help message");

// ...

my_parser.parse(argc, argv);

// ...

if (show_help) {
  std::cout << "You are using Solve version 1.0\n\n"
            << "This program accepts the following options:\n";
  my_parser.print_help(std::cout) << std::endl;
  return 0;
}

// ...

```

If we run `solve --help` we might get output that looks like this:

```
You are using Solve version 1.0

This program accepts the following options:
  -?, --help                  Display help message
  -v, --verbose               Increase verbosity
  -f, --file=FILE             Write output to FILE
  -i, --num-iterations=ITERS  Number of iterations to perform
```

The option description text will automatically be word-wrapped if it
is too long to fit in one line (by default the maximum line length is
78 characters, which is close to the default width of most terminals;
this can be changed in the arguments to `parser::print_help`).


@subsection groups Grouping Options in the Help Message

As your program grows in size and your users' needs grow more diverse,
you may find that you need to accept many, many different options. For
programs with dozens or hundreds of options, it is helpful to group
certain options together so that you can display more organized help
messages. Option++ lets you do this with the `parser::group` method.

Instead of doing `my_parser.add_option().long_name("my-option");`, we
can do something like

```
my_parser.group("My group").add_option().long_name("my-option");
```

Or, using the `option_group` subscript operator, we could also write

```
my_parser.group("My group")["my-option"];
```

We can add multiple options to the same group. Going back to our `solve`
program, we might write

```
unsigned int precision = 16;
std::string method;
bool optimize = false;

auto& math = my_parser.group("Math options");
math["precision"].short_name('p').bind_uint(&precision)
  .argument("DIGITS").description("Output precision");
math["method"].argument("TYPE").bind_string(&method)
  .short_name('m')
  .description("Algorithm to use: "
               "'newton' for Newton's method (default), "
               "'bisection' for bisection method, "
               "'secant' for secant method, "
               "'hybrid' for hybrid method");
math["optimize"].short_name('o').bind_bool(&optimize)
  .description("Use experimental speed optimizations");
```

When displaying the help message using `parser::print_help`, all
options belonging to the same group will appear together in a block.
The name or description of the group that is passed to `parser::group`
is displayed on a line before the option block:

```
You are using Solve version 1.0

This program accepts the following options:
  -?, --help                  Display help message
  -v, --verbose               Increase verbosity
  -f, --file=FILE             Write output to FILE
  -i, --num-iterations=ITERS  Number of iterations to perform

Math options
  -p, --precision=DIGITS      Output precision
  -m, --method=TYPE           Algorithm to use: 'newton' for Newton's method
                                (default), 'bisection' for bisection method,
                                'secant' for secant method, 'hybrid' for
                                hybrid method
  -o, --optimize              Use experimental speed optimizations
```

Notice that all the math options are together, in the order we
originally specified them. If you prefer the options to be sorted
within each group, you can call `parser::sort_options` before calling
`parser::print_help`.


@section conclusion Conclusion

This concludes the tutorial. For additional help or for more details,
please consult the documentation. Now go and have fun writing the
command-line program of your dreams!
