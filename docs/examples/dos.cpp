/*
 * Accepting DOS-style arguments: /A /B /F:file ...
 *
 * Not perfect: unlike the real DOS, you can't do /A/B without space
 * between the options.
 */

#include <iostream>
#include <optionpp/optionpp.hpp>

using optionpp::parser;
using optionpp::parser_result;

struct Disk {
  int size{160};
  int sectors{15};
  int tracks{80};
  bool system_reserve{};
  bool copy_os{};
  bool quick_fmt{};
};

int main(int argc, char* argv[]) {

  parser opt_parser;
  Disk disk;

  opt_parser.set_custom_strings(" \t\r\n", // Whitespace delimiters
                                "/", // Short prefix
                                "\0", // Long prefix, not used here
                                "\0", // End-of-options, not used here
                                ":"); // Argument assignment

  opt_parser['F'].argument("(size)", true).bind_int(&disk.size)
    .description("Format disk to specific size");
  opt_parser['N'].argument("(sectors)", true).bind_int(&disk.sectors)
    .description("Specify number of sectors per track on the disk");
  opt_parser['T'].argument("(tracks)", true).bind_int(&disk.tracks)
    .description("Specify number of tracks on the disk");
  opt_parser['B'].bind_bool(&disk.system_reserve)
    .description("Reserve space on disk to later copy system files");
  opt_parser['S'].bind_bool(&disk.copy_os)
    .description("Copy operating system files to the disk after formatting");
  opt_parser['Q'].bind_bool(&disk.quick_fmt)
    .description("Quick format - erases file allocation table but does not "
                 "identify bad sectors");

  parser_result result;
  try {
    result = opt_parser.parse(argc, argv);
  } catch(const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  if (result.empty()) {
    std::cout << "Acceptable options:\n";

    // Line length: 78, group-indent: 0, option-indent: 2
    // First line indent: 16, multiline indent: 18
    opt_parser.print_help(std::cout, 78, 0, 2, 16, 18) << std::endl;
    return 0;
  }

  if (disk.quick_fmt)
    std::cout << "Performing quick format...\n";
  std::cout << "Formatting disk of size "
            << disk.size << " Kb with "
            << disk.sectors << " sectors per track and "
            << disk.tracks << " tracks\n";
  if (disk.system_reserve)
    std::cout << "Reserving space for system files...\n";
  if (disk.copy_os)
    std::cout << "Copying operating system files...\n";

  std::cout << "Done" << std::endl;

  return 0;
}
