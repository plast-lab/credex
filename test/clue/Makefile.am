
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
# Set up for dalvik tests
#
TEST_EXTENSIONS= .py
PY_LOG_COMPILER= $(PYTHON)
AM_TESTS_ENVIRONMENT= PYTHONPATH='$(top_srcdir)/test'; export PYTHONPATH;
#AM_PY_LOG_FLAGS= -v


#
# The list of tests to run
#
TESTS = sample_test sample_dalvik_test.py
VERBOSE=1

#
# Libraries to link with the tests
#
TEST_LIBS = libcluetesting.la $(top_builddir)/test/libgtest_main.la \
  $(top_builddir)/libredex.la


#
# Java variables and targets
# 

CLASSPATH=$(subst $() $(),:,$(wildcard $(top_builddir)/test/java/*.jar))

junit.dex: $(wildcard $(top_builddir)/test/java/*.jar)
	$(DX) --dex --output $@ $^

#
#  sample_test definitions
#

sample_test_SOURCES = SampleTest.cpp
sample_test_LDADD = $(TEST_LIBS) -ldl
EXTRA_sample_test_DEPENDENCIES = sample-classes.dex

sample-classes.jar: SampleClass.java
	mkdir -p sample-classes
	$(JAVAC) $(JAVACFLAGS) -cp $(CLASSPATH) -d sample-classes $^
	jar cf $@ -C sample-classes .


export JAVA=@JAVA@

sample-classes.dex: sample-classes.jar
	$(DX) --dex --output=$@ $^


sample_dalvik_test.py: sample-classes.dex junit.dex


#
# Test programs plus utilities
#
check_PROGRAMS = sample_test
check_SCRIPTS = sample_dalvik_test.py
