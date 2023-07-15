
d = {}

with open("./output_10000_200_100000_1.01.txt", 'r') as f:
    while True:
        line = f.readline()
        if not line:
            break
        else:
            arr = line.split()
            clk, idx = int(arr[0]), int(arr[1])

            if idx in d:
                if clk - d[idx][1] > 100:
                    d[idx][1] = clk
                else:
                    d[idx][0].append(clk - d[idx][1])
                    d[idx][1] = clk
                    d[idx][2] += 1
            else:
                d[idx] = [[], clk, 0]

period_num = 0
period_value = 0
flow_num = 0
for key, value in d.items():
    if len(value[0]) > 0:
        flow_num += 1
        period_num += value[2]
        period_value += sum(value[0])

print(flow_num, period_num)
print(period_value / period_num)
