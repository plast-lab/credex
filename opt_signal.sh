#!/usr/bin/env bash

PG_CONFIGS="-P signal_configs/proguard-retrofit.pro -P signal_configs/proguard-workmanager.pro -P signal_configs/proguard-square-okio.pro -P signal_configs/proguard-spongycastle.pro -P signal_configs/proguard-sqlite.pro -P signal_configs/proguard-retrolambda.pro -P signal_configs/proguard-jackson.pro -P signal_configs/proguard-shortcutbadger.pro -P signal_configs/proguard-webrtc.pro -P signal_configs/proguard-okhttp.pro -P signal_configs/proguard-rounded-image-view.pro -P signal_configs/proguard-dagger.pro -P signal_configs/proguard-automation.pro -P signal_configs/proguard-google-play-services.pro -P signal_configs/proguard-glide.pro -P signal_configs/proguard-ez-vcard.pro -P signal_configs/proguard-appcompat-v7.pro -P signal_configs/proguard-klinker.pro -P signal_configs/proguard-square-okhttp.pro"

PG_CONFIG_SMALL="-P signal_configs/proguard-retrofit.pro"

ANDROID_SDK=~/Android/Sdk ./credex Signal-play-debug-4.12.3.apk  $PG_CONFIGS --config config/devirt.config -o optimized.apk 
