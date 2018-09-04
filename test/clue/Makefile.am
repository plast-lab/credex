
AM_CXXFLAGS = --std=gnu++14

#
# Include paths for tests
#
AM_CPPFLAGS = \
	-I$(top_srcdir)/test/googletest-release-1.7.0/include \
	-I$(top_srcdir)/test/common \
	-I$(top_srcdir)/liblocator \
	-I$(top_srcdir)/libredex \
	-I$(top_srcdir)/libresource \
	-I$(top_srcdir)/libresource/android \
	-I$(top_srcdir)/libresource/androidfw \
	-I$(top_srcdir)/libresource/cutils \
	-I$(top_srcdir)/libresource/system \
	-I$(top_srcdir)/libresource/utils \
	-I$(top_srcdir)/opt \
	-I$(top_srcdir)/opt/all-static \
	-I$(top_srcdir)/opt/annokill \
	-I$(top_srcdir)/opt/bridge \
	-I$(top_srcdir)/opt/constant_propagation \
	-I$(top_srcdir)/opt/delinit \
	-I$(top_srcdir)/opt/delsuper \
	-I$(top_srcdir)/opt/final_inline \
	-I$(top_srcdir)/opt/interdex \
	-I$(top_srcdir)/opt/local-dce \
	-I$(top_srcdir)/opt/peephole \
	-I$(top_srcdir)/opt/rebindrefs \
	-I$(top_srcdir)/opt/remove_empty_classes \
	-I$(top_srcdir)/opt/renameclasses \
	-I$(top_srcdir)/opt/shorten-srcstrings \
	-I$(top_srcdir)/opt/simpleinline \
	-I$(top_srcdir)/opt/singleimpl \
	-I$(top_srcdir)/opt/static-sink \
	-I$(top_srcdir)/opt/staticrelo \
	-I$(top_srcdir)/opt/synth \
	-I$(top_srcdir)/opt/unterface \
	-I$(top_srcdir)/tools/redex-all \
	-I$(top_srcdir)/util \
	-I$(top_srcdir)/shared \
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
