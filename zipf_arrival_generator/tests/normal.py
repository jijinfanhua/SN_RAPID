import numpy as np
import matplotlib.pyplot as plt

def generate_data_and_plot(num_flow, time):
    mean = time / 2
    std_dev = 100

    # generate data
    data = np.random.normal(mean, std_dev, num_flow)

    # Clip the data to be within [0, time]
    data = np.clip(data, 0, time)

    # plot histogram
    plt.hist(data, bins=100, density=True, alpha=0.6, color='g')
    plt.title('Histogram of Generated Data')
    plt.xlabel('Time')
    plt.ylabel('Frequency')

    plt.show()

if __name__ == "__main__":
    num_flow = 1000
    time = 100000
    generate_data_and_plot(num_flow, time)
