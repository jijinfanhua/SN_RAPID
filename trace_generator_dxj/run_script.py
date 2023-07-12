# # 这个脚本中,实现的和trace_generator中的效果不一样
# # 在这里,只需要实现生成output
# # 但是同时,要求完成更好的命名规范
# # 为了完成这个目的,也需要对generate和p_g做适当的修改
#
# # todo
# # 首先,生成符合要求的数据,送给g_z_i
# # g_z_i生成input,命名好,生成一个文件夹,送到这个文件夹里面去
# # p_g从文件夹里面读取input,生成符合命名规范的output
# import subprocess
# import sys
# import os
# import shutil
# import time
#
# def main(arg1,arg2,arg3,arg4):
#     subprocess.check_call(['python','./generate_zipf_input.py',str(arg1),str(arg2),str(arg3),str(arg4)])
#     input_file_name = ("./input_{}.txt".format(str(arg4)))
#     output_file_name = ("./output_{}.txt".format(str(arg4)))
#     while not os.path.isfile(input_file_name):
#         time.sleep(1)
#     subprocess.check_call(['python','./packet_generator.py',input_file_name,output_file_name])
#     while not os.path.isfile(output_file_name):
#        time.sleep(1)
#
#
#
#
# if __name__ == "__main__":
#     main(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4])
import subprocess
import sys
import os
import shutil
import time


def main(arg1, arg2, arg3, arg4):
    # Create a new folder for the current execution inside the "trace" folder
    folder_name = "./trace/input_{}_{}_{}_{}".format(str(arg1), str(arg2), str(arg3), str(arg4))
    os.makedirs(folder_name, exist_ok=True)

    subprocess.check_call(['python', './generate_zipf_input.py', str(arg1), str(arg2), str(arg3), str(arg4)])

    input_file_name = ("./input_{}.txt".format(str(arg4)))
    output_file_name = ("./output_{}.txt".format(str(arg4)))

    while not os.path.isfile(input_file_name):
        time.sleep(1)

    subprocess.check_call(['python', './packet_generator.py', input_file_name, output_file_name])

    while not os.path.isfile(output_file_name):
        time.sleep(1)

    # Move the files to the newly created folder and delete them from the main folder
    shutil.move(input_file_name, os.path.join(folder_name, os.path.basename(input_file_name)))
    shutil.move(output_file_name, os.path.join(folder_name, os.path.basename(output_file_name)))


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
