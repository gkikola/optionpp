@page example_programs Example Programs

@brief Various example programs, each consisting of a single file.

The examples can also be found in `docs/examples` under the root
project directory.

@section example_basic Introductory Example

A basic program that shows off some of Option++'s features.

@include basic.cpp

Sample input/output:

```
prompt$ example_basic hello world
Received argument 'hello'
Received argument 'world'
prompt$ example_basic hello -v --output="My File.txt"
Writing output to 'My File.txt'
Received argument 'hello'
```

