import numpy as np
from scipy.special import factorial

# m = 50
# lamda = 0.5
# sita = 1/3
#
#

# # Eh_square = 7500

#
# # computing EN_q using the formula
# # EN_q = \frac{\lambda E(h^2)}{2(m-\lambda Eh)Eh}(1+\sum_{k=0}^{m-1}\frac{(m-1)!(m-\lambda Eh)}{k!(\lambda Eh)^{m-k}})^{-1}
# # First we calculate the sum
# sum_term = sum(
#     [factorial(m - 1) * (m - lamda * Eh) / (factorial(k) * (lamda * Eh) ** (m - k)) for k in range(m)])
#
# # Then we calculate the entire formula
# EN_q = lamda * Eh_square / (2 * (m - lamda * Eh) * Eh) * (1 + sum_term) ** -1


for m in range(88, 90, 2):
    for lamda in range(0, 100, 2):
        lamda = lamda / 100
        for sita in range(2, int(100 - lamda * 100), 2):
            sita = sita / 100
            Eh = m / (1 - sita)
            Eh_square = m * m * (1 + sita) / (1 - sita) / (1 - sita)
            sum_term = sum(
                [factorial(m - 1) * (m - lamda * Eh) / (factorial(k) * (lamda * Eh) ** (m - k)) for k in range(m)])
            try:
                EN_q = lamda * Eh_square / (2 * (m - lamda * Eh) * Eh) * (1 + sum_term) ** -1
                EW_q = EN_q / lamda
            except:
                print('run')

            if 2 * (m - lamda * Eh) * Eh < 0 or EN_q < 0:
                print('wrong')
            print('Eh^2: ', int(Eh_square), 'Eh: ', int(Eh), 'm: ', m, 'lambda: ', "{:.2f}".format(lamda), 'sita: ', "{:.2f}".format(sita), 'EN_q: ', EN_q, 'EW_q: ', EW_q)
