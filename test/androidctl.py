#
# Android SDK controller
#
# The purpose of this code is to provide tooling for running
# tests on android devices (real or emulated).
#

import os, io
import subprocess as sp
import threading
from pathlib import Path, PurePosixPath


EMULATOR = ('emulator','emulator')
ADB = ('platform-tools','adb')


class AndroidSdk:
    """A wrapper to an installed SDK"""
    def __init__(self, ah=None):
        # The deprecated envvar
        if ah is None:
            ah = os.getenv('ANDROID_HOME')

        # The newer envvar 
        if ah is None:
            ah = os.getenv('ANDROID_SDK_ROOT')

        # This is where Android Studio installs the SDK
        if ah is None:
            ah = os.path.join(os.getenv('HOME'),'Android','Sdk')

        # This is a non-standard default
        if not os.path.isdir(ah):
            ah = os.path.join(os.getenv('HOME'),'android')

        if not os.path.isdir(ah):
            raise RuntimeError("Cannot locate Android SDK. Please set ANDROID_SDK_ROOT")

        self.root = ah

    def emulator(self, avd):
        """Return a new Emulator for this sdk"""
        return Emulator(self, avd)

    def cmd(self, *pc):
        return os.path.join(self.root, *pc)
    
    
class Emulator:
    """
    Instances of this class encapsulate an AVD and 
    methods to execute it (in an android emulator) and
    communicate with it via adb.
    """
    
    def __init__(self, sdk: AndroidSdk, avd: str):
        self.sdk = sdk
        self.avd = avd
        self.out = ""
        # Private: execution thread 
        self.__thread = None
        self.__ctxc = 0

    def __enter__(self):
        if self.__ctxc==0:
            self.start()
        self.__ctxc += 1
    def __exit__(self,exc_type,exc_value,tb):
        self.__ctxc -= 1
        if self.__ctxc==0:
            self.stop()            
        
    def start(self):
        """Start the emulator if it is not running"""
        if self.__thread is None:
            self.__thread = threading.Thread(target=self.__run)
            self.__thread.start()
            sp.check_call([self.sdk.cmd(*ADB),'wait-for-device'])

            
    def __bool__(self):
        """Return true if the emulator is running"""
        return self.__thread is not None

    def shell(self, *args, **kwargs):
        with self:
            cmd = [self.sdk.cmd(*ADB),'shell']
            cmd.extend(args)
            return sp.check_output(cmd, universal_newlines=True, **kwargs)
    
    def dalvikvm(self, cls, *dexen):
        """\
        Run a class in dalvikvm, using local .dex files as classpath.

        Loads a number of .dex files into the emulator and executes
        class `cls`. The local .dex files are provided as strings, and
        are user-expanded, i.e.,  '~/foo.dex' is legal.

        Example:
        emu.dalvikvm('foo.MyClass', '~/classes.dex', '~/mylib.dex')
        """
        if len(dexen)==0:
            raise ValueError("No .dex files are provided!")
        dex_local = [Path(dex).expanduser() for dex in dexen]
        for dex in dex_local:
            if not dex.is_file():
                raise ValueError(str(dex)+" is not a legal dex file")
        dex_names = [str(dex.name) for dex in dex_local]

        with self:
        
            # Create temp path
            tmp_path = self.shell('mktemp',
                                  '-p','/data/local/tmp',
                                  '-d', 'credex.XXXXXX').strip()

            classpath = ':'.join(str(PurePosixPath(tmp_path,dex))
                                 for dex in dex_names)
        
            adb = self.sdk.cmd(*ADB)
            sp.run([adb, 'push']+[str(dex) for dex in dex_local]
                   +[tmp_path])
            rcmd = sp.run([adb, 'shell', 'dalvikvm', '-cp', classpath, cls],
                          stdout=sp.PIPE, stderr=sp.PIPE)

            # Clean up temp path
            self.shell('rm', '-r', tmp_path)
            return rcmd
        
    def stop(self):
        """Stop the emulator if it is running"""
        if self:
            adb = self.sdk.cmd(*ADB)
            sp.run([adb, 'emu', 'kill'])
            self.__thread.join()
            self.out += self.__thread.runner.stdout
            self.__thread = None
        return self.out

    def __repr__(self):
        return "Emulator[%s,%s]" % (self.avd,
                                    "running" if self else "stopped")

    def __run(self):
        """\
        Private: this is called in a separate thread to execute 
        the emulator """
        cmd = [self.sdk.cmd(*EMULATOR),
               '-avd', self.avd,
               '-no-audio', '-no-window']
        threading.current_thread().runner =  sp.run(cmd,
                              universal_newlines=True,
                              stdout=sp.PIPE,stderr=sp.STDOUT)

    

if __name__=='__main__':
    sdk = AndroidSdk()
    print("Android SDK=", sdk.root)
    emu = sdk.emulator('myavd1')
    

    



