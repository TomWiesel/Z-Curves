
import subprocess
import os
import getopt
import sys
import time
from enum import Enum

class Version(Enum):
    ZCURVE = 0
    ZCURVE_MULTITHREADED = 1
    ZCURVE_LOOKUP_4BIT = 2
    ZCURVE_LOOKUP_8BIT = 3
    ZCURVE_LOOKUP_16BIT = 4
    ZCURVE_MAGIC = 5
    ZCURVE_SIMD = 6
    ZCURVE_LOOKUP_SIMD_16BIT = 7
    ZCURVE_MAGIC_SIMD = 8
    MAX_IMPLEMENTATION = 9


FROM = 1
TO = 8
FILE = "Test_Bilder_Autogenerated"
INTERVALL = 1
REPEAT = 3
WIDTH = 10
HEIGHT = 10
VERSION = Version.ZCURVE
OPTIMIZATION_LEVEL = 3
CURVE_PROGRAM = "zcurve"
THREADS = 2

def print_help():
    global TO, FROM, WIDTH, HEIGHT, INTERVALL, REPEAT, VERSION, THREADS
    print(f"""------- Performance.py -------
        -f<degree>\t Starting degree value (Default: {FROM})
        -t<degree>\t Ending degree value (Default: {TO})
        -i<intervall>\t Interval for increasing degree value (Default: {INTERVALL})
        -B<repetiton>\t  Test repitions(Default: {REPEAT})
        -V<version>\t specify program version(Default: {VERSION})
        -m<threads>\t specify number of threads (Default: {THREADS})
        -O \t optimization (Default: None)
        -h\t printing help message""")
    exit(0)


if __name__ == "__main__":
    try:
        opts, args = getopt.getopt(sys.argv[1:],"f:t:i:B:V:O:hm:")
    except getopt.GetoptError:
        print_help()

    optimized = False
    for i in opts:
        if i[0] == '-f':
            FROM =  int(i[1])
            TO += 10
        elif i[0] == '-t':
            TO =  int(i[1])
        elif i[0] == '-i':
            INTERVALL = int(i[1])
        elif i[0] == '-B':
            REPEAT = int(i[1])
        elif i[0] == '-V':
            if int(i[1]) >= Version.MAX_IMPLEMENTATION.value or int(i[1]) < 0:
                print("Invalid version")
                print("Valid versions are:\n" + "\n".join([v.name + " = " + str(v.value)  for v in Version]))
                exit(1)
            VERSION = Version(int(i[1]))

        elif i[0] == '-O':
            OPTIMIZATION_LEVEL = int(i[1])
            if(OPTIMIZATION_LEVEL != 2 and OPTIMIZATION_LEVEL != 0 and OPTIMIZATION_LEVEL != 3):
                print("Invalid optimization level")
                exit(1)
            if OPTIMIZATION_LEVEL == 3:
                CURVE_PROGRAM = "zcurve"
            else:
                CURVE_PROGRAM = "zcurve_o" + str(OPTIMIZATION_LEVEL)
        elif i[0] == '-m':
            THREADS = int(i[1])
        elif i[0] == '-h':
            print_help()
    
    if os.path.exists(CURVE_PROGRAM):
        os.remove(CURVE_PROGRAM)

    os.system(f"make {CURVE_PROGRAM}")
    header = f"PROGRAM VERSION {VERSION.value}:{VERSION.name} -- OPTIMIZATION LEVEL {OPTIMIZATION_LEVEL} -- REPITITIONS {REPEAT} -- FROM {FROM} -- TO {TO} -- INTERVALL {INTERVALL}"
    if VERSION == Version.ZCURVE_MULTITHREADED:
        header += f" -- THREADS {THREADS}"

    print(header)
    for o in range(FROM, TO + 1, INTERVALL): 
        try:
            output = subprocess.check_output([f"./{CURVE_PROGRAM}",  f"-d{o}", f"-V{VERSION.value}", f"-B{REPEAT}", f"-t{THREADS}"])
            output = output.decode("UTF-8").split(" ")[5].replace(".",",")
            print(f"Degree {o}: {output}")                    
        except Exception as e:
            print("ERROR ", e)
            exit(1)
    exit(0)
