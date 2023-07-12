# 打开文件并读取每一行
with open("packet_sequence_info.txt", "r") as file:
    lines = file.readlines()

# 对每一行进行处理，将其转换为（x，y）的元组，并将y转换为整数
lines = [(line.split()[0], int(line.split()[1])) for line in lines]

# 按照y的值对元组进行排序
lines = sorted(lines, key=lambda x: x[1])

# 将排序后的元组转换回字符串的形式
lines = [f"{x} {y}\n" for x, y in lines]

# 将排序后的行写入新的文件中
with open("sorted_packet_sequence_info.txt", "w") as file:
    file.writelines(lines)
