// Copyright (C) 2009 VZLU Prague
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
#include <octave/utils.h>
#include <octave/symtab.h>
#include <octave/oct-map.h>

DEFUN_DLD (packfields, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} packfields (@var{s_name}, @var{var1}, @var{var2}, @dots{})\n\
Create struct from variables.\n\
\n\
Inserts the named variables @var{var1}, @var{var2}, @dots{} as fields into\n\
the struct named @var{s_name}.  If it does not exist, a struct with that name\n\
is created.\n\
\n\
This is equivalent to the code:\n\
@example\n\
@group\n\
  s_name.var1 = var1;\n\
  s_name.var2 = var2;\n\
          :          \n\
@end group\n\
@end example\n\
but more efficient and more concise.\n\
\n\
@seealso{setfield, setfields, struct, unpackfields}\n\
@end deftypefn")
{
  int nargin = args.length ();

  if (nargin > 0)
    {
      std::string struct_name = args (0).string_value ();
      string_vector fld_names(nargin-1);
      octave_value_list fld_vals(nargin-1);

      if (! error_state && ! valid_identifier (struct_name))
        error ("packfields: invalid variable name: %s", struct_name.c_str ());

      for (octave_idx_type i = 0; i < nargin-1; i++)
        {
          if (error_state)
            break;

          std::string fld_name = args(i+1).string_value ();

          if (error_state)
            break;

          if (valid_identifier (fld_name))
            {
              fld_names(i) = fld_name;
              octave_value fld_val = symbol_table::varval (fld_name);
              if (fld_val.is_defined ())
                fld_vals(i) = fld_val;
              else
                error ("packfields: variable %s not defined", fld_name.c_str ());
            }
          else
            error ("packfields: invalid field name: %s", fld_name.c_str ());
        }

      if (! error_state)
        {
          // Force the symbol to be inserted in caller's scope.
          symbol_table::symbol_record& rec = symbol_table::insert (struct_name);

          octave_value& struct_ref = rec.varref ();

          // If not defined, use struct ().
          if (! struct_ref.is_defined ())
            struct_ref = octave_scalar_map ();

          if (struct_ref.is_map ())
            {
              // Fast code for a built-in struct.
              octave_scalar_map map = struct_ref.scalar_map_value ();

              if (! error_state)
                {
                  // Do the actual work.
                  struct_ref = octave_value (); // Unshare map.
                  for (octave_idx_type i = 0; i < nargin-1; i++)
                    map.assign (fld_names(i), fld_vals(i));
                  struct_ref = map;
                }
              else
                error ("packfields: structure must have singleton dimensions");
            }
          else
            {
              // General case.
              struct_ref.make_unique ();
              std::list<octave_value_list> idx (1);

              for (octave_idx_type i = 0; i < nargin-1; i++)
                {
                  idx.front () = args(i+1); // Save one string->octave_value conversion.
                  struct_ref = struct_ref.subsasgn (".", idx, fld_vals (i));

                  if (error_state)
                    break;
                }
            }
        }
    }
  else
    print_usage ();

  return octave_value_list ();
}

/*
%!test
%! foo = "hello";
%! bar = 42;
%! packfields ("s", "foo", "bar");
%! assert (s, struct ("foo", "hello", "bar", 42));
*/
