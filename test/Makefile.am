
SUBDIRS = . sample clsinline

AM_CXXFLAGS= -g3 -O3 --std=gnu++14 -Wall

check_LTLIBRARIES = libgtest_main.la  libcluetesting.la

#
# googletest
#

libgtest_main_la_CPPFLAGS = -Igoogletest-release-1.7.0 -Igoogletest-release-1.7.0/src -Igoogletest-release-1.7.0/include

libgtest_main_la_SOURCES = googletest-release-1.7.0/src/gtest-all.cc googletest-release-1.7.0/src/gtest_main.cc

#
# cluetesting
#

libcluetesting_la_CPPFLAGS = -I@top_srcdir@/includes -I/usr/include/jsoncpp -I@top_srcdir@/test/googletest-release-1.7.0/include -Iclue

libcluetesting_la_SOURCES= clue/ClueTest.cpp clue/AndroidTest.cpp

libcluetesting_la_LIBADD= \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_REGEX_LIB)



check_DATA= junit.dex

#junit_dex:
#	@(DX) --dex --output=junit.dex java/junit-4.12.jar java/hamcrest-core-1.3.jar

junit.dex: $(wildcard @top_srcdir@/test/java/*.jar)
	$(DX) --dex --output $@ $^

