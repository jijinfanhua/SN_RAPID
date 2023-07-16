import numpy as np


def process_file():
    # 在这里,需要修改为 input*.txt ,如果不是zipf分布的话要改一改
    with open('input_zipf.txt', 'r') as file:
        lines = file.readlines()

    result = []
    for line in lines[3:]:
        numbers = line.split()
        if int(numbers[1]) > 1:
            result.append(numbers[0])

    # 写入到新的文件GreaterThan_1.txt中
    with open('gt_1.txt', 'w') as file:
        for number in result:
            file.write(number + '\n')

process_file()

def calculate():
    with open('gt_1.txt', 'r') as file:
        output_numbers = [line.strip() for line in file]

# 为了存储内容,需要一个新的程序
    result_file = open('result.txt', 'w')

    # 这里不需要改,output3.txt在其他地方被固化,如果要改的话要一次性修改多个地方
    with open('output3_2292.txt', 'r') as file:
        lines = file.readlines()

    for output_number in output_numbers:
        diff_sum = 0
        prev_number = None
        count = 0
        for line in lines:
            numbers = line.split()
            if numbers[1] == output_number:  # 如果第二个数字等于当前的output_number
                current_number = int(numbers[0])
                if prev_number is not None:  # 如果不是第一次遇到符合条件的行
                    diff_sum += (current_number - prev_number)
                prev_number = current_number
                count += 1

        if count > 0: # div 0 err
            result = diff_sum / count
            result_file.write(output_number + ' ' + str(np.ceil(result)) + '\n')

    result_file.close()

calculate()
