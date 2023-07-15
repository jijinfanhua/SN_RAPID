import matplotlib.pyplot as plt

def plot_packets(output_file):
    arrival_time = []
    flow_ids = []
    with open(output_file, 'r') as file:
        for line in file:
            time, flow_id = line.split()
            arrival_time.append(int(time))
            flow_ids.append(int(flow_id))

    plt.scatter(arrival_time, flow_ids, alpha=0.5)
    plt.title('Packet Arrivals Over Time')
    plt.xlabel('Arrival Time')
    plt.ylabel('Flow ID')
    plt.show()

if __name__ == "__main__":
    output_file = "./output.txt"
    plot_packets(output_file)
