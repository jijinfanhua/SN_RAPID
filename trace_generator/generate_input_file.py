# generate input file for packet generator

# input format:
#   1. number of packets
#   2. time
#   3. number of flows
#   4-4+number of flows:  flow id, arrival rate of each flow

with open("input.txt", "w") as f:
    f.write(str(int(5e3)) + '\n')
    f.write(str(int(1e4)) + '\n')
    f.write("5000\n")
    for i in range(5000):
        f.write(str(i) + " " + str(1) + "\n")
