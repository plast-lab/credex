AUTOMAKE_OPTIONS = foreign

SUBDIRS = . test

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = --std=gnu++14 -O3 -Wall -g3

# comment right before redex_srcdir
redex_srcdir= @srcdir@/redex

#
# Include paths
#
#include Incpath.am.inc

AM_CPPFLAGS= -I@top_srcdir@/includes -I/usr/include/jsoncpp

noinst_LTLIBRARIES= libredex.la libredex_opt.la libcredex_passes.la libcredex.la

include Libredex.am.inc

libredex_la_LIBADD = \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_REGEX_LIB) \
	$(BOOST_IOSTREAMS_LIB) \
	$(BOOST_THREAD_LIB) \
	-lpthread \
	-ldl

include RedexObj.am.inc

libcredex_passes_la_SOURCES=\
	passes/remover/Remover.cpp \
	passes/plast-devirt/PlastDevirtualizationPass.cpp \
	passes/plast-devirt/PlastDVParser.cpp

# The following is a trick to force C++ linking, but it is not
# really needed, since libcredex is added manually!!!
# nodist_EXTRA_libcredex_la_SOURCES= dummy.cpp

libcredex_la_SOURCES=
libcredex_la_LIBADD=\
	libredex_opt.la \
	libcredex_passes.la


noinst_PROGRAMS = credex-all

credex_all_SOURCES = \
	@top_srcdir@/redex/tools/redex-all/main.cpp

EXTRA_credex_all_DEPENDENCIES= libcredex.la

#credex_all_LDADD = \
#	libredex.la \
#	$(BOOST_FILESYSTEM_LIB) \
#	$(BOOST_SYSTEM_LIB) \
#	$(BOOST_REGEX_LIB) \
#	$(BOOST_PROGRAM_OPTIONS_LIB) \
#	$(BOOST_THREAD_LIB) \
#	-lpthread \
#	-ldl

credex_all_LDADD = \
	libredex.la \
	$(BOOST_PROGRAM_OPTIONS_LIB) 

credex_all_LDFLAGS = \
	-Wl,--whole-archive,--library-path=.libs,-lcredex,--no-whole-archive\
	-rdynamic # function names in stack traces

#credex_all_LDFLAGS = \
#	-Wl,--whole-archive,--library-path=.libs,-lredex_opt,--no-whole-archive\
#	-Wl,--whole-archive,--library-path=.libs,-lcredex_passes,--no-whole-archive\
#	-rdynamic # function names in stack traces


#
# redex: Python driver script
#
bin_SCRIPTS = credex redex/apkutil
CLEANFILES = credex

PYTHON_SRCS := $(redex_srcdir)/redex.py \
	$(redex_srcdir)/pyredex/__init__.py \
	$(redex_srcdir)/pyredex/logger.py \
	$(redex_srcdir)/pyredex/unpacker.py \
	$(redex_srcdir)/pyredex/utils.py

credex: credex-all $(PYTHON_SRCS)
	./build-credex.sh
