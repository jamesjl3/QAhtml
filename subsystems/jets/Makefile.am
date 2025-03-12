AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(OFFLINE_MAIN)/rootmacros \
  -isystem$(ROOTSYS)/include \
  -isystem$(OPT_SPHENIX)/include

lib_LTLIBRARIES = \
  libqadrawjet.la

libqadrawjet_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lqadrawclient \
  -lqadrawdb \
  -lemcNoisyTowerFinder \
  -lodbc++

jetincludedir=$(pkgincludedir)/jet

jetinclude_HEADERS = \
  JetDraw.h 

libqadrawjet_la_SOURCES = \
  JetDraw.cc 

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = \
  testexternals.cc

testexternals_LDADD = \
  libqadrawjet.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
