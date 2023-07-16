
f = open("resubmit_writeback.txt", 'w')


for zipfa in range(101, 121):
    for wb_idx in range(0, 4):
        with open(f"result_zipfa_{zipfa}_wb_{wb_idx}.txt", 'r') as g:
            while True:
                line = g.readline()
                if not line:
                    break

                arr = line.split()
                
                file_name = arr[0].split('_')
                pkt_num = file_name[1]
                flow_num = file_name[2]
                zipf_a = file_name[-1]

                f.write(f"{arr[0]}, {pkt_num}, {flow_num}, {zipf_a}, {arr[1]}, {arr[2]}, {arr[3]}, {arr[4]}, {arr[5]}, {arr[6]}\n")


f.close()