from androidctl import *
import sys


if __name__=='__main__':
    emu = emulator('myavd1')
    ret = emu.dalvikvm(['org.junit.runner.JUnitCore','clue.testing.SampleClass'],
                   "sample-classes.dex", "junit.dex")
    
    sys.exit(ret.returncode)
