// Copyright (C) 2002 Andy Adler
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, see <http://www.gnu.org/licenses/>.

#include <octave/oct.h>
#include <octave/file-io.h>

DEFUN_DLD (mark_for_deletion, args,,
"mark_for_deletion ( filename1, filename2, ... );\n\
put filenames in the list of files to be deleted\n\
when octave terminates.\n\
This is useful for any function which uses temprorary files.")
{
  octave_value retval;
  for ( int i=0; i< args.length(); i++) {
    if( ! args(i).is_string() ) {
      error ("mark_for_deletion: arguments must be string filenames");
      return retval;
    } else {
      mark_for_deletion( args(i).string_value() );
    }
  }
  return retval;
}
