import numpy as np
from scipy.special import factorial

m = 50
lamda = 0.5
Eh = 75
Eh_square = 7500

# computing EN_q using the formula
# EN_q = \frac{\lambda E(h^2)}{2(m-\lambda Eh)Eh}(1+\sum_{k=0}^{m-1}\frac{(m-1)!(m-\lambda Eh)}{k!(\lambda Eh)^{m-k}})^{-1}
# First we calculate the sum
sum_term = sum(
    [factorial(m - 1) * (m - lamda * Eh) / (factorial(k) * (lamda * Eh) ** (m - k)) for k in range(m)])

# Then we calculate the entire formula
EN_q = lamda * Eh_square / (2 * (m - lamda * Eh) * Eh) * (1 + sum_term) ** -1

print('EN_q:', EN_q)

