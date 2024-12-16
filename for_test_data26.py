import random
import sys

def generate_random_words(file_name, word_count):
    # Списки случайных слов на английском и русском
    english_words = ["apple", "banana", "cat", "dog", "elephant", "flower", "grape", "house", "ice", "jungle"]
    russian_words = ["яблоко", "банан", "кот", "собака", "слон", "цветок", "виноград", "дом", "лед", "джунгли"]

    # Генерация случайных слов
    random_words = []
    for _ in range(word_count):
        if random.choice([True, False]):  # Выбор языка
            random_words.append(random.choice(english_words))
        else:
            random_words.append(random.choice(russian_words))

    # Запись в файл
    with open(file_name, "w", encoding="utf-8") as file:
        file.write(" ".join(random_words))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <file_name> <word_count>")
        sys.exit(1)

    file_name = sys.argv[1]
    try:
        word_count = int(sys.argv[2])
    except ValueError:
        print("Error: word_count must be an integer.")
        sys.exit(1)

    if word_count <= 0:
        print("Error: word_count must be greater than 0.")
        sys.exit(1)

    generate_random_words(file_name, word_count)
    print(f"File '{file_name}' created with {word_count} random words.")
