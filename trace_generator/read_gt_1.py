import numpy as np


def process_file():
    # 首先，我们打开文件并读取它的内容
    with open('input_zipf.txt', 'r') as file:
        lines = file.readlines()

    # 然后，我们将需要的数字存储在一个新的列表中
    result = []
    for line in lines[3:]:  # 从第四行开始处理
        numbers = line.split()  # 将每一行的内容根据空格分开
        if int(numbers[1]) > 1:  # 如果第二个数字大于1
            result.append(numbers[0])  # 将第一个数字添加到结果列表中

    # 最后，我们将结果写入到新的文件中
    with open('gt_1.txt', 'w') as file:
        for number in result:
            file.write(number + '\n')


process_file()

def calculate():
    # 从之前生成的文件中读取数据
    with open('gt_1.txt', 'r') as file:
        output_numbers = [line.strip() for line in file]

    # 创建一个新的文件用于存储结果
    result_file = open('result.txt', 'w')

    # 读取并处理 output3.txt 文件
    with open('output3.txt', 'r') as file:
        lines = file.readlines()

    for output_number in output_numbers:
        diff_sum = 0
        prev_number = None
        count = 0
        for line in lines:
            numbers = line.split()
            if numbers[1] == output_number:  # 如果第二个数字等于当前的 output_number
                current_number = int(numbers[0])
                if prev_number is not None:  # 如果不是第一次遇到符合条件的行
                    diff_sum += (current_number - prev_number)
                prev_number = current_number
                count += 1

        if count > 0:  # 避免除以零
            result = diff_sum / count
            # 将 output_number 和计算结果写入文件
            result_file.write(output_number + ' ' + str(np.ceil(result)) + '\n')

    # 关闭文件
    result_file.close()

calculate()
