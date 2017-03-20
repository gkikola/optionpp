Option++ Release Notes
======================

Option++ 1.01 (TBD)
--------------------------

Changes:

* Add `const` qualifier to `OptionParser::print_usage` and
  `OptionParser::usage`
* Do not include `argv[0]` in `OptionParser::program_args()`; that is,
  ignore the command used to invoke the program when populating the
  `program_args()` container
* Add `program_cmd` function to `OptionParser` to return the command used
  to invoke the program
* Do case-insensitive sort for options in usage message
* Fix problem with error message for bad options not showing correct
  option name


Option++ 1.0 (2017-03-12)
-------------------------

Initial release
