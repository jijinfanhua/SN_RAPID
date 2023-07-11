import random

new_f = open("pkt_200000_cycle_300000_flow_800.txt", 'w')

d = {}
with open("output3.txt", 'r') as f:
    while True:
        line = f.readline()
        if not line:
            break

        flag = 0
        arr = line.split()
        clk, idx = int(arr[0]), int(arr[1])

        if idx in d:
            d[idx] += 1
            pass
        else:
            d[idx] = 0

        ra = random.randint(0, 100)
        if ra < 20:
            flag = 1
        new_f.write(line.strip() + ' ' + str(flag) + ' ' + str(d[idx]) + '\n')

new_f.close()

