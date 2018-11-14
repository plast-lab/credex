#
# Android SDK controller
#
# The purpose of this code is to provide tooling for running
# tests on android devices (real or emulated).
#

import os, io
import atexit
import telnetlib
import re
import time
import subprocess as sp
from pathlib import Path, PurePosixPath
from contextlib import closing
from signal import SIGTERM

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

    def devices(self):
        out = sp.check_output([self.cmd(*ADB),'devices','-l'])
        devlist = []
        with io.StringIO(out.decode('utf-8')) as output:
            topline = output.readline()
            assert topline.startswith("List of devices")
            for line in output.readlines():
                splitline = line.split()
                if not splitline:  # skip empty lines
                    continue
                assert len(splitline)>=2
                serial = splitline[0]
                state = splitline[1]
                attrs = dict(frag.split(':',maxsplit=1)
                             for frag in splitline[2:])
                yield serial,state,attrs

    def device(self, serial):
        """Return a device object by serial, as reported by adb"""
        if serial.startswith("emulator-"):
            port = int(serial[9:])
            return Emulator(port=port, sdk=self)
        else:
            return Device(serial, sdk=self)

    def __new_emulator_at_port(self, avd, port):
        # Check if port is available
        try:
            con = EmulatorConsole(port)
            con.close()
            return None
        except ConnectionRefusedError:
            pass
        except:
            return None

        return Emulator.start(avd, port, self)
        
    def emulator(self, avd):
        """Return an Emulator object for a given avd"""

        if avd not in self.get_avds():
            raise ValueError("Cannot find AVD "+str(avd))
        
        # Try to find an emulator for given avd
        # by scanning the 64 even ports starting from
        # 5554
        for port in range(5554,5554+128,2):
            try:
                with closing(EmulatorConsole(port)) as con:
                    con.auth()
                    if con.avd==avd:
                        return Emulator(port)
            except:
                pass

        for port in range(5554,5554+128,2):
            emu = self.__new_emulator_at_port(avd, port)
            if emu is not None:
                return emu

        return None
        
        
    def get_avds(self):
        """Return a list of installed AVD names"""
        out = sp.check_output([self.cmd(*EMULATOR),'-list-avds'])
        return [l.rstrip(b"\r\n").decode('utf-8')
                for l in io.BytesIO(out).readlines()]
    
    def cmd(self, *pc):
        return os.path.join(self.root, *pc)

SDK = None
try:
    SDK = AndroidSdk()
    emulator = SDK.emulator
    device= SDK.device
    devices = SDK.devices
    get_avds = SDK.get_avds
    cmd = SDK.cmd    
except:
    pass


class Device:
    """
    Instances of this class encapsulate an android device
    (real or emulated) connected to this machine.

    Methods support accessing the device (via adb) to 
    perform various operations.
    """
    
    def __init__(self, serial=None, sdk=SDK):
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

        def append_args(args):
            for arg in args:
                if isinstance(arg, (tuple, list)):
                    append_args(arg)
                else:
                    cmdlist.append(str(arg))        
        
        append_args(args)
        return cmdlist

    def state(self):
        cmd = self.adb("get-state")
        return sp.check_output(cmd, universal_newlines=True).rstrip()

    def shell(self, *args, **kwargs):
        cmd = self.adb('shell', '-n', args)
        return sp.run(cmd, universal_newlines=True, **kwargs)
        
    
    def shell_output(self, *args, **kwargs):
        """
        Execute an adb shell command.

        If the command exits with an exit status other than 0 (failure)
        the method throws an exception. Otherwise, the output of the
        shell command is returned.
        """

        cmd = self.adb('shell', '-n', args)
        return sp.check_output(cmd, universal_newlines=True, **kwargs)

    def push(self, local_paths, remote_path, sync=False, **run_args):
        """
        Push local files to the remote path.

        'local_paths' can be either a str/bytes or an iterable of such.
        'remote_path' must be a str/bytes object
        """
        try:
            if os.path.exists(local_paths):
                local_paths = [local_paths]
        except:
            pass
        finally:
            Local = [str(name) for name in local_paths]
        return sp.run(self.adb('push', Local, remote_path), **run_args)
        
    
    def dalvikvm(self, cls, *dexen, **run_kwargs):
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
        tmp_path = self.shell_output('mktemp',
                              '-p','/data/local/tmp',
                              '-d', 'credex.XXXXXX').strip()

        classpath = ':'.join(str(PurePosixPath(tmp_path,dex))
                             for dex in dex_names)

        # Push local .dex files to the temp directory created
        self.push(dex_local, tmp_path)

        # Execute dalvikvm on the temp directory
        rcmd = self.shell('dalvikvm', '-cp', classpath, cls, **run_kwargs)

        # Clean up temp path
        self.shell('rm', '-r', tmp_path)

        # Return the result object of the dalvikvm execution
        return rcmd

    

