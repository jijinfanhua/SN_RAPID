import numpy as np

lambda_1 = 3  # 第一个参数 lambda
samples_1 = np.random.poisson(lambda_1, 1000)  # 生成泊松分布

lambda_2 = 6  # 修改第一个参数 lambda
samples_2 = np.random.poisson(lambda_2, 1000)  # 生成泊松分布

# 输出结果
print("泊松分布1的均值和方差:", np.mean(samples_1), np.var(samples_1))
print("泊松分布2的均值和方差:", np.mean(samples_2), np.var(samples_2))
