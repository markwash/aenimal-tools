lib_LTLIBRARIES = libsimplevector.la libaeinteger.la libaefactor.la

libsimplevector_la_SOURCES = simple_vector.c

libaeinteger_la_SOURCES = integer.c
libaeinteger_la_LIBADD = libsimplevector.la

libaefactor_la_SOURCES = factor.c
libaefactor_la_LIBADD = libsimplevector.la

include_HEADERS = simple_vector.h integer.h factor.h
