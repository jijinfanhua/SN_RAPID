import numpy as np
from scipy.stats import zipf
import matplotlib.pyplot as plt

def generate_zipf(n, lamda, a=1.01, filename='zipf_numbers.txt'):
    """Generate a list of numbers satisfying Zipf's distribution and write them to a file.

    Arguments:
    n -- the number of numbers to generate
    lamda -- the sum of the numbers
    a -- the parameter of the Zipf distribution
    filename -- the name of the file to write the numbers to

    Returns:
    A list of n integers that sum up to lamda and approximately satisfy Zipf's distribution.
    """

    # Generate n numbers from the Zipf distribution.
    x = zipf.rvs(a, size=n)
    x = x.astype(np.float64)

    # Normalize the numbers so that their sum is lamda.
    x *= lamda / np.sum(x)

    # Round the numbers to get integers.
    x = np.round(x).astype(int)

    # If the sum of the numbers is not exactly lamda due to rounding, adjust it.
    diff = lamda - np.sum(x)
    if diff > 0:
        indices = np.random.choice(n, diff, replace=False)
        for i in indices:
            x[i] += 1
    elif diff < 0:
        indices = np.random.choice(n, -diff, replace=False)
        for i in indices:
            x[i] -= 1

    # Write the numbers to a file.
    with open(filename, 'w') as f:
        for num in x:
            f.write(str(num) + '\n')

    # Plot a histogram of the numbers.
    plt.hist(x, bins='auto')
    plt.title("Histogram of Generated Numbers")
    plt.xlabel("Number")
    plt.ylabel("Frequency")
    plt.show()

    return x.tolist()

# 用法示例:
n = 10000
lamda = 100
print(generate_zipf(n, lamda))