class EmulatorConsole:
    def __init__(self, port=None):
        self.avd = None
        self.port = port
        self.__auth = False
        self.tn = telnetlib.Telnet()
        if port is not None:
            self.open()

    def open(self, port=None):
        if self.avd is not None:
            raise RuntimeError("The session is already open!")
        if port is None:
            port = self.port
        if port is None:
            raise ValueError("No port has been specified")
        self.tn.close()
        self.tn.open('localhost', port)
        self.port = port

        self.tn.read_until(b"OK\r\n")
        self.avd = self.check_cmd('avd name')
        self.__auth = False
        return self.avd

    def close(self):
        self.tn.close()
        self.avd = None

    def __del__(self):
        self.close()

    ROK = re.compile(b"OK\r\n")
    RKO = re.compile(b"KO:.*\r\n")
    EXPECT_LIST = [ROK,RKO]
                
    def cmd(self, *args):
        # Create and send command
        def flatten_args(args):
            for arg in args:
                if isinstance(arg, (list,tuple)):
                    yield from flatten_args(arg)
                else:
                    yield (arg if isinstance(arg,bytes)
                             else bytes(str(arg),encoding='ascii'))

        cmd = b" ".join(flatten_args(args))
        self.tn.write(cmd)
        self.tn.write(b"\r\n")

        # Get output. Each response ends with either a line
        # OK\r\n
        #  or a line
        # KO:<error>\r\n

        rspno, rspmatch, rspdata = self.tn.expect(self.EXPECT_LIST)

        # Decode output
        # Return a pair, where 1st item is None or <error>
        # and 2nd item is the response text (maybe empty)

        resplines = rspdata[:rspmatch.start()].\
                    rstrip(b"\r\n").decode('utf-8')

        if rspno==0:
            resperr = None
        else:
            resperr = rspdata[rspmatch.start()+3:-2].decode('utf-8')
        
        return (resperr, resplines)

    def check_cmd(self, *args):
        rerr, rlines = self.cmd(*args)
        if rerr:
            raise RuntimeError(rerr,rlines)
        return rlines

    __call__ = check_cmd
    
    def auth(self):
        # Get the authentication token
        auth_token_file = os.path.join(os.getenv("HOME"),
                                       ".emulator_console_auth_token")
        with open(auth_token_file) as f:
            auth_token = f.readline()
        
        # Start by authenticating
        retval =  self.check_cmd("auth",auth_token)
        self.__auth = True
        return retval

    def kill(self):
        if not self.__auth: self.auth()
        return self.check_cmd("kill")
    def ping(self):
        return self.check_cmd("ping")

    

class Emulator(Device):
    def __init__(self, port, sdk=SDK):
        self.port = port
        # Check to see if the port is a legal emulator
        # console port

        with closing(EmulatorConsole(port)) as con:
            # throw on failure, which may mean that
            # the current user is not the owner of the
            # emulator
            con.auth()
            self.avd = con.avd
            
        serial = ("emulator-%d" % int(port))
        super().__init__(serial, sdk)

    def console(self):
        return EmulatorConsole(self.port)

    def stop(self):
        with closing(self.console()) as con:
            con.auth()
            con.cmd("kill")
    
    @classmethod
    def start(cls, avd, port, sdk=SDK):
        cmd = [sdk.cmd(*EMULATOR),
               '-avd', avd, '-port', str(port),
               '-no-audio', '-no-window','-no-boot-anim']
        proc = sp.Popen(cmd,
                        stdin=sp.DEVNULL,
                        stdout=sp.DEVNULL, stderr=sp.DEVNULL)

        # loop to connect, check status
        check_flag = False
        try_loops = 0
        while proc.poll() is None:
            try:
                # Try to connect console to verify
                with closing(EmulatorConsole(port)) as con:
                    if con.avd!=avd:
                        # Oops, something else is listening here!
                        proc.send_signal(SIGTERM)
                    else:
                        con.auth()
                        # If there is no error,
                        check_flag = True
            except:
                pass

            try_loops += 1 
            if check_flag or try_loops>20:
                break

            # wait for next loop
            time.sleep(0.2)
            
        if check_flag:
            return Emulator(port, sdk)
        else:
            return None



if __name__=='__main__':
    emu = AndroidSdk().emulator('myavd1')
    print("Android SDK=", emu.sdk.root)
    

    



