AC_INIT(configure.ac)

AC_CONFIG_HEADER(src/config.h)

AC_CHECK_HEADERS([sys/sysctl.h])
AC_CHECK_FUNCS( [sysctl sysctlbyname] )
AC_CHECK_HEADERS([sys/sysinfo.h])
AC_CHECK_FUNCS( [get_nprocs] )

AC_OUTPUT(src/Makevars)
