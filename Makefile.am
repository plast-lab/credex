AUTOMAKE_OPTIONS = foreign

SUBDIRS = . test

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = --std=gnu++14 -O3 -Wall -g3

# comment right before redex_srcdir
redex_srcdir= @srcdir@/redex

#------------------------------------------------------------
# Include paths
#
# Path ./includes is a directory with symlinks to all header files
# (and some directories). This way, we avoid having extremely long
# sequences of -I options to the compiler.
#
# The ./includes directory is built automatically by ./configure.
# It can be rebuilt (without a full configuration) by running
#
#   ./config.status generate_includes
#
#------------------------------------------------------------

AM_CPPFLAGS= -I@top_srcdir@/includes -I/usr/include/jsoncpp

#
#  Build artifacts:
#  libredex.a 			Contains code that is linked as a library.
#
#  libredex_opt.a  		Contains the code of the FB/redex passes.
#
#  libcredex_passes.a 	Contains code that consists of credex passes
#
#  libredex_tools.a   	Contains code from redex/tools needed to link
#                  		executables
#
#  libcredex.a  		Contains code that is linked with --whole-archive passed
#               		to the linker. This is because redex passes are registered
#                       inside the constructors of unreferenced global objects
#                       (an unfortunate decision IMHO) so that a library-style link
#                       would not include the passes in the build.
#
#                       In fact, this file is made by concatenating two other files,
#                       libcredex_passes.a and libredex_opt.a 
#

noinst_LTLIBRARIES= libredex_opt.la libcredex_passes.la

lib_LTLIBRARIES = libcredex.la libredex.la libredex_tools.la

# 
# libredex.a   
#

include Libredex.am.inc

libredex_la_LIBADD = \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_REGEX_LIB) \
	$(BOOST_IOSTREAMS_LIB) \
	$(BOOST_THREAD_LIB) \
	-lpthread \
	-ldl

#
# libcredex.a
#

include RedexObj.am.inc

libcredex_passes_la_SOURCES=\
	passes/remover/Remover.cpp \
	passes/plast-devirt/PlastDevirtualizationPass.cpp \
	passes/plast-devirt/PlastDVParser.cpp

libredex_tools_la_SOURCES= \
	@top_srcdir@/redex/tools/common/ToolsCommon.cpp


libcredex_la_SOURCES=
libcredex_la_LIBADD=\
	libredex_opt.la \
	libcredex_passes.la \
	libredex.la

#---------------------------------
# Build targets
#---------------------------------

#
# credex-all   is not really installable, since it is wrapped inside
#              the 'credex' self-extracting file
#

noinst_PROGRAMS = credex-all

credex_all_SOURCES = \
	@top_srcdir@/redex/tools/redex-all/main.cpp

EXTRA_credex_all_DEPENDENCIES= libcredex.la


credex_all_LDADD = \
	libredex_tools.la \
	libredex.la \
	$(BOOST_PROGRAM_OPTIONS_LIB) 

credex_all_LDFLAGS = \
	-all-static \
	 -Wl,--whole-archive,--library-path=@top_builddir@/.libs,./.libs/libcredex.a,--no-whole-archive \
	-rdynamic # function names in stack traces

#
# credex: Python driver script
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


#
# seeder tool
#

bin_PROGRAMS= seeder

seeder_SOURCES = \
	@top_srcdir@/tools/seeder/main_seed.cpp

seeder_LDADD = \
	libredex.la \
	libredex_tools.la \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_PROGRAM_OPTIONS_LIB)
