AC_INIT(src/blasctl.c)

YEARSTAMP=$(date '+%y')
YDAYSTAMP=$(date '+%j.%H')
DATESTAMP=$(date '+%Y-%m-%d')

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


AC_SUBST(YEARSTAMP)
AC_SUBST(YDAYSTAMP)
AC_SUBST(DATESTAMP)

AC_SUBST(OMP_CFLAGS)
AC_SUBST(OMP_LDFLAGS)

AC_OUTPUT(src/Makevars)

AC_OUTPUT(src/dust/DESCRIPTION)
mv src/dust/DESCRIPTION DESCRIPTION

