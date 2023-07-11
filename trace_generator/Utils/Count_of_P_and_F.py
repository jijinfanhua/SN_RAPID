def process_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
    # 计算第一列的个数
    count = len(lines)
    # 对第二列的数值求和
    sum_values = sum(int(line.split()[1]) for line in lines)
    return count, sum_values

filename = 'packet_sequence_info.txt'  # 替换为你的文件名
count, sum_values = process_file(filename)

print("第一个数目的个数：", count)
print("第二个数目的求和：", sum_values)

with open("countof_P_S_I.txt",'w') as f:
    f.write('flows:{} \npackets:{}\n'.format(count,sum_values))