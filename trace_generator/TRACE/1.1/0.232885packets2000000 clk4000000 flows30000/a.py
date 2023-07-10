
d = []

with open("./input_zipf_sorted.txt", 'r') as f:
    while True:
        line = f.readline()
        if not line:
            break
        arr = line.split()
        d.append(int(arr[1]))

print(len(d), sum(d))