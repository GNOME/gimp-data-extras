## Makefile.am for gimp-data-extras/desktop

appstream_in_files = \
	org.gimp.GIMP.data_extras.metainfo.xml.in

appstream_files = $(appstream_in_files:.xml.in=.xml)

appdatadir = $(datadir)/metainfo
appdata_DATA = $(appstream_files)

EXTRA_DIST = \
	$(appstream_in_files) \
	test-appdata.sh.in


DISTCLEANFILES = $(appstream_files)

@INTLTOOL_XML_RULE@

TESTS_ENVIRONMENT = \
	chmod +x test-appdata.sh; \
	GIMP_TESTING_ABS_TOP_BUILDDIR=@abs_top_builddir@
TESTS =

if HAVE_APPSTREAMCLI
TESTS += test-appdata.sh
endif
