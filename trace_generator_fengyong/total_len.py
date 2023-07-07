import sys

sum = 0

write_name = 'sum_result'
writefile = open(write_name, 'w+')

sys.stdout = writefile


for i in range(16):
  with open('../port_' + str(i) + '.txt', 'r') as f:
    lines = f.readlines()
    for line in lines:
      time, flow5tuple, pkt_len = line.split(' ')
      sum = sum + int(pkt_len)
    print('{length} {sum}'.format(length = len(lines), sum = sum), len(lines), sum)

print(sum)
     