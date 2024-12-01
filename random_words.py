import random
import string

# Список русских слов (пример, можно расширить)
russian_words = [
    "собака", "дерево", "машина", "дом", "книга", "стол", "окно", "река", "гора", "дорога", 
    "солнце", "птица", "человек", "молоко", "молния", "кошка", "песок", "вода", "небо", "лес"
]

# Список английских слов (пример, можно расширить)
english_words = [
    "dog", "tree", "car", "house", "book", "table", "window", "river", "mountain", "road",
    "sun", "bird", "human", "milk", "lightning", "cat", "sand", "water", "sky", "forest"
]

# Функция для генерации случайного слова на английском или русском
def generate_word(language="english"):
    if language == "russian":
        return random.choice(russian_words)
    else:
        return random.choice(english_words)

# Основная функция для генерации файла
def generate_file(filename, num_lines):
    with open(filename, "w", encoding="utf-8") as f:
        for _ in range(num_lines):
            # Генерируем случайное количество слов в строке (между 10 и 50)
            num_words_in_line = random.randint(10, 50)
            line = []
            for _ in range(num_words_in_line):
                # Случайно выбираем язык: русский или английский
                language = random.choice(["english", "russian"])
                line.append(generate_word(language))
            f.write(" ".join(line) + "\n")

# Ввод параметров от пользователя
filename = input("Введите имя файла для записи: ")
num_lines = int(input("Введите количество строк в файле: "))

# Генерация файла
generate_file(filename, num_lines)
print(f"Файл '{filename}' успешно сгенерирован.")
