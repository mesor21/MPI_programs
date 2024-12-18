import random
import sys

def generate_matrix_file(filename, rows, cols, min_value=1, max_value=15):
    with open(filename, 'w') as f:
        f.write(f"{rows} {cols}\n")
        for _ in range(rows):
            row = [random.randint(min_value, max_value) for _ in range(cols)]
            f.write(" ".join(map(str, row)) + "\n")

if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage: python script.py <file_name> <N> <M> <max value>")
        sys.exit(1)
    
    file_name = sys.argv[1]
    try:
        N = int(sys.argv[2])
    except ValueError:
        print("Error: N must be an integer.")
        sys.exit(1)

    if N <= 0:
        print("Error: N must be greater than 0.")
        sys.exit(1)
    
    try:
        M = int(sys.argv[3])
    except ValueError:
        print("Error: M must be an integer.")
        sys.exit(1)

    if M <= 0:
        print("Error: M must be greater than 0.")
        sys.exit(1)
    
    try:
        max = int(sys.argv[4])
    except ValueError:
        print("Error: max must be an integer.")
        sys.exit(1)

    if max <= 0:
        print("Error: max must be greater than 0.")
        sys.exit(1)
    
    generate_matrix_file(file_name, N, M, max_value=max)
    print(f"Матрица размером {N}x{M} успешно сохранена в файл '{file_name}'.")