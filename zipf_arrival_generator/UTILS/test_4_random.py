# 随机性具有密集分布的特点
# 这是一个测试:生成一系列大于总时间,假定是100 k,的数字.用他们模100 k,观察图像

import random
import matplotlib.pyplot as plt

# 生成十万个数字
# randint 用于生成从a到b的伪随机数
numbers = [random.randint(100000, 1000000) for _ in range(100000)]

# 对每个数字进行取模运算
modulos = [num % 100000 for num in numbers]

# 绘制分布图
plt.plot(modulos)
plt.xlabel('Modulo')
plt.ylabel('Count')
plt.title('Distribution of Modulos')
plt.show()
# 得出结果,python伪随机数不能够产生真正随机的数
