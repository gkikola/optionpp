@page example_programs Example Programs

@brief Various example programs, each consisting of a single file.

The examples can also be found in `docs/examples` under the root
project directory.

@tableofcontents

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

An unoptimized and highly-simplified version of the Unix `grep`
utility for regular expression pattern matching.

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

@section example_dos DOS-Style Options

This shows how to customize the strings that the parser interprets as
option indicators.

@include dos.cpp

Sample input/output:

```
prompt$ example_dos
Acceptable options:
  /F:(size)     Format disk to specific size
  /N:(sectors)  Specify number of sectors per track on the disk
  /T:(tracks)   Specify number of tracks on the disk
  /B            Reserve space on disk to later copy system files
  /S            Copy operating system files to the disk after formatting
  /Q            Quick format - erases file allocation table but does not
                  identify bad sectors
prompt$ example_dos /F:720 /N:9 /T:80 /S
Formatting disk of size 720 Kb with 9 sectors per track and 80 tracks
Copying operating system files...
Done
```
