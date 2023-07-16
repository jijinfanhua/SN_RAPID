import subprocess
import sys
import os
import shutil
import time

def main(arg1, arg2, arg3, arg4,arg5):

    subprocess.check_call(['python', './zipf_input.py', str(int(arg1)), str(arg2), str(arg3), str(arg4)])

    input_file_name = "./input_{}_{}_{}_{}.txt".format(str(arg1), str(arg2), str(arg3), str(arg4))
    output_file_name = "./output_{}_{}_{}_{}.txt".format(str(arg1), str(arg2), str(arg3), str(arg4))

    while not os.path.isfile(input_file_name):
        time.sleep(1)
        print("sleep 1")

    subprocess.check_call(['python', './arrival_average.py', input_file_name, output_file_name,str(arg5)])

    while not os.path.isfile(output_file_name):
        time.sleep(1)
        print("sleep 2")

    # Move the 'syn' file to the 'non_blocking/trace_syn' folder
    # shutil.move(output_file_name, os.path.join(output_folder_name, os.path.basename(output_file_name)))
    #
    # # Remove the original 'trace' folder and all its contents
    # shutil.rmtree(trace_folder_name)
    #
    # os.remove(input_file_name)

if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])

# trace_file_name = "./trace"
# shutil.rmtree(trace_file_name)