from collections import Counter
from sys import argv

filename = argv[1]
if (filename is None):
    print("No args, enter file name")
    exit(-1)

# Чтение данных из Sem1Raw.txt
with open(filename, 'r') as f:
    content = f.read().strip()

# Парсинг чисел
numbers = []
for num_str in content.replace('\n', '').split(','):
    num_str = num_str.strip()
    if num_str:
        numbers.append(float(num_str))

# Группировка и подсчет
counter = Counter(numbers)
sorted_items = sorted(counter.items())

# Запись в Sem1List.txt
filename = filename.replace("Raw", "List")
with open(filename, 'w') as f:
    for value, count in sorted_items:
        formatted_value = f"{value:.2f}"
        f.write(f"{formatted_value:<10} | {count:>7}\n")

print(f"Файл {filename} создан успешно!")