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

DEFUN_DLD (unpackfields, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} unpackfields (@var{s_name}, @var{fld1}, @var{fld2}, @dots{})\n\
Create variables from fields of a struct.\n\
\n\
Inserts the named fields @var{fld1}, @var{fld2}, @dots{}, from the struct\n\
named @var{s_name}, into the current scope.  Noe that @var{s_name} is the\n\
name of the struct in the current scope, not the struct itself.\n\
\n\
@example\n\
@group\n\
unpackfields (\"struct\", \"var1\", \"var2\")\n\
@end group\n\
@end example\n\
\n\
is equivalent to the code:\n\
@example\n\
@group\n\
  var1 = struct.var1;\n\
  var2 = struct.var2;\n\
          :          \n\
@end group\n\
@end example\n\
but more efficient and more concise.\n\
\n\
@seealso{getfield, getfields, packfields, struct}\n\
@end deftypefn")
{
  int nargin = args.length ();

  if (nargin > 0)
    {
      std::string struct_name = args (0).string_value ();
      string_vector fld_names(nargin-1);

      if (! error_state && ! valid_identifier (struct_name))
        error ("unpackfields: invalid variable name: %s", struct_name.c_str ());

      for (octave_idx_type i = 0; i < nargin-1; i++)
        {
          if (error_state)
            break;

          std::string fld_name = args(i+1).string_value ();

          if (error_state)
            break;

          if (valid_identifier (fld_name))
            fld_names(i) = fld_name;
          else
            error ("unpackfields: invalid field name: %s", fld_name.c_str ());
        }

      if (! error_state)
        {
          // Force the symbol to be inserted in caller's scope.
          octave_value struct_val = symbol_table::varval (struct_name);

          if (struct_val.is_map ())
            {
              // Fast code for a built-in struct.
              const octave_scalar_map map = struct_val.scalar_map_value ();

              if (! error_state)
                {
                  // Do the actual work.
                  for (octave_idx_type i = 0; i < nargin-1; i++)
                    {
                      octave_scalar_map::const_iterator iter = map.seek (fld_names(i));
                      if (iter != map.end ())
                        symbol_table::assign (fld_names(i), map.contents (iter));
                      else
                        {
                          error ("unpackfields: field %s does not exist", fld_names(i).c_str ());
                          break;
                        }
                    }
                }
              else
                error ("unpackfields: structure must have singleton dimensions");
            }
          else if (struct_val.is_defined ())
            {
              // General case.
              std::list<octave_value_list> idx (1);

              for (octave_idx_type i = 0; i < nargin-1; i++)
                {
                  idx.front () = args(i+1); // Save one string->octave_value conversion.
                  octave_value val = struct_val.subsref (".", idx);

                  if (error_state)
                    break;

                  if (val.is_defined ())
                    symbol_table::assign (fld_names(i), val);
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
%! s.foo = "hello";
%! s.bar = 42;
%! unpackfields ("s", "foo", "bar");
%! assert (foo, "hello");
%! assert (bar, 42);
*/
