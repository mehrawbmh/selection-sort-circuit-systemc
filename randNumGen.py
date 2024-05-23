from random import randint

LENGTH = 256
file_name = "mem.txt"
BITS = 16


def generate_rand_num(BITS: int) -> str:
    bits_list = [randint(0, 1) for i in range(BITS)]
    return "".join(str(num) for num in bits_list) + '\n'


with open(file_name, 'w') as f:
    for i in range(LENGTH):
        f.write(generate_rand_num(BITS))


print("DONE")


