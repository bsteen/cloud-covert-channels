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

plt.axhline(y=data[0], color='r', linestyle='-')	# ZERO_UPPER_LIMIT
plt.axhline(y=data[1], color='b', linestyle='-')	# ONE_UPPER_LIMIT
plt.plot(data[2:])

plt.ylabel("FreeMem kb")
plt.xlabel("index #")
plt.title("FreeMem Data Recorded by Sink")

plt.savefig("FreeMem_data.png")
# plt.show()
plt.close()
exit(0)