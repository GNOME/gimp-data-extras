dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.54)

m4_define([gimp_data_extras_major_version], [2])
m4_define([gimp_data_extras_minor_version], [0])
m4_define([gimp_data_extras_micro_version], [5])
m4_define([gimp_data_extras_real_version],
          [gimp_data_extras_major_version.gimp_data_extras_minor_version.gimp_data_extras_micro_version])
m4_define([gimp_data_extras_version], [gimp_data_extras_real_version])

AC_INIT([GIMP Extra Data Files], [gimp_data_extras_version],
        [https://gitlab.gnome.org/GNOME/gimp-data-extras/issues/new],
        [gimp-data-extras])

AC_CONFIG_SRCDIR([brushes/Splatters/flower.gbr])

AM_INIT_AUTOMAKE(no-define dist-bzip2)
AM_MAINTAINER_MODE

PKG_CHECK_MODULES(gimp, gimp-2.0)
GIMP_DATA_DIR=`$PKG_CONFIG gimp-2.0 --variable=gimpdatadir`

AC_SUBST(GIMP_DATA_DIR)

dnl Output the Makefiles
AC_CONFIG_FILES([
Makefile
brushes/Makefile
brushes/Fun/Makefile
brushes/Geometry/Makefile
brushes/Splatters/Makefile
brushes/Texture/Makefile
patterns/Makefile
patterns/Legacy/Makefile
scripts/Makefile
])

AC_OUTPUT