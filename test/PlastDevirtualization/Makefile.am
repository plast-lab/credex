
AM_CXXFLAGS= -g3 -O3 --std=gnu++14 -Wall

#
# Include paths for tests
#

AM_CPPFLAGS = -I@top_srcdir@/includes -I/usr/include/jsoncpp
AM_CPPFLAGS += -I@top_srcdir@/test/googletest-release-1.7.0/include
AM_CPPFLAGS += -I@top_srcdir@/test/clue
AM_CPPFLAGS += -I@top_srcdir@/passes/PlastDevirtualizationPass


#
# The list of tests to run
#
TESTS = simpleTest
#       recursionTest guardTest
VERBOSE=1

#
# Libraries to link with the tests
#
TEST_LIBS = \
  @top_builddir@/test/libcluetesting.la \
  @top_builddir@/test/libgtest_main.la \
  @top_builddir@/libredex.la


#
# Java variables and targets
#

#CLASSPATH=$(subst $() $(),:,$(wildcard @top_srcdir@/test/java/*.jar))

#
#  sample_test definitions
#

plast_devirtualization_test_SOURCES = PlastDevirtualizationTest.cpp

plast_devirtualization_test_LDADD = $(TEST_LIBS) \
  $(top_srcdir)/passes/PlastDevirtualizationPass/PlastDevirtualizationPass.o \
  $(top_srcdir)/redex/libredex/DexAsm.o

#plast_devirtualization_test_LDFLAGS=\
# -Wl,--whole-archive,--library-path=@top_builddir@/.libs,-lcredex,--no-whole-archive \
# -rdynamic

plast_devirtualization_test_LDFLAGS=\
 -rdynamic


EXTRA_plast_devirtualization_test_DEPENDENCIES = simpleTest.dex @top_builddir@/libcredex.la

sample-classes.jar: testing/SampleClass.java
	mkdir -p sample-classes
	$(JAVAC) $(JAVACFLAGS) -cp $(CLASSPATH) -d sample-classes $^
	jar cf $@ -C sample-classes .


export JAVA=@JAVA@


simpleTest.dex: simpleTest.jar
  $(DX) --dex --output=$@ $^


simpleTest.jar: SimpleTest.java
  	mkdir -p simpleTest
  	$(JAVAC) $(JAVACFLAGS) --release 8 -d simpleTest $^
  	jar cf $@ -C simpleTest .


#
# Test programs plus utilities
#
check_PROGRAMS = $(TESTS)
