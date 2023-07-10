import subprocess
import sys

def main(arg1, arg2, arg3):
    # 运行第一个脚本
    subprocess.check_call(['python', './trace_generator/generate_zipf_input.py', str(arg1), str(arg2), str(arg3)])

    # 运行第二个脚本
    subprocess.check_call(['python', './trace_generator/packet_generator.py'])

    # 运行第三个脚本
    subprocess.check_call(['python', './trace_generator/blocking_test.py'])



main(sys.argv[1], sys.argv[2], sys.argv[3])
