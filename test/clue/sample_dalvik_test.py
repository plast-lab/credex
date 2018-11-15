from androidctl import *
import sys


if __name__=='__main__':
    emu = emulator()
    emu.wait_for('device',timeout=10)
    ret = emu.dalvikvm(['org.junit.runner.JUnitCore','clue.testing.SampleClass'],
                   "sample-classes.dex", "junit.dex")
    
    sys.exit(ret.returncode)

