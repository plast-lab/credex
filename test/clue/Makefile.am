
AM_CXXFLAGS = --std=gnu++14

redex_srcdir=$(top_srcdir)/redex

#
# Include paths for tests
#
AM_CPPFLAGS = \
	-I$(redex_srcdir)/test/googletest-release-1.7.0/include \
	-I$(redex_srcdir)/test/common \
	-I$(redex_srcdir)/liblocator \
	-I$(redex_srcdir)/libredex \
	-I$(redex_srcdir)/libresource \
	-I$(redex_srcdir)/libresource/android \
	-I$(redex_srcdir)/libresource/androidfw \
	-I$(redex_srcdir)/libresource/cutils \
	-I$(redex_srcdir)/libresource/system \
	-I$(redex_srcdir)/libresource/utils \
	-I$(redex_srcdir)/opt \
	-I$(redex_srcdir)/opt/all-static \
	-I$(redex_srcdir)/opt/annokill \
	-I$(redex_srcdir)/opt/bridge \
	-I$(redex_srcdir)/opt/constant_propagation \
	-I$(redex_srcdir)/opt/delinit \
	-I$(redex_srcdir)/opt/delsuper \
	-I$(redex_srcdir)/opt/final_inline \
	-I$(redex_srcdir)/opt/interdex \
	-I$(redex_srcdir)/opt/local-dce \
	-I$(redex_srcdir)/opt/peephole \
	-I$(redex_srcdir)/opt/rebindrefs \
	-I$(redex_srcdir)/opt/remove_empty_classes \
	-I$(redex_srcdir)/opt/renameclasses \
	-I$(redex_srcdir)/opt/shorten-srcstrings \
	-I$(redex_srcdir)/opt/simpleinline \
	-I$(redex_srcdir)/opt/singleimpl \
	-I$(redex_srcdir)/opt/static-sink \
	-I$(redex_srcdir)/opt/staticrelo \
	-I$(redex_srcdir)/opt/synth \
	-I$(redex_srcdir)/opt/unterface \
	-I$(redex_srcdir)/tools/redex-all \
	-I$(redex_srcdir)/util \
	-I$(redex_srcdir)/shared \
	-I/usr/include/jsoncpp


# Rules for making the cluetesing library
noinst_LTLIBRARIES = libcluetesting.la

libcluetesting_la_SOURCES= ClueTest.cpp


#
# The list of tests to run
#
TESTS = \
	sample_test

#
# Libraries to link with the tests
#
TEST_LIBS = libcluetesting.la $(top_builddir)/test/libgtest_main.la $(top_builddir)/libredex.la

#
#  sample_test definitions
#

sample_test_SOURCES = SampleTest.cpp
sample_test_LDADD = $(TEST_LIBS)
EXTRA_sample_test_DEPENDENCIES = sample-classes.dex

sample-classes.jar: SampleClass.java
	mkdir -p sample-classes
	$(JAVAC) -d sample-classes $^
	$(JAR) cf $@ -C sample-classes .

sample-classes.dex: sample-classes.jar
	$(DX) --dex --output=$@ $^

#
# Test programs plus utilities
#
check_PROGRAMS = $(TESTS)
