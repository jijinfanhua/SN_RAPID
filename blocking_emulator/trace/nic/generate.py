
with open("one_flow.txt", 'w') as f:
    for i in range(0, 100000):
        f.write("{} {} {}\n".format(1, 0, 0))