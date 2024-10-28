#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>

#define POOL_SIZE 1024 // Определение общего размера пула памяти
#define BLOCK_SIZE 64  // Определение размера одного блока памяти
#define NUM_BLOCKS (POOL_SIZE / BLOCK_SIZE) // Вычисление общего количества блоков памяти

unsigned char bitmap[NUM_BLOCKS / 8]; // Битовая карта для отслеживания состояния каждого блока
void* mem = NULL;

// Функция для установки бита в битовой карте (отмечает блок как занятый)
void setBit(int n) {
    bitmap[n / 8] |= 1 << (n % 8); // Устанавливает n-ый бит в 1
}

// Функция для сброса бита в битовой карте (отмечает блок как свободный)
void clearBit(int n) {
    bitmap[n / 8] &= ~(1 << (n % 8)); // Сбрасывает n-ый бит в 0
}

// Функция для проверки, установлен ли бит в битовой карте (проверяет, занят ли блок)
int isBitSet(int n) {
    return bitmap[n / 8] & (1 << (n % 8)); // Возвращает 1, если n-ый бит установлен (блок занят), иначе 0
}

// Функция инициализации пула памяти
void initMemoryPool() {
    mem = malloc(POOL_SIZE);
    for (int i = 0; i < NUM_BLOCKS / 8; i++) {
        bitmap[i] = 0; // Инициализирует каждый элемент битовой карты нулем (все блоки свободны)
    }
}

// Функция выделения памяти
int lastAllocated = -1; // Переменная для отслеживания индекса последнего выделенного блока

void* allocMemory(size_t size) {
    /*if (size > BLOCK_SIZE) {
        printf("Запрошенный размер больше размера блока\n");
        return NULL; // Возвращает NULL, если запрошенный размер больше размера одного блока
    }*/

    int len_block = ceil(double(size) / BLOCK_SIZE);

    int start = (lastAllocated + 1) % NUM_BLOCKS; // Начинает поиск со следующего блока после последнего выделенного
    for (int i = 0; i < NUM_BLOCKS; i++) {
        int idx = (start + i) % NUM_BLOCKS; // Циклический поиск следующего подходящего блока
        bool flag = false;
        for (int k = 0; k < len_block; k++) {
            if (isBitSet(idx + k)) break;
            if (k == len_block - 1) flag = true;
        }
        if (flag) {
            for (int k = 0; k < len_block; k++) {
                setBit(idx + k);
            }
            lastAllocated = idx + len_block - 1;
            return (void*)((char*)mem + idx * BLOCK_SIZE);
        }
    }

    return NULL; // Возвращает NULL, если не найдено свободных блоков
}

// Функция освобождения памяти
void freeMemory(void* ptr, size_t size) {
    int blockIndex = ((char*)ptr - (char*)mem) / BLOCK_SIZE; // Вычисляет индекс блока из указателя
    int len_block = ceil(double(size) / BLOCK_SIZE);
    for (int i = 0; i < len_block; i++) {
        clearBit((blockIndex + i) % NUM_BLOCKS); // Отмечает блок как свободный
    }
}

// Функция для вывода состояния битовой карты
void printBitmap() {
    printf("Состояние битовой карты:\n");
    for (int i = 0; i < NUM_BLOCKS / 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d", (bitmap[i] >> j) & 1); // Печатает состояние каждого бита в байте
        }
        printf(" "); // Добавляет пробел для разделения байтов
    }
    printf("\n");
}

// Функция для вывода состояния памяти
void printMemoryState() {
    printf("Состояние памяти:\n");
    for (int i = 0; i < NUM_BLOCKS; i++) {
        printf("Блок %d: %s\n", i, isBitSet(i) ? "занят" : "свободен"); // Выводит состояние каждого блока
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    initMemoryPool(); // Инициализация пула памяти

    void* block1 = allocMemory(100); // Выделение блока памяти
    void* block2 = allocMemory(50); // Выделение еще одного блока памяти
    printBitmap(); // Вывод состояния битовой карты
    printMemoryState(); // Вывод состояния памяти

    freeMemory(block1, 100); // Освобождение первого блока памяти
    freeMemory(block2, 50); // Освобождение второго блока памяти
    printBitmap(); // Вывод состояния битовой карты после освобождения блоков
    printMemoryState(); // Вывод состояния памяти после освобождения блоков

    return 0;
}
