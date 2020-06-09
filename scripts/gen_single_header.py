# Generate single header for Option++
# Copyright (C) 2017-2020 Greg Kikola.
#
# This file is part of Option++.
#
# Option++ is free software: you can redistribute it and/or modify
# it under the terms of the Boost Software License version 1.0.
#
# Option++ is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# Boost Software License for more details.
#
# You should have received a copy of the Boost Software License
# along with Option++.  If not, see
# <https://www.boost.org/LICENSE_1_0.txt>.
#
# Written by Greg Kikola <gkikola@gmail.com>.

from pathlib import Path
from datetime import datetime
from datetime import timezone

_start_comment = """\
/* Option++ -- read command-line program options
 * Copyright (C) 2017-2020 Greg Kikola.'
 *
 * This file is part of Option++.
 *
 * Option++ is free software: you can redistribute it and/or modify
 * it under the terms of the Boost Software License version 1.0.
 *
 * Option++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Boost Software License for more details.
 *
 * You should have received a copy of the Boost Software License
 * along with Option++.  If not, see
 * <https://www.boost.org/LICENSE_1_0.txt>.
 */
/* Written by Greg Kikola <gkikola@gmail.com>. */

"""

_transl_units = ['error', 'utility', 'option', 'option_group', 'parser_result',\
                 'result_iterator', 'parser']

def generate():
    single_header_dir = Path('..') / Path('single_header')
    single_header_dir /= Path('optionpp')
    single_header_dir.mkdir(parents=True, exist_ok=True)

    timestamp = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%S")

    output = _start_comment
    output += '// Single-header generated ' + timestamp + 'Z\n\n'

    incl_list, content = _parse_files(header=True)

    output += incl_list + '\n'
    output += content

    incl_list, content = _parse_files(header=False)

    # Define macro so output only goes in one translation unit
    output += '\n\n#ifdef OPTIONPP_MAIN\n\n'
    output += incl_list + '\n'
    output += content
    output += '\n#endif\n#undef OPTIONPP_MAIN\n'

    with open(single_header_dir / Path('optionpp.hpp'), 'w') as file:
        file.write(output)

def _add_extension(base_name_list, extension):
    result = [name + '.' + extension for name in base_name_list]
    return result

def _parse_files(header=False):
    if header:
        incl = Path('..') / Path('include') / Path('optionpp')
        ext = 'hpp'
    else:
        incl = Path('..') / Path('src')
        ext = 'cpp'

    includes = ''
    content = ''
    for filename in _add_extension(_transl_units, ext):
        i, c = _parse_file(incl / Path(filename), header)
        includes += i + '\n'
        content += c + '\n'
    return (_remove_dupes(includes), content)

def _parse_file(filename, header=False):
    includes = ''
    content = ''
    in_comment = False
    found_content = False

    with open(filename) as file:
        for line in file:
            sline = line.strip()
            if sline.startswith('/*'): # Skip commented lines
                in_comment = True
            if in_comment:
                if sline.endswith('*/'):
                    in_comment = False
                continue
            if header and sline.startswith('#ifndef'): # Header guard
                found_content = True
                continue

            if not header and sline.startswith('using namespace optionpp'):
                found_content = True
            elif not header and sline.startswith('namespace'):
                found_content = True
            elif sline.startswith('#include <optionpp'):
                continue # Ignore local library includes
            elif sline.startswith('#include'): # Add unique includes
                includes += line
                continue
            elif header and sline.startswith('#endif'):
                break
            if found_content and not sline.startswith('#define'):
                content += line.partition('//')[0].rstrip()
                if not content.endswith('\n'):
                    content += '\n'
    return (includes, content)

def _remove_dupes(string):
    unique = set(string.splitlines())
    result = []
    for line in unique:
        result.append(line)
    result.sort()
    return '\n'.join(result)

if __name__ == "__main__":
    generate()
