
d = {}

with open("caida_build_trace_22_NIC_400W_from_2000W_simple.txt", 'r') as f:
    while True:
        line = f.readline()
        if not line:
            break
        pkt_id = int(line.split()[0])
        if pkt_id in d:
            d[pkt_id] += 1
        else:
            d[pkt_id] = 1

print(len(d))