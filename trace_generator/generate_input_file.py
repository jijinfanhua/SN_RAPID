# generate input file for packet generator

# input format:
#   1. number of packets
#   2. time
#   3. number of flows
#   4-4+number of flows:  flow id, arrival rate of each flow

with open("input.txt", "w") as f:
    f.write(str(int(9e7)) + '\n')
    f.write(str(int(1e9)) + '\n')
    f.write("10\n")
    for i in range(10):
        f.write(str(i) + " " + str(i + 10) + "\n")
