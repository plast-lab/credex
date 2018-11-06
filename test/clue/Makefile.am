
AM_CXXFLAGS = --std=gnu++14

redex_srcdir=$(top_srcdir)/redex

#
# Include paths for tests
#
include ../../Incpath.am.inc

AM_CPPFLAGS += -I$(top_srcdir)/test/googletest-release-1.7.0/include 


# Rules for making the cluetesing library
noinst_LTLIBRARIES = libcluetesting.la

libcluetesting_la_SOURCES= ClueTest.cpp


#
# The list of tests to run
#
TESTS = \
	sample_test
VERBOSE=1

#
# Libraries to link with the tests
#
TEST_LIBS = libcluetesting.la $(top_builddir)/test/libgtest_main.la \
  $(top_builddir)/libredex.la

#
#  sample_test definitions
#

sample_test_SOURCES = SampleTest.cpp
sample_test_LDADD = $(TEST_LIBS) -ldl
EXTRA_sample_test_DEPENDENCIES = sample-classes.dex

sample-classes.jar: SampleClass.java
	mkdir -p sample-classes
	$(JAVAC) $(JAVACFLAGS) -d sample-classes $^
	jar cf $@ -C sample-classes .

sample-classes.dex: sample-classes.jar
	$(DX) --dex --output=$@ $^

#
# Test programs plus utilities
#
check_PROGRAMS = $(TESTS)
