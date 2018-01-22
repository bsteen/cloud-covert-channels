import matplotlib.pyplot as plt

data = []
try:
	data_file = open("FreeMem_data.txt", "r")
except IOError:
	print("\tCould not find FreeMem_data.txt")
	exit(1)

for line in data_file:
	data.append(line)
data_file.close()

plt.plot(data[2:], color="black", linestyle='-', label="FreeMem values")
plt.axhline(y=data[0], color='red', linestyle='--', label="ZERO_UPPER_LIMIT")
plt.axhline(y=data[1], color='blue', linestyle='--', label="ONE_UPPER_LIMIT")

plt.ylabel("FreeMem kb")
plt.xlabel("index #")
plt.title("FreeMem Data Recorded by Sink")
plt.legend(loc='upper right')

plt.savefig("FreeMem_data.png")
# plt.show()
plt.close()
exit(0)