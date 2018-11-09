#
# Android SDK controller
#
# The purpose of this code is to provide tooling for running
# tests on android devices (real or emulated).
#

import os, io
import atexit
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


class Device:
    """
    Instances of this class encapsulate an android device
    (real or emulated) connected to this machine.

    Methods support accessing the device (via adb) to 
    perform various operations.
    """
    
    def __init__(self, serial=None, sdk=None):
        """
        Construct a device with given serial name and accessible
        via the provided Android SDK.

        `serial` can be None, when only one device is present.
        `sdk`    can be None, a pathname to the ANDROID_SDK_ROOT, or
                 an instance of AndroidSdk
        """
        if isinstance(sdk, AndroidSdk):
            self.sdk = sdk
        else:
            self.sdk = AndroidSdk(sdk)
        self.serial = serial

    def set_serial(self, serial):
        """
        Set the serial for this instance
        """
        self.serial = serial

    @staticmethod
    def __add_args(cmdlist, args):
        for arg in args:
            if isinstance(arg, (tuple, list)):
                Device.__add_args(cmdlist, arg)
            else:
                cmdlist.append(str(arg))        
        
    def adb(self, *args):
        """
        Return a command list for passing and adb invocation
        to the subprocess.run method (and its variants).

        For each arg which is an instance of list or tuple, its 
        elements are recursively added to the command, else the
        arg is transformed into a string and added to the command.
        """

        cmdlist = [self.sdk.cmd(*ADB)]
        if self.serial is not None:
            cmdlist.append('-s')
            cmdlist.append(self.serial)
        self.__add_args(cmdlist, args)
        return cmdlist
                
    def shell(self, *args, **kwargs):
        """
        Execute an adb shell command.

        If the command exits with an exit status other than 0 (failure)
        the method throws an exception. Otherwise, the output of the
        shell command is returned.
        """

        cmd = self.adb('shell', args)
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

        # Create temp path
        tmp_path = self.shell('mktemp',
                              '-p','/data/local/tmp',
                              '-d', 'credex.XXXXXX').strip()

        classpath = ':'.join(str(PurePosixPath(tmp_path,dex))
                             for dex in dex_names)

        # Push local .dex files to the temp directory created
        sp.run(self.adb('push', [str(dex) for dex in dex_local], tmp_path))

        # Execute dalvikvm on the temp directory
        rcmd = sp.run(self.adb('shell', 'dalvikvm', '-cp', classpath, cls),
                      stdout=sp.PIPE, stderr=sp.PIPE)

        # Clean up temp path
        self.shell('rm', '-r', tmp_path)

        # Return the result object of the dalvikvm execution
        return rcmd



    
class Emulator(Device):
    """
    An Android emulator device.

    Instances of this class encapsulate an Android emulator AVD and 
    methods to execute it (in an android emulator).

    Instances can be running or stopped; this is controlled
    in several ways:
    1) A (Python) context manager api: thus,
    ```
    with emu:
       ... do stuff
    ```
    ensures that 'stuff' is done with a running context manager,
    but exiting for any reason will stop it

    The context manager api is recursive, so that nested contexts
    can be supported.

    2) Methods start() and stop() allow running and stopping
    outside a context. By default, start() registers an atexit
    handler, so that quitting the process will also quit the 
    emulator.
    """
    
    def __init__(self, avd, port=None, sdk = None):
        self.avd = avd
        self.port = None
        serial = None if port is None else ("emulator-%d" % int(port))
        super().__init__(serial, sdk)
        
        self.out = ""
        # Private: execution thread 
        self.__thread = None
        self.__ctxc = 0

    def __enter__(self):
        if self.__ctxc==0:
            self.__start()
        self.__ctxc += 1

    def __exit__(self,exc_type,exc_value,tb):
        self.__ctxc -= 1
        if self.__ctxc==0:
            self.__stop()            
        
    def start(self, at_exit_stop=True):
        """Context manager entry."""
        #if at_exit_stop:
        #    atexit.register(self.stop)
        return self.__enter__()

    def stop(self):
        """Context manager exit"""
        print("Stopping")
        #atexit.unregister(self.stop)
        return self.__exit__(None,None,None)

    def __del__(self):
        self.__stop()
    
    def __start(self):
        """Start the emulator if it is not running"""
        if self.__thread is None:
            self.__thread = threading.Thread(target=self.__run)
            self.__thread.start()
            sp.check_call(self.adb('wait-for-device'))

    def __stop(self):
        """Stop the emulator if it is running"""
        if self.__thread is not None:
            sp.check_call(self.adb('emu', 'kill'))
            self.__thread.join()
            self.out += self.__thread.runner.stdout
            self.__thread = None
        return self.out


    def __run(self):
        """\
        Private: this is called in a separate thread to execute 
        the emulator """
        cmd = [self.sdk.cmd(*EMULATOR),
               '-avd', self.avd,
               '-no-audio', '-no-window','-no-boot-anim']
        threading.current_thread().runner =  sp.run(cmd,
                              universal_newlines=True,
                              stdout=sp.PIPE,stderr=sp.STDOUT)    
            
    def __bool__(self):
        """Return true if the emulator is running"""
        return self.__thread is not None

    def __repr__(self):
        return "Emulator(%s) [%s]" % (self.avd,
                                    "running" if self else "stopped")

    

if __name__=='__main__':
    emu = Emulator('myavd1')
    print("Android SDK=", emu.sdk.root)
    

    



