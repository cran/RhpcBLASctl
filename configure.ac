AC_INIT
AC_CONFIG_SRCDIR([configure.ac])

AC_CONFIG_HEADERS([src/config.h])

AC_CHECK_HEADERS([sys/sysctl.h])
AC_CHECK_FUNCS( [sysctl sysctlbyname] )
AC_CHECK_HEADERS([sys/sysinfo.h])
AC_CHECK_FUNCS( [get_nprocs] )

AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
