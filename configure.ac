#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([credex], [0.1.0], [yannis@di.uoa.gr])

AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_MACRO_DIR([m4])
LT_INIT

# clear out default cxx flags (was "-O2 -g") so that they don't override
# the flags defined in AM_CXXFLAGS
: ${CXXFLAGS=""}

# Checks for C++ compiler
AC_PROG_CXX
AC_PROG_CC

# Checks for java compiler (used in tests)
AC_CHECK_CLASSPATH
AX_JAVA_OPTIONS
AX_PROG_JAVAC
AX_PROG_JAVA

# Checks for Android SDK (used in tests)
ANDROID_SDK

# Python is needed for the credex wrapper
AM_PATH_PYTHON([3.5], [], [AC_MSG_ERROR([CRedex requires python 3.5 or newer])])

# Checks for libraries.
AX_PTHREAD
AX_BOOST_BASE([1.58.0], [], [AC_MSG_ERROR(
              [Please install boost >= 1.58 (including filesystem)])])
AX_BOOST_FILESYSTEM
AX_BOOST_SYSTEM
AX_BOOST_REGEX
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_IOSTREAMS
AX_BOOST_THREAD
AC_CHECK_LIB([z], [adler32], [], [AC_MSG_ERROR([Please install zlib])])
AC_CHECK_LIB([jsoncpp], [main], [], [AC_MSG_ERROR([Please install jsoncpp])])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h memory.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime gettimeofday memmove memset munmap regcomp strchr strdup strerror strrchr strstr strtol])

#------------------------------------------
# We must create the includes directory
# for all redex include files
#------------------------------------------

AC_CONFIG_COMMANDS([generate_includes],[
mkdir -p includes
rm -r includes
mkdir includes
inclpath="redex/libredex redex/liblocator redex/opt redex/service redex/shared redex/sparta redex/tools redex/util passes"

find $inclpath -name "*.h" -printf "../%p\0" |xargs -0 -n 1 ln -s -t includes
find redex/libresource -mindepth 1 -maxdepth 1 -type d -name "@<:@a-z@:>@*" -printf "../%p\0"  |xargs -0 -n 1 ln -s -t includes
])


AC_CONFIG_FILES([
        Makefile        
        test/Makefile
        test/clue/Makefile
])

### This is not enabled,  --deleted: as it would interfere with the testing code
### as we are taking over redex building
## AC_CONFIG_SUBDIRS([redex])

AC_OUTPUT
