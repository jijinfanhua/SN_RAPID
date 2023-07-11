import matplotlib.pyplot as plt

x_values = []
y_values = []

with open("./packet_sequence_info.txt",'r') as f:
    for line in f:
        x, y = line.split()
        x_values.append(int(x))
        y_values.append(int(y))


plt.figure(figsize=(10,6))

plt.plot(y_values)

plt.title('Data from packet_sequence_info.txt')
plt.xlabel('X values')
plt.ylabel('Y values')
plt.grid(True)
plt.show()