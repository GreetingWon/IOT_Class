from platformio.project.config import ProjectConfig
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import numpy as np
import serial
import sys, signal, getopt

width = 50
short_options = 'ht:w:'
long_options = ['title=','width=','help']
title = 'Serial Data Plot'
data=[]
data_label=[]
# Handle Ctrl-C
def sighandler(signum, frame):
    print('Ctrl-C pressed')
    exit(9)
signal.signal(signal.SIGINT, sighandler)

# This is called by animation and plots the graph
def animate(self):
    ax.clear()
    line = ser.readline().split()
    if len(line) > len(data_label):
        j = len(line) - len(data_label)
        k = 0
        while j > k:
            data_label.append('data' + str(k + 1))
            k = k + 1

    i = 0
    for d in line:
        if len(data) <= i:
            data.append([])
        data[i].append(float(d))
        data[i] = data[i][-width:]              # truncate to the graph width
        ax.plot(data[i], label=data_label[i])
        i = i + 1

    plt.title(title)
    plt.xticks(rotation=90, ha='right')
    plt.legend()
    plt.axis([0, width, 0, None]) #Use for arbitrary number of trials
    plt.grid(color='gray', linestyle='dotted', linewidth=1)
    fig.tight_layout(pad=2.5)

# main start
try:
    arguments, data_label = getopt.getopt(sys.argv[1:], short_options, long_options)
except getopt.error as err:
    print (str(err))
    sys.exit(1)

for arg, val in arguments:
    if arg in ('-w', '--width'):
        width = int(val)
    elif arg in ('-t', '--title'):
        title = val
    elif arg in ('-h', '--help'):
        print('\nUsage:')
        print('\n\t python {} [-h] [-w 100] [-t ChartName] dataName1 dataName2 ...'.format(sys.argv[0]))
        print('\t\tor')
        print('\n\t python {} [--help] [--width=100] [--title=ChartName] dataName1 dataName2 ...\n'.format(sys.argv[0]))
        exit()

ser = serial.Serial()
ser.timeout = 10
config = ProjectConfig.get_instance()  # PIO project config
for s in config.sections():
    ser.port = config.get(s, 'monitor_port')
    ser.baudrate = config.get(s, 'monitor_speed')
if ser.port == None or ser.baudrate == None:
    print("Please check the platformio.ini for the 'monitor_port")
    exit(2)

ser.open()
if ser.is_open==True:
	print('\nSerial port listening:')
	print('\tport: {}, baud: {}\n'.format(ser.port, ser.baudrate))

fig = plt.figure()
fig.canvas.manager.set_window_title(ser.port)
ax = fig.subplots()
ani = animation.FuncAnimation(fig, animate,  interval=1000)
plt.show()
