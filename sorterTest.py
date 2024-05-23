def read_binary_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
    return [line.strip() for line in lines]


def write_binary_file(filename, lines):
    with open(filename, 'w') as file:
        for line in lines:
            file.write(line + '\n')


def sort_binary_words(words):
    return sorted(words)


def main():
    input_filename = 'mem.txt'
    output_filename = 'sorted_mem_python.txt'

    binary_words = read_binary_file(input_filename)
    sorted_binary_words = sort_binary_words(binary_words)
    write_binary_file(output_filename, sorted_binary_words)


if __name__ == '__main__':
    main()
