/*

Copyright (C) 2014   Thomas Vasileiou

This file is part of LTI Syncope.

LTI Syncope is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LTI Syncope is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LTI Syncope.  If not, see <http://www.gnu.org/licenses/>.

H-infinity optimal controller using modified Glover's and Doyle's
formulas (continuous-time).
Uses SLICOT SB10AD by courtesy of NICONET e.V.
<http://www.slicot.org>

Author: Thomas Vasileiou <thomas-v@wildmail.com>
Created: January 2014
Version: 0.1

*/

#include <octave/oct.h>
#include <f77-fcn.h>
#include "common.h"

extern "C"
{ 
    int F77_FUNC (sb10ad, SB10AD)
                 (int& JOB,
                  int& N, int& M, int& NP,
                  int& NCON, int& NMEAS,
                  double& GAMMA,
                  double* A, int& LDA,
                  double* B, int& LDB,
                  double* C, int& LDC,
                  double* D, int& LDD,
                  double* AK, int& LDAK,
                  double* BK, int& LDBK,
                  double* CK, int& LDCK,
                  double* DK, int& LDDK,
                  double* AC, int& LDAC,
                  double* BC, int& LDBC,
                  double* CC, int& LDCC,
                  double* DC, int& LDDC,
                  double* RCOND,
                  double& GTOL, double& ACTOL,
                  int* IWORK, int& LIWORK,
                  double* DWORK, int& LDWORK,
                  bool* BWORK, int& LBWORK,
                  int& INFO);
}

