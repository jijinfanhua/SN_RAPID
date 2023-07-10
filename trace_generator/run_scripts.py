import subprocess
import sys
import os
import shutil
import time

def main(arg1, arg2, arg3):
    # 运行第一个脚本
    subprocess.check_call(['python', './generate_zipf_input.py', str(arg1), str(arg2), str(arg3)])

    # 等待文件生成
    while not os.path.isfile('./input_zipf.txt'):
        time.sleep(1)

    # 运行第二个脚本
    subprocess.check_call(['python', './packet_generator.py'])

    # 等待文件生成
    while not os.path.isfile('./output3.txt'):
        time.sleep(1)

    # 运行第三个脚本，并获取它的输出
    result = subprocess.check_output(['python', './blocking_test.py'])

    # 等待文件生成
    while not os.path.isfile('./record1_1.txt') or not os.path.isfile('./whether_packet_drop.txt'):
        time.sleep(1)

    lines = result.decode('utf-8').split('\n')
    last_line = lines[-1] if lines[-1] else lines[-2]  # 获取最后一行

    # 假设最后一行是可以转化为浮点数的字符串
    last_number = float(last_line.strip())

    # 计算比值
    ratio = last_number / float(arg1)
    print(ratio)
    if ratio <= 0.30 and ratio >= 0.10:
        new_folder = ("packets{} clk{} flows{}".format(arg1,arg2,arg3))
        os.makedirs(new_folder, exist_ok=True)
        files_to_copy = [
            './input_zipf.txt',
            './output3.txt',
            './record1_1.txt',
            './whether_packet_drop.txt'
        ]
        # 将每个文件复制到新的文件夹
        for file_path in files_to_copy:
            shutil.copy(file_path, new_folder)

if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("需要提供三个参数.")
        sys.exit(1)

    main(sys.argv[1], sys.argv[2], sys.argv[3])
