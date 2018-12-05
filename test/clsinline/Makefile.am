
AM_CXXFLAGS= -g --std=gnu++14 -Wall
#AM_CXXFLAGS= -g -O3 --std=gnu++14 -Wall

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

cls_test_SOURCES = ClsTest.cpp ClsInlinePass.cpp ClsInliner.cpp
cls_test_LDADD = $(TEST_LIBS)
cls_test_LDFLAGS= $(TEST_LDFLAGS)

EXTRA_cls_test_DEPENDENCIES = \
   trivial.dex trivial-test.dex \
   @top_builddir@/libcredex.la

%.dex: %.jar
	$(DX) --dex --output=$@ $^

%-test.jar:
	echo Making test $*
	/usr/bin/test -d $*/test
	-rm -r $*/build
	mkdir -p $*/build
	$(JAVAC) $(JAVACFLAGS) -cp $(CLASSPATH):$*.jar -d $*/build `find $*/test -name "*.java"`
	jar cf $@ -C $*/build .
	-rm -r $*/build

%.jar:
	echo "Making normal jar"
	/usr/bin/test -d $*/java
	-rm -r $*/build
	mkdir -p $*/build
	$(JAVAC) $(JAVACFLAGS) -cp $(CLASSPATH) -d $*/build `find $*/java -name "*.java"`
	jar cf $@ -C $*/build .
	-rm -r $*/build

trivial.jar: trivial/java/HostClass.java trivial/java/RsrcClass.java
trivial-test.jar: trivial/test/TrivialTest.java trivial.jar

#
# Test programs plus utilities
#
check_PROGRAMS = cls_test
