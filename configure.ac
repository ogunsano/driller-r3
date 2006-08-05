AC_INIT(Driller, 0.7, jmillikin@gmail.com)
AC_CONFIG_SRCDIR(src/driller.cpp)

dnl Find what Driller is being compiled for
AC_CANONICAL_TARGET

dnl Initialize automake
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS(config.h)

dnl Enable maintainer mode flag
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_CXX

dnl Required for everything
LIBXML_REQUIRED=2.0.0

dnl Required only when building the Qt GUI
QT_REQUIRED=4.1.0

dnl Required only when building with MySQL support
MYSQL_REQUIRED=4.0.0

AC_ARG_ENABLE(mysql,
  AS_HELP_STRING(--enable-mysql, Enable output to a MySQL database),
  [enable_mysql="$enableval"],
  [enable_mysql=no]
)

AC_ARG_WITH(mysql,
  AS_HELP_STRING(--with_mysql=<directory>, Use a custom directory for mysql),
  [extra_mysql_dir="$withval"],
  [extra_mysql_dir=""]
)

AC_ARG_ENABLE(qt,
  AS_HELP_STRING(--enable-qt, Enable building the Qt user interface),
  [enable_qt="$enableval"],
  [enable_qt=no]
)

AC_ARG_ENABLE(debug,
  AS_HELP_STRING(--enable-debug, Enables various options which aid debugging),
  [enable_debug="$enableval"],
  [enable_debug=no]
)

AC_ARG_ENABLE(optimizations,
  AS_HELP_STRING(--enable-optimizations, Enables optimization of the binary),
  [enable_optimizations="$enableval"],
  [enable_optimizations=yes]
)

dnl Test for libXML
PKG_CHECK_MODULES(libXML, libxml-2.0 >= $LIBXML_REQUIRED)

AC_MSG_CHECKING([whether to build the MySQL output module])
if test "$enable_mysql" = "yes"; then
  AC_MSG_RESULT([yes])
  AC_DEFINE(ENABLE_MYSQL, 1, Enable output to a MySQL database)

  if test -d "$extra_mysql_dir"; then
    AC_PATH_PROG(MYSQL_CONFIG, [mysql_config], [], ["${extra_mysql_dir}/bin"])
  else
    AC_PATH_PROG(MYSQL_CONFIG, [mysql_config])
  fi

  if test -z $MYSQL_CONFIG; then
    AC_MSG_ERROR([Cannot find mysql_config])
  fi

  mysql_CFLAGS="`mysql_config --cflags`"
  mysql_LIBS="`mysql_config --libs`"
else
  AC_MSG_RESULT([no])
  AC_DEFINE(ENABLE_MYSQL, 0, Enable output to a MySQL database)
fi
AM_CONDITIONAL(ENABLE_MYSQL, test "$enable_mysql" = "yes")

AC_MSG_CHECKING([whether to build the Qt GUI])
if test "$enable_qt" = "yes"; then
  AC_MSG_RESULT([yes])
  AC_DEFINE(ENABLE_QT_GUI, 1, Enable the Qt GUI)
  AC_DEFINE(ENABLE_GUI, 1, Enable building a GUI)

  PKG_CHECK_MODULES(Qt, QtGui >= $QT_REQUIRED QtCore >= $QT_REQUIRED)

  dnl Check that moc is in path
  AC_PATH_PROG(MOC, moc)
  if test -z "$MOC"; then
    AC_MSG_ERROR([*** moc must be in path])
  fi

  # uic is the Qt user interface compiler
  AC_PATH_PROG(UIC, uic)
  if test -z "$UIC"; then
    AC_MSG_ERROR([*** uic must be in path])
  fi
else
  AC_MSG_RESULT([no])

  AC_DEFINE(ENABLE_QT_GUI, 0, Enable the Qt GUI)
  AC_DEFINE(ENABLE_GUI, 0, Enable building a GUI)
fi
AM_CONDITIONAL(ENABLE_GUI, test "$enable_qt" = "yes")
AM_CONDITIONAL(ENABLE_QT_GUI, test "$enable_qt" = "yes")

FEATURES_CFLAGS="$libXML_CFLAGS $mysql_CFLAGS $Qt_CFLAGS"
FEATURES_LIBS="$libXML_LIBS $mysql_LIBS $Qt_LIBS"

AC_MSG_CHECKING(if debugging is enabled[])
if test "$enable_debug" = "yes"; then
  AC_MSG_RESULT([yes])

  DEBUG_CFLAGS="-g3 -Wall -ansi"
else
  AC_MSG_RESULT([no])

  dnl Enable building on Windows without a console window
  case "$target_os" in
	  "cygwin" | "mingw32" )
		  LIBS="$LIBS -mwindows"
	  ;;

	  *)
	  ;;
  esac

  DEBUG_CFLAGS="-s"
fi

AC_MSG_CHECKING([if optimizations are enabled])
if test "$enable_optimizations" = "yes"; then
  AC_MSG_RESULT([yes])

  OPTIMIZATION_CFLAGS="-O2 -finline-functions"
else
  AC_MSG_RESULT([no])

  OPTIMIZATION_CFLAGS="-O0"
fi

CPPFLAGS="$CFLAGS $FEATURES_CFLAGS $OPTIMIZATION_CFLAGS $DEBUG_CFLAGS"
LIBS="$LIBS $FEATURES_LIBS $OPTIMIZATION_LIBS $DEBUG_LIBS"

AC_OUTPUT([
  Makefile
  src/Makefile
])
