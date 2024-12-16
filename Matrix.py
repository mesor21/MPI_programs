import random

def generate_matrix_file(filename, rows, cols, min_value=1, max_value=15):
    with open(filename, 'w') as f:
        f.write(f"{rows} {cols}\n")
        for _ in range(rows):
            row = [random.randint(min_value, max_value) for _ in range(cols)]
            f.write(" ".join(map(str, row)) + "\n")

# Задайте размеры матрицы
N = 48 # Количество строк
M = 1000  # Количество столбцов
filename = "matrix.txt"

generate_matrix_file(filename, N, M)

print(f"Матрица размером {N}x{M} успешно сохранена в файл '{filename}'.")