// PKG_ADD: autoload ("__sl_sb10ad__", "__control_slicot_functions__.oct");    
DEFUN_DLD (__sl_sb10ad__, args, nargout,
   "-*- texinfo -*-\n\
Slicot SB10AD Release 5.0\n\
No argument checking.\n\
For internal use only.")
{
    int nargin = args.length ();
    octave_value_list retval;
    
    if (nargin != 9)
    {
        print_usage ();
    }
    else
    {
        // arguments in
        Matrix a = args(0).matrix_value ();
        Matrix b = args(1).matrix_value ();
        Matrix c = args(2).matrix_value ();
        Matrix d = args(3).matrix_value ();
        
        int ncon = args(4).int_value ();
        int nmeas = args(5).int_value ();
        double gamma = args(6).double_value ();
        double gtol = args(7).double_value ();
        double actol = args(8).double_value ();
        
        int n = a.rows ();      // n: number of states
        int m = b.columns ();   // m: number of inputs
        int np = c.rows ();     // np: number of outputs
        
        int lda = max (1, a.rows ());
        int ldb = max (1, b.rows ());
        int ldc = max (1, c.rows ());
        int ldd = max (1, d.rows ());
        
        int ldak = max (1, n);
        int ldbk = max (1, n);
        int ldck = max (1, ncon);
        int lddk = max (1, ncon);
        
        int ldac = max (1, 2*n);
        int ldbc = max (1, 2*n);
        int ldcc = max (1, np-nmeas);
        int lddc = max (1, np-nmeas);
        
        int job = 1;
        
        // arguments out
        Matrix ak (ldak, n);
        Matrix bk (ldbk, nmeas);
        Matrix ck (ldck, n);
        Matrix dk (lddk, nmeas);
        Matrix ac (ldac, 2*n);
        Matrix bc (ldbc, m-ncon);
        Matrix cc (ldcc, 2*n);
        Matrix dc (lddc, m-ncon);
        ColumnVector rcond (4);
        
        // workspace
        
        int m2 = ncon;
        int m1 = m - m2;
        int np2 = nmeas;
        int np1 = np - np2;
        int nd1 = np1 - m2;
        int nd2 = m1 - np2;
        
        int liwork = max (2*max (n, m-ncon, np-nmeas, ncon, nmeas), n*n);
        int lw1 = n*m + np*n + np*m + m2*m2 + np2*np2;
        int lw2 = max ((n + np1 + 1)*(n + m2) +
                  max (3*(n + m2) + n + np1, 5*(n + m2)),
                      (n + np2)*(n + m1 + 1) +
                  max (3*(n + np2) + n + m1, 5*(n + np2)),
                      m2 + np1*np1 +
                  max (np1*max (n, m1), 3*m2 + np1, 5*m2),
                      np2 + m1*m1 +
                  max (max (n, np1)*m1, 3*np2 + m1, 5*np2));
        int lw3 = max (nd1*m1 + max (4*min (nd1, m1) + max (nd1,m1),
                      6*min (nd1, m1)), np1*nd2 +
                  max (4*min (np1, nd2) + max (np1, nd2),
                                    6*min (np1, nd2)));
        int lw4 = 2*m*m + np*np + 2*m*n + m*np + 2*n*np;
        int lw5 = 2*n*n + m*n + n*np;
        int lw6 = max (m*m + max (2*m1, 3*n*n +
                  max (n*m, 10*n*n + 12*n + 5)),
                      np*np + max (2*np1, 3*n*n +
                  max (n*np, 10*n*n + 12*n + 5)));
        int lw7 = m2*np2 + np2*np2 + m2*m2 +
                  max (nd1*nd1 + max (2*nd1, (nd1 + nd2)*np2),
                      nd2*nd2 + max (2*nd2, nd2*m2), 3*n,
                      n*(2*np2 + m2) +
                  max (2*n*m2, m2*np2 +
                  max (m2*m2 + 3*m2, np2*(2*np2 + m2 + max (np2, n)))));
        int ldwork = lw1 + max (1, lw2, lw3, lw4, lw5 + max (lw6,lw7));
        int lbwork = 2*n;

        OCTAVE_LOCAL_BUFFER (int, iwork, liwork);
        OCTAVE_LOCAL_BUFFER (double, dwork, ldwork);
        OCTAVE_LOCAL_BUFFER (bool, bwork, lbwork);
        
        // error indicator
        int info;


        // SLICOT routine SB10AD
        F77_XFCN (sb10ad, SB10AD,
                 (job,
                  n, m, np,
                  ncon, nmeas,
                  gamma,
                  a.fortran_vec (), lda,
                  b.fortran_vec (), ldb,
                  c.fortran_vec (), ldc,
                  d.fortran_vec (), ldd,
                  ak.fortran_vec (), ldak,
                  bk.fortran_vec (), ldbk,
                  ck.fortran_vec (), ldck,
                  dk.fortran_vec (), lddk,
                  ac.fortran_vec (), ldac,
                  bc.fortran_vec (), ldbc,
                  cc.fortran_vec (), ldcc,
                  dc.fortran_vec (), lddc,
                  rcond.fortran_vec (),
                  gtol, actol,
                  iwork, liwork,
                  dwork, ldwork,
                  bwork, lbwork,
                  info));

        if (f77_exception_encountered)
            error ("hinfsyn: __sl_sb10ad__: exception in SLICOT subroutine SB10AD");

        static const char* err_msg[] = {
            "0: successful exit",
            "1: the matrix [A-j*omega*I, B2; C1, D12] had "
                "not full column rank in respect to the tolerance EPS",
            "2: the matrix [A-j*omega*I, B1; C2, D21] "
                "had not full row rank in respect to the tolerance EPS",
            "3: the matrix D12 had not full column rank in "
                "respect to the tolerance SQRT(EPS)",
            "4: the matrix D21 had not full row rank in respect "
                "to the tolerance SQRT(EPS)",
            "5: the singular value decomposition (SVD) algorithm "
                "did not converge (when computing the SVD of one of the matrices "
                "[A, B2; C1, D12], [A, B1; C2, D21], D12 or D21)",
            "6: the controller is not admissible (too small value "
                "of gamma)",
            "7: the X-Riccati equation was not solved "
                "successfully (the controller is not admissible or "
                "there are numerical difficulties)",
            "8: the Y-Riccati equation was not solved "
                "successfully (the controller is not admissible or "
                "there are numerical difficulties)",
            "9: the determinant of Im2 + Tu*D11HAT*Ty*D22 is "
                "zero [3]",
            "10: there was numerical problems when estimating"
                "the singular values of D1111, D1112, D1111', D1121'",
            "11: the matrices Inp2 - D22*DK or Im2 - DK*D22"
                "are singular to working precision",
            "12: a stabilizing controller cannot be found"};

        error_msg ("hinfsyn", info, 12, err_msg);

     
        // return values
        retval(0) = ak;
        retval(1) = bk;
        retval(2) = ck;
        retval(3) = dk;
        retval(4) = ac;
        retval(5) = bc;
        retval(6) = cc;
        retval(7) = dc;
        retval(8) = gamma;
        retval(9) = rcond;
    }
    
    return retval;
}
