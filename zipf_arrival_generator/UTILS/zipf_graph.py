import numpy as np
import matplotlib.pyplot as plt

a = 1.29
flows = 10000

generated = np.random.zipf(a,flows)
generated.sort()

for i in range(10000):
    if generated[i] > 1:
        print(i)
        break
# print(generated)

# plt.plot(generated)
# plt.show()

#   a == 1.0001结果是430个的时候就开始大于1了,那么也就是说如果对于zipf的话,用floor是不现实的,还是太多了
#   a = 1.29 那么应该是2500个的时候
