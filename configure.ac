dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.54)

m4_define([gimp_data_extras_major_version], [2])
m4_define([gimp_data_extras_minor_version], [0])
m4_define([gimp_data_extras_micro_version], [5])
m4_define([gimp_data_extras_real_version],
          [gimp_data_extras_major_version.gimp_data_extras_minor_version.gimp_data_extras_micro_version])
m4_define([gimp_data_extras_version], [gimp_data_extras_real_version])

m4_define([intltool_required_version], [0.40.1])

AC_INIT([GIMP Extra Data Files], [gimp_data_extras_version],
        [https://gitlab.gnome.org/GNOME/gimp-data-extras/issues/new],
        [gimp-data-extras])

AC_CONFIG_SRCDIR([brushes/Splatters/flower.gbr])

AM_INIT_AUTOMAKE(no-define dist-bzip2)
AM_MAINTAINER_MODE

######################
# Internationalisation
######################

IT_PROG_INTLTOOL(intltool_required_version)

########################
# Check for appstreamcli
########################

AC_ARG_WITH(appdata-test,  [  --without-appdata-test  do not validate the appdata file])

have_appstreamcli="no (disabled)"
if test "x$with_appdata_test" != "xno"; then
  AC_PATH_PROG(APPSTREAM_UTIL, appstreamcli, no)
  if test "x$APPSTREAM_UTIL" != "xno"; then
    have_appstreamcli="yes"
  else
    have_appstreamcli="no (appstreamcli not found)"
  fi
fi
AM_CONDITIONAL(HAVE_APPSTREAMCLI, test "x$have_appstreamcli" = "xyes")
if test "x$have_appstreamcli" = "xyes"; then
  have_appstreamcli="yes (this test requires network access; --without-appdata-test to disable)"
fi

################
# Check for GIMP
################

AC_ARG_ENABLE(gimp3,
  [  --enable-gimp3          Install GIMP 2.99/3 data and plug-ins],,
  enable_gimp3=no)

gimp_pkg_config_name="gimp-2.0"
if test "x$enable_gimp3" = xyes; then
  gimp_pkg_config_name="gimp-3.0"
fi

PKG_CHECK_MODULES(gimp, $gimp_pkg_config_name)

if test "x$enable_gimp3" = xyes; then
  GIMP_DATA_DIR=`$PKG_CONFIG gimp-3.0 --variable=gimpdatadir`
  AC_SUBST(GIMP_DATA_DIR)

  GIMP3_PLUG_INS_DIR=`$PKG_CONFIG gimp-3.0 --variable=gimplibdir`/plug-ins
  AC_SUBST(GIMP3_PLUG_INS_DIR)
else
  GIMP_DATA_DIR=`$PKG_CONFIG gimp-2.0 --variable=gimpdatadir`
  AC_SUBST(GIMP_DATA_DIR)
fi

AM_CONDITIONAL(ENABLE_GIMP3, test "x$enable_gimp3" = xyes)

dnl Output the Makefiles
AC_CONFIG_FILES([
Makefile
brushes/Makefile
brushes/Fun/Makefile
brushes/Geometry/Makefile
brushes/Splatters/Makefile
brushes/Texture/Makefile
desktop/Makefile
desktop/org.gimp.GIMP.data_extras.metainfo.xml.in
desktop/test-appdata.sh
patterns/Makefile
patterns/Legacy/Makefile
plug-ins/Makefile
plug-ins/python3/Makefile
po/Makefile.in
scripts/Makefile
])

AC_OUTPUT

AC_MSG_RESULT([
Building gimp-data-extras with prefix=$prefix, datadir=$datadir

Test appdata: $have_appstreamcli
]);
