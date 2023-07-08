input_file1 = "countof_P_S_I.txt"  # 第一个输入文件的路径和文件名
input_file2 = "packet_sequence_info.txt"  # 第二个输入文件的路径和文件名
output_file = "output.txt"  # 输出文件的路径和文件名

with open(input_file1, "r") as file:
    lines = file.readlines()

flows_line = lines[0].strip()  # 提取第一行，并去除首尾的空格和换行符
packets_line = lines[1].strip()  # 提取第二行，并去除首尾的空格和换行符

flows = flows_line.split(":")[1].strip()  # 提取flows后的数字
packets = packets_line.split(":")[1].strip()  # 提取packets后的数字

with open(output_file, "w") as file:
    file.write(packets + "\n\n" + flows + "\n")

with open(input_file2, "r") as file:
    file_contents = file.read()

with open(output_file, "a") as file:
    file.write(file_contents)
