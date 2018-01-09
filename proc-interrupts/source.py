from threading import Thread
from time import sleep

def print_LOC():
    interrupts = open("/proc/interrupts", "r")
    for line in interrupts.readlines():
        if line.find("LOC") != -1:
            print(line.strip())
            break

def threaded_function(arg):
    sleep(1)

if __name__== "__main__":
    print_LOC()
    thread = Thread(target = threaded_function, args = (0, ))

    thread.start()
    thread.join()
    print_LOC()

    sleep(1)
    print_LOC()