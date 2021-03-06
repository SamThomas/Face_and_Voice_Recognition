MKOCTFILE ?= mkoctfile

ifndef LAPACK_LIBS
LAPACK_LIBS := $(shell $(MKOCTFILE) -p LAPACK_LIBS)
endif
ifndef BLAS_LIBS
BLAS_LIBS := $(shell $(MKOCTFILE) -p BLAS_LIBS)
endif
ifndef FLIBS
FLIBS := $(shell $(MKOCTFILE) -p FLIBS)
endif
LFLAGS := $(shell $(MKOCTFILE) -p LFLAGS) $(LAPACK_LIBS) $(BLAS_LIBS) $(FLIBS)

all: __control_slicot_functions__.oct \
     is_real_scalar.oct \
     is_real_vector.oct \
     is_real_matrix.oct \
     is_real_square_matrix.oct \
     is_matrix.oct

# TODO: Private oct-files for control package.

# unpack and compile SLICOT library
# Note that TG04BX is a custom routine.
# It has the extension .fortran such that
# it is not deleted by rm *.f when using
# the developer makefile makefile_control.m
slicotlibrary.a: slicot.tar.gz
	tar -xzf slicot.tar.gz
	mkdir sltmp
	mv slicot/src/*.f ./sltmp
	mv slicot/src_aux/*.f ./sltmp
	cp TG04BX.fortran ./sltmp/TG04BX.f
	cd sltmp; $(MKOCTFILE) -c *.f
	ar -rc slicotlibrary.a ./sltmp/*.o
	rm -rf sltmp slicot

# slicot functions
__control_slicot_functions__.oct: __control_slicot_functions__.cc slicotlibrary.a
	LFLAGS="$(LFLAGS)" \
    $(MKOCTFILE) __control_slicot_functions__.cc common.cc slicotlibrary.a

# helpers
is_real_scalar.oct: is_real_scalar.cc
	$(MKOCTFILE) is_real_scalar.cc

is_real_vector.oct: is_real_vector.cc
	$(MKOCTFILE) is_real_vector.cc

is_real_matrix.oct: is_real_matrix.cc
	$(MKOCTFILE) is_real_matrix.cc

is_real_square_matrix.oct: is_real_square_matrix.cc
	$(MKOCTFILE) is_real_square_matrix.cc

is_matrix.oct: is_matrix.cc
	$(MKOCTFILE) is_matrix.cc

clean:
	rm -rf *.o core octave-core *.oct *~ *.f slicot sltmp

realclean: clean
	rm -rf *.a