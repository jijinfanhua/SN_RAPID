import sys


def get_min():
    min_time = times[0][0]
    min_i = 0
    for i in range(16):
        if times[i][0] < min_time:
            min_time = times[i][0]
            min_i = i

    return min_i


def reset(s):
    _list = s[:-1].split(' ')
    _list[0] = float(_list[0])
    return _list


open_files = []
tuples = []
times = []
sizes = []
time = 0
for i in range(16):
    open_files.append(open('./trace/port_' + str(i) + '.txt', 'r'))
    line = open_files[i].readline()
    raw_tuple = reset(line)
    times.append(raw_tuple)
write_name = './trace/switch.txt'
writefile = open(write_name, 'w+')
sys.stdout = writefile

while True:
    min_seq = get_min()
    if times[min_seq][0] == 20000000000:
        break
    elif times[min_seq][0] <= time:
        print(times[min_seq])
        line = open_files[min_seq].readline()
        if line:
            times[min_seq] = reset(line)
        else:
            times[min_seq] = (20000000000, 0, 0)
    else:
        print()

    time += 1.5
