import subprocess
import enum
import getopt
import random
import sys
import regex
import math
import os
import filecmp

class Version_pos(enum.Enum):
    ZCURVE_MAGIC = 0
    ZCURVE = 1

class Version_at(enum.Enum):
    ZCURVE_MAGIC = 0
    ZCURVE_LOOKUP_16BIT = 1
    ZCURVE_LOOKUP_8BIT = 2
    ZCURVE_LOOKUP_4BIT = 3
    ZCURVE = 4

class Version_multi(enum.Enum):
    ZCURVE_MAGIC_SIMD = 0
    ZCURVE_LOOKUP_SIMD_16BIT = 1
    ZCURVE_SIMD = 3
    ZCURVE_MULTITHREADED = 7

OPTION = ""
DEGREE = 3
ARGUMENTS = ["",""]
PRINT = False
TESTS = 1
ZCURVE_PROGRAM = "zcurve"
VERSION_POS = 0
VERSION_AT = 0


def print_help():
    print(f""" --------- Test.py ---------
    -p \t Test the function, which calculates the index of a given x and y coordinate. 
    <x> \t Explicitly specify the x coordinate. The first number is always the X value. "_" or no argument for random value.
    <y> \t Explicitly specify the y coordinate. The second number is always the Y value. "_" or no argument for random value.

    -i[<index>] \t Test the function, which calculates the x and y coordinate of a given index (Optional: Explicitly specify the index)

    -m \t Test if the multithreaded and simd versions produce the same result

    -d \t Grad (Default: {DEGREE})
    -t \t Anzahl an Tests (Default: {TESTS})
    -h \t printing help message
    -s \t Print the result of the test
    """)
    exit(0)

def get_positional_arguments():
    global ARGUMENTS
    amount = 0
    indexes_to_remove = []
    for i in range(1, len(sys.argv)):
        if sys.argv[i] == "_":
            amount += 1
            indexes_to_remove.append(i)
        if sys.argv[i].isnumeric() and  amount < 2 and sys.argv[i-1] != "-d" and sys.argv[i-1] != "-t":
            ARGUMENTS[amount] = sys.argv[i]
            indexes_to_remove.append(i)
            amount += 1

    for i in range(0, len(indexes_to_remove)):
        sys.argv.pop(indexes_to_remove[i]-i)

def test_index_to_coordinates():
    global PRINT
    global ARGUMENTS
    global DEGREE
    global TESTS

    print(ARGUMENTS)
    values = []
    index_used = {}
    index = ""
    if ARGUMENTS[0] != "":
        index = f"{ARGUMENTS[0]}"
    if TESTS > 4**DEGREE:
        print(f"Error: Degree is too low for {TESTS} tests")
        exit(1)

    for i in range(0,TESTS):
        if ARGUMENTS[0] == "":
            index = random.randint(0,4**DEGREE-1)
            while index_used.get(index) != None:
                index = random.randint(0,4**DEGREE-1)
            index_used[index] = True

        for version in Version_at:
            output = subprocess.check_output([f"./zcurve", f"-V{version.value}", f"-d{DEGREE}", "-i", f"{index}"])
            output = regex.findall(r"\(([^)]+)\)", output.decode("utf-8"))
            values.append(output)
        for o in range(1, len(values)):
            if values[0] != values[o]:
                print("Error: Different results")
                for l in range(0, len(values)):
                    print(f"{Version_at(l).name}: {values[l]}")
                exit(1)
        if PRINT:
            print(f"Test {i} passed for {index} transposed to {values[0]}")
        values = []

def test_coordinates_to_index():
    global PRINT
    global ARGUMENTS
    global DEGREE
    global TESTS

    values = []
    x_val_used = {}
    y_val_used = {}
    x = ""
    y = ""
    amount_args = 0

    # Check, if x and y are given
    if ARGUMENTS[0] != "":
        x = f"{ARGUMENTS[0]}"
        amount_args += 1
    if ARGUMENTS[1] != "":
        y = f"{ARGUMENTS[1]}"
        amount_args += 1
    
    # Check if degree is too low for the amount of tests
    # If only one argument is given, the amount of tests is limited by the square root of the amount of possible values
    if amount_args == 1 and TESTS > math.sqrt(4**DEGREE):
        print(f"Error: Degree is too low for {TESTS} tests")
        exit(1)
    elif amount_args == 0 and TESTS > 4**DEGREE:
        print(f"Error: Degree is too low for {TESTS} tests")
        exit(1)

    for i in range(0,TESTS):
        if ARGUMENTS[0] == "":
            x = random.randint(0,math.sqrt(4**DEGREE)-1)
            while x_val_used.get(x) != None:
                x = random.randint(0,math.sqrt(4**DEGREE)-1)
            x_val_used[x] = True
        if ARGUMENTS[1] == "":
            y = random.randint(0,math.sqrt(4**DEGREE)-1)
            while y_val_used.get(y) != None:
                y = random.randint(0,math.sqrt(4**DEGREE)-1)
            y_val_used[y] = True

        for version in Version_pos:
            output = subprocess.check_output([f"./zcurve", f"-V{version.value}", f"-d{DEGREE}", "-p", f"{x}", f"{y}"])
            output = output.decode("utf-8").split(" ")[9].split("\n")[0]
            values.append(output)

        for o in range(0, len(values)):
            if values[0] != values[o]:
                print("WARNING: Different results found for X: {x} Y: {y}")
                for c, v in enumerate(Version_pos):
                    print(f"{v.name}: {values[c]}")
                exit(1)
        if PRINT:
            print(f"Test {i} passed for X: {x} Y: {y} transposed to {values[0]}")
        values = []

def test_multi():
    global DEGREE
    global TESTS

    for i in Version_multi:
        print("Generating SVG for " + i.name)
        subprocess.call([f"./zcurve", f"-V{i.value}", f"-d{DEGREE}" ,f"-s", f"{i.name}.svg"], stdout=open(os.devnull, "w"), stderr=subprocess.STDOUT)
    for j in Version_multi:
        if j == Version_multi.ZCURVE_MAGIC_SIMD.name:
            continue
        if filecmp.cmp(f"{Version_multi.ZCURVE_MAGIC_SIMD.name}.svg", f"{j.name}.svg") == False:
            print(f"Error: {Version_multi.ZCURVE_MAGIC_SIMD.name} and {j.name} are not the same")
            exit(1)

    for i in Version_multi:
        os.remove(f"{i.name}.svg")
    print("All tests passed!")

def recompile():
    global ZCURVE_PROGRAM
    if os.path.exists(ZCURVE_PROGRAM):
        os.remove(ZCURVE_PROGRAM)
    os.system(f"make {ZCURVE_PROGRAM}")


if __name__ == "__main__":
    get_positional_arguments()
    try:
        opts, args = getopt.getopt(sys.argv[1:],"spmid:t:h")
    except getopt.GetoptError:
        print_help()
    try:
        version_tmp = 0
        for i in opts:
            if i[0] == '-p':
                OPTION = "-p"
            elif OPTION == "" and i[0] == '-i':
                OPTION = "-i"
            elif OPTION == "" and i[0] == '-m':
                OPTION = "-m"
            elif i[0] == '-V':
                version_tmp = int(i[1])
            elif i[0] == '-d':
                DEGREE = int(i[1])
            elif i[0] == '-t':
                TESTS = int(i[1])
            elif i[0] == '-s':
                PRINT = True
            elif i[0] == '-h':
                print_help()
    except ValueError:
        print("Error: Invalid argument")
        print_help()

    if OPTION == "":
        print("Error: No option specified")
        print_help()

    recompile()
    if OPTION == "-i":
        test_index_to_coordinates()
    elif OPTION == "-p":
        test_coordinates_to_index()
    elif OPTION == "-m":
        test_multi()