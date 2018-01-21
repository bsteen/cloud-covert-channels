import matplotlib.pyplot as plt

data = []
try:
    data_file = open("sink_raw_recording.txt", "r")
except IOError:
    exit(1)

for line in data_file:
    data.append(line)
data_file.close()

plt.plot(data)
plt.ylabel("FreeMeme kb")
plt.xlabel("index #")
plt.show()
exit(0)