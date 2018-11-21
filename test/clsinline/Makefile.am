
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
# Libraries to link with the tests
#
TEST_LIBS = \
  @top_builddir@/test/libcluetesting.la \
  @top_builddir@/test/libgtest_main.la \
  @top_builddir@/libredex.la \
  $(BOOST_FILESYSTEM_LIB) \
  $(BOOST_SYSTEM_LIB)


TEST_LDFLAGS= \
 -Wl,--whole-archive,--library-path=@top_builddir@/.libs,-lcredex,--no-whole-archive \
 -rdynamic


#
# The list of tests to run
#
TESTS =  cls_test 
VERBOSE=1

#
# Java variables and targets
# 

CLASSPATH=$(subst $() $(),:,$(wildcard @top_srcdir@/test/java/*.jar))

#
#  cls_test definitions
#

cls_test_SOURCES = ClsTest.cpp
cls_test_LDADD = $(TEST_LIBS)
cls_test_LDFLAGS= $(TEST_LDFLAGS)


# EXTRA_sample_test_DEPENDENCIES = sample-classes.dex @top_builddir@/libcredex.la

# sample-classes.jar: testing/SampleClass.java
#	mkdir -p sample-classes
#	$(JAVAC) $(JAVACFLAGS) -cp $(CLASSPATH) -d sample-classes $^
#	jar cf $@ -C sample-classes .

# export JAVA=@JAVA@
#sample-classes.dex: sample-classes.jar
#	$(DX) --dex --output=$@ $^



#
# Test programs plus utilities
#
check_PROGRAMS = cls_test
