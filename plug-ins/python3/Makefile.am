## Makefile.am for gimp-data-extras/plug-ins/python3

if ENABLE_GIMP3

# The benchmark_foreground_extract plug-in used to be only installed for
# unstable GIMP builds. We moved it to gimp-data-extras in case someone
# decide it was actually still useful, but won't install it, at least
# for now.

#benchmark_foreground_extractdir = $(gimpplugindir)/plug-ins/benchmark-foreground-extract
#benchmark_foreground_extract_SCRIPTS = benchmark-foreground-extract.py

py_slicedir = $(GIMP3_PLUG_INS_DIR)/py-slice
py_slice_SCRIPTS = py-slice.py

EXTRA_DIST = \
        py-slice.py

#EXTRA_DIST += benchmark-foreground-extract.py

endif
