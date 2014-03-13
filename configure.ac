AC_INIT(configure.ac)

AC_CONFIG_HEADER(src/config.h)

AC_CHECK_HEADERS([sys/sysctl.h])
AC_CHECK_FUNCS( [sysctl sysctlbyname] )
AC_CHECK_HEADERS([sys/sysinfo.h])
AC_CHECK_FUNCS( [get_nprocs] )

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
