import matplotlib.pyplot as plt

y_data = []
zero_x_indexes = []
zero_y_indexes = []
one_x_indexes = []
one_y_indexes = []


try:
    data_file = open("output/FreeMem_values.txt", "r")
except IOError:
    print("\tCould not find FreeMem_values.txt")
    exit(1)

try:
    index_file = open("output/One-Zero_indexes.txt", "r")
except IOError:
    print("\tCould not find indexes.txt")
    data_file.close()
    exit(1)

for line in data_file:
    y_data.append(int(line.strip()))
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

plt.plot(y_data[2:], color="black", linestyle='-', label="FreeMem values")
plt.scatter(zero_x_indexes, zero_y_indexes, color="red", marker='.', label="0 bit detected")
plt.scatter(one_x_indexes, one_y_indexes, color="blue", marker='.', label="1 bit detected")
plt.axhline(y=y_data[0], color='red', linestyle='--', label="Zero Cutoff")
plt.axhline(y=y_data[1], color='blue', linestyle='--', label="One Cutoff")

plt.ylabel("FreeMem (kB)")
plt.xlabel("index #")
plt.title("FreeMem Values Recorded by Sink")
plt.legend(loc='upper right')

plt.savefig("output/FreeMem_values.png")
# plt.show()
plt.close()
exit(0)