AUTOMAKE_OPTIONS = foreign

SUBDIRS = . test

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = --std=gnu++14 -O3 -Wall -g

# comment right before redex_srcdir
redex_srcdir= $(srcdir)/redex

#
# Include paths
#
include Incpath.am.inc

include Libredex.am.inc

noinst_PROGRAMS = credex-all

include RedexObj.am.inc


credex_all_SOURCES = \
	$(REDEX_ALL_SRC) \
	$(top_srcdir)/redex/tools/redex-all/main.cpp \
	passes/remover/Remover.cpp \
	passes/plast-devirt/PlastDevirtualizationPass.cpp \
	passes/plast-devirt/PlastDVParser.cpp



credex_all_LDADD = \
	libredex.la \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_REGEX_LIB) \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(BOOST_THREAD_LIB) \
	-lpthread \
	-ldl

credex_all_LDFLAGS = \
	-rdynamic # function names in stack traces


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
