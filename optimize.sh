#!/bin/sh
FLAGS='--sign --gdb'
OUT_APK=app-out.apk
INPUT_APK=./apks/Signal-play-debug-4.12.3.apk 
CONFIG=devirt.config
PROGUARD_CONFIGS="-P ./proguard_configs/proguard-android.txt"

ANDROID_SDK=~/Android/Sdk/ ./credex $INPUT_APK --config config/$CONFIG  $PROGUARD_CONFIGS -o $OUT_APK  $FLAGS
du -B 1024 $INPUT_APK
du -B 1024 $OUT_APK