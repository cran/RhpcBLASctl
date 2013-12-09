AC_INIT(src/blasctl.c)

R_HOME=${R_HOME}
CC=$(${R_HOME}/bin/R CMD config CC 2>/dev/null)
case "$CC" in
        *xl*)
             OMP_CFLAGS="-qsmp=omp"
             OMP_LDFLAGS="-qsmp=omp"
        ;;
        *path*)
             OMP_CFLAGS="-mp"
             OMP_LDFLAGS="-mp"
        ;;
        *pgcc*)
             OMP_CFLAGS="-mp"
             OMP_LDFLAGS="-mp"
        ;;
        *icc*)
             OMP_CFLAGS="-openmp"
             OMP_LDFLAGS="-openmp"
        ;;
        *sun*)
             OMP_CFLAGS="-xopenmp=parallel"
             OMP_LDFLAGS="-xopenmp=parallel"
        ;;
        *gcc*)
             OMP_CFLAGS="-fopenmp"
             OMP_LDFLAGS="-fopenmp"
        ;;
esac


AC_SUBST(OMP_CFLAGS)
AC_SUBST(OMP_LDFLAGS)

AC_OUTPUT(src/Makevars)
