def check_file_for_duplicates(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    # Dictionary to store the unique numbers and their line numbers
    unique_numbers = {}

    for i, line in enumerate(lines, 1):  # Starting the line numbering at 1
        # Split the line into numbers and take the first one
        first_number = int(line.split()[0])
        if first_number in unique_numbers:
            return f"Number {first_number} first appeared at line {unique_numbers[first_number]} and repeated at line {i}"
        else:
            unique_numbers[first_number] = i

    # If we have gone through all the lines without finding duplicates
    return "No duplicates found"

print(check_file_for_duplicates("output_10000_200_100000_1.01.txt"))