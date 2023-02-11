
AC_INIT([RhpcBLASctl],[0.23],[nakama@ki.rim.or.jp])

: ${R_HOME=`R RHOME`}
if test -z "${R_HOME}"; then
  echo "could not determine R_HOME"
  exit 1
fi
CC=`"${R_HOME}/bin/R" CMD config CC`
CFLAGS=`"${R_HOME}/bin/R" CMD config CFLAGS`
CPPFLAGS=`"${R_HOME}/bin/R" CMD config CPPFLAGS`


AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([src/config.h])

AC_CHECK_LIB([dl],[dlopen],[LIBDL=-ldl],[])
LIBDL="${LIBDL}"


AC_CHECK_HEADERS([sys/sysctl.h])

AC_CHECK_FUNCS( [sysctl sysctlbyname] )
AC_CHECK_HEADERS([sys/sysinfo.h])
AC_CHECK_FUNCS( [get_nprocs] )

AC_CONFIG_FILES([src/Makevars])


AC_SUBST([LIBDL])

AC_OUTPUT
