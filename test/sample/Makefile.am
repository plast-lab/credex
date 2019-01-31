
AM_CXXFLAGS= -g3 -O3 --std=gnu++14 -Wall

#
# Include paths for tests
#

AM_CPPFLAGS = -I@top_srcdir@/includes -I/usr/include/jsoncpp
AM_CPPFLAGS += -I@top_srcdir@/test/googletest-release-1.7.0/include 
AM_CPPFLAGS += -I@top_srcdir@/test/clue

#
# Set up for dalvik tests
#
include ../python-tests.am.inc

#
# The list of tests to run
#
TESTS = sample_test sample_dalvik_test.py
VERBOSE=1

#
# Libraries to link with the tests
#
TEST_LIBS = \
	@top_builddir@/test/libcluetesting.la \
	@top_builddir@/test/libgtest_main.la \
	-lcredex


#
# Java variables and targets
# 

CLASSPATH=$(subst $() $(),:,$(wildcard @top_srcdir@/test/java/*.jar))

#
#  sample_test definitions
#

sample_test_SOURCES = SampleTest.cpp

sample_test_LDADD = $(TEST_LIBS) \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB)

#sample_test_LDFLAGS=\
# -Wl,--whole-archive,--library-path=@top_builddir@/.libs,-lcredex,--no-whole-archive \
# -rdynamic

sample_test_LDFLAGS=  -L@top_builddir@   \
 -rdynamic


EXTRA_sample_test_DEPENDENCIES = sample-classes.dex

sample-classes.jar: testing/SampleClass.java
	mkdir -p sample-classes
	$(JAVAC) $(JAVACFLAGS) -cp $(CLASSPATH) -d sample-classes $^
	jar cf $@ -C sample-classes .


export JAVA=@JAVA@

sample-classes.dex: sample-classes.jar
	$(DX) --dex --output=$@ $^


sample_dalvik_test.py: sample-classes.dex


#
# Test programs plus utilities
#
check_PROGRAMS = sample_test
check_SCRIPTS = sample_dalvik_test.py
