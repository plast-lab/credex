
#
# Set up for tests written in python
#
# Simply include this fragment into Makefile.am
# 
# include ../python-test.am.inc
#
# Then, declare a test
# TESTS= ...  mytest.py ...
#
# and declare the script
# check_SCRIPTS= ... mytest.py ...

TEST_EXTENSIONS= .py
PY_LOG_COMPILER= $(PYTHON)
AM_TESTS_ENVIRONMENT= PYTHONPATH=@abs_top_srcdir@/test; export PYTHONPATH; 
AM_TESTS_ENVIRONMENT+= PYTHON=$(PYTHON); export PYTHON; ANDROID_SDK_ROOT=$(ANDROID_SDK_ROOT); export ANDROID_SDK_ROOT;
#AM_PY_LOG_FLAGS= -v

