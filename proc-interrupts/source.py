from pynput.keyboard import Key, Controller

def print_kb_interrupt():
    interrupts = open("/proc/interrupts", "r")
    interrupts.readline()
    interrupts.readline()
    print(interrupts.readline().strip())
    interrupts.close()

if __name__== "__main__":
    keyboard = Controller()

    print_kb_interrupt()

    for x in range (10):
        exec(open("./kb-interrupt").read())

    print_kb_interrupt()