#
# SYNOPSIS
#
#   ANDROID_SDK
#
# DESCRIPTION
#
#   Test for the existence of Android SDK installation and exports
#   variables for build tools.
#   This macro calls:
#
#     AC_SUBST(DX)
#
#   And sets:
#
#     HAVE_BOOST_LOG
#
# LICENSE
#
#   Copyright (c) 2018 Vasilis Samoladas <vsam@softnet.tuc.gr>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([ANDROID_SDK],
[
	AC_ARG_WITH([android-sdk],
		AS_HELP_STRING([--with-android-sdk@<:@=special-lib@:>@],
		[path to the build tools from the provided Android SDK installation
			e.g. --with-android-sdk=$HOME/Android/Sdk]), [
			android_sdk="$withval"
		], [android_sdk="auto"]
	)

	echo "Given Android SDK root=" /${android_sdk}/

	if test "x$android_sdk" = "xauto"; then
	# Try to use the locate database
	  android_sdk="`locate -0 --regex 'platform-tools/adb$' | xargs -0 -r dirname | xargs -r dirname | head -n 1`"
	fi

	echo "Android SDK root=" /$android_sdk/


	if test "x${android_sdk}" = "x"; then
	   android_bt="auto"
	else
	  android_bt="`find ${android_sdk}/build-tools -mindepth 1 -maxdepth 1|head -n 1`"
	fi

	if test "x$android_bt" = "xauto"; then
	# Most linuxes have the locate database utility set.
		android_bt="`locate --regex "build-tools/.*/dx$"|xargs dirname|sort|tail -n 1`"
	fi

	# Check the path
	# TODO...

	echo "Android build tools=" $android_bt

	AC_DEFINE_UNQUOTED(WITH_ANDROID_SDK,"${android_sdk}",[the path to Android SDK])
	AC_ARG_VAR(ANDROID_SDK_ROOT,[Path to the Android SDK])
	AC_SUBST(ANDROID_SDK_ROOT,$android_sdk)
	

	AC_DEFINE_UNQUOTED(WITH_ANDROID_BUILD_TOOLS,"${android_bt}",[the path to Android SDK build tools])
	AC_ARG_VAR(ANDROID_BUILD_TOOLS,[Path to the Android SDK build tools])
	AC_SUBST(ANDROID_BUILD_TOOLS,$android_bt)
	AC_PATH_PROG([DX],[dx],
		AC_MSG_ERROR(Could not find the dx compiler in Android build tools)
	,[$android_bt])
])
