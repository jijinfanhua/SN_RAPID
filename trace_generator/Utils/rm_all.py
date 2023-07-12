import os

file_to_delete = [
    './input_zipf.txt',
   # './output3.txt',
    #   './record1_1.txt',
    './whether_packet_drop.txt'
]

pathname = "."

for i in range(0,2):
    os.remove(os.path.join(pathname,file_to_delete[i]))