import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

config_data = []
y_data = []
zero_x_indexes = []
zero_y_indexes = []
one_x_indexes = []
one_y_indexes = []

try:
    data_file = open("output/MemFree_values.txt", "r")
except IOError:
    print("\tCould not find MemFree_values.txt")
    exit(1)

try:
    index_file = open("output/One-Zero_indexes.txt", "r")
except IOError:
    print("\tCould not find indexes.txt")
    data_file.close()
    exit(1)

for line in data_file:
    if line.find(" ") == -1:
        y_data.append(int(line.strip()))
    else:
        config_data = line.split();     # First line of file will have all config values
data_file.close()

for line in index_file:
    list = line.split()
    if int(line[0]) == 0:
        zero_x_indexes.append(int(list[1].strip()))
        zero_y_indexes.append(int(list[2].strip()))
    else:
        one_x_indexes.append(int(list[1].strip()))
        one_y_indexes.append(int(list[2].strip()))
index_file.close()

# Plots Cutoff lines
plt.axhline(y=int(config_data[1]), color='red', linestyle='-', label="Zero Cutoff", zorder=1)
plt.axhline(y=int(config_data[0]), color='red', linestyle='--', label="Zero Nominal", zorder=1)
plt.axhline(y=int(config_data[3]), color='cyan', linestyle='-', label="One Cutoff", zorder=1)
plt.axhline(y=int(config_data[2]), color='cyan', linestyle='--', label="One Nominal", zorder=1)
plt.axhline(y=int(config_data[4]), color='green', linestyle='--', label="Null Nominal", zorder=1)

# Plots all readings as a line and as a scatter plot
plt.plot(y_data, color="black", linestyle='-', label="MemFree plot", zorder=2)
plt.scatter(range(len(y_data)), y_data, color="black", marker='.', label="MemFree values", zorder=2)

# Plots detected values
plt.scatter(zero_x_indexes, zero_y_indexes, color="red", marker='.', label="0 bit detected", zorder=3)
plt.scatter(one_x_indexes, one_y_indexes, color="cyan", marker='.', label="1 bit detected", zorder=3)

# Add lables and legends
plt.xlabel("index #")
plt.ylabel("MemFree (kB)")

title = "MemFree Values Recorded by Sink\nZa=" + config_data[5] + " kB; Oa=" + config_data[6] + \
        " kB; holdt=" + config_data[7] + " us; channelt=" + config_data[8] + " s"
plt.title(title)
# plt.legend(loc='lower right')
plt.gcf().set_tight_layout(True)  # Prevent labels from being cutoff

plt.savefig("output/MemFree_graph.png")
plt.close()
exit(0)
