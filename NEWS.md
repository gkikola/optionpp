Option++ Release Notes
======================

Option++ 1.01 (TBD)
--------------------------

Additions:

* Add conversion functions `arg_to_int`, `arg_to_unsigned`, `arg_to_long`,
  and `arg_to_double` to `Option` class.
* Add `OptionParser::program_cmd` member function to return the command used
  to invoke the program (argv[0])

Changes:

* Add `const` qualifier to `OptionParser::print_usage` and
  `OptionParser::usage`
* Do not include `argv[0]` in `OptionParser::program_args()`; that is,
  ignore the command used to invoke the program when populating the
  `program_args()` container
* Do case-insensitive sort for options in usage message
* Do not sort option descriptions unless a usage message is requested
* Fix problem with error message for invalid options not showing correct
  option name


Option++ 1.0 (2017-03-12)
-------------------------

Initial release
