def process_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
    # 计算第一列的个数
    count = len(lines)
    # 对第二列的数值求和
    sum_values = sum(int(line.split()[1]) for line in lines)
    return sum_values

filename = './packet_sequence_info.txt'  # 替换为你的文件名
sum_values = process_file(filename)

print(sum_values)