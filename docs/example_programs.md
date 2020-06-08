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

@section example_mygrep A grep Clone - Full Working Example

@include mygrep.cpp

Sample input/output:

```
prompt$ echo 'And you may find yourself' >> input.txt
prompt$ echo 'Living in a shotgun shack' >> input.txt
prompt$ echo 'And you may find yourself' >> input.txt
prompt$ echo 'In another part of the world' >> input.txt
prompt$ echo 'And you may find yourself' >> input.txt
prompt$ echo 'Behind the wheel of a large automobile' >> input.txt
prompt$ example_mygrep -E 'shack|auto' input.txt
Living in a shotgun shack
Behind the wheel of a large automobile
prompt$ example_mygrep -E 'shack|auto' --count input.txt
2
prompt$ example_mygrep 'WORLD' input.txt
prompt$ example_mygrep 'WORLD' --ignore-case input.txt
In another part of the world
prompt$ example_mygrep -E '^a' -i --invert-match input.txt
Living in a shotgun shack
In another part of the world
Behind the wheel of a large automobile
```
