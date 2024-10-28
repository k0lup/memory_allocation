#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>

#define POOL_SIZE 1024 // ����������� ������ ������� ���� ������
#define BLOCK_SIZE 64  // ����������� ������� ������ ����� ������
#define NUM_BLOCKS (POOL_SIZE / BLOCK_SIZE) // ���������� ������ ���������� ������ ������

unsigned char bitmap[NUM_BLOCKS / 8]; // ������� ����� ��� ������������ ��������� ������� �����
void* mem = NULL;

// ������� ��� ��������� ���� � ������� ����� (�������� ���� ��� �������)
void setBit(int n) {
    bitmap[n / 8] |= 1 << (n % 8); // ������������� n-�� ��� � 1
}

// ������� ��� ������ ���� � ������� ����� (�������� ���� ��� ���������)
void clearBit(int n) {
    bitmap[n / 8] &= ~(1 << (n % 8)); // ���������� n-�� ��� � 0
}

// ������� ��� ��������, ���������� �� ��� � ������� ����� (���������, ����� �� ����)
int isBitSet(int n) {
    return bitmap[n / 8] & (1 << (n % 8)); // ���������� 1, ���� n-�� ��� ���������� (���� �����), ����� 0
}

// ������� ������������� ���� ������
void initMemoryPool() {
    mem = malloc(POOL_SIZE);
    for (int i = 0; i < NUM_BLOCKS / 8; i++) {
        bitmap[i] = 0; // �������������� ������ ������� ������� ����� ����� (��� ����� ��������)
    }
}

// ������� ��������� ������
int lastAllocated = -1; // ���������� ��� ������������ ������� ���������� ����������� �����

void* allocMemory(size_t size) {
    /*if (size > BLOCK_SIZE) {
        printf("����������� ������ ������ ������� �����\n");
        return NULL; // ���������� NULL, ���� ����������� ������ ������ ������� ������ �����
    }*/

    int len_block = ceil(double(size) / BLOCK_SIZE);

    int start = (lastAllocated + 1) % NUM_BLOCKS; // �������� ����� �� ���������� ����� ����� ���������� �����������
    for (int i = 0; i < NUM_BLOCKS; i++) {
        int idx = (start + i) % NUM_BLOCKS; // ����������� ����� ���������� ����������� �����
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

    return NULL; // ���������� NULL, ���� �� ������� ��������� ������
}

// ������� ������������ ������
void freeMemory(void* ptr, size_t size) {
    int blockIndex = ((char*)ptr - (char*)mem) / BLOCK_SIZE; // ��������� ������ ����� �� ���������
    int len_block = ceil(double(size) / BLOCK_SIZE);
    for (int i = 0; i < len_block; i++) {
        clearBit((blockIndex + i) % NUM_BLOCKS); // �������� ���� ��� ���������
    }
}

// ������� ��� ������ ��������� ������� �����
void printBitmap() {
    printf("��������� ������� �����:\n");
    for (int i = 0; i < NUM_BLOCKS / 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d", (bitmap[i] >> j) & 1); // �������� ��������� ������� ���� � �����
        }
        printf(" "); // ��������� ������ ��� ���������� ������
    }
    printf("\n");
}

// ������� ��� ������ ��������� ������
void printMemoryState() {
    printf("��������� ������:\n");
    for (int i = 0; i < NUM_BLOCKS; i++) {
        printf("���� %d: %s\n", i, isBitSet(i) ? "�����" : "��������"); // ������� ��������� ������� �����
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    initMemoryPool(); // ������������� ���� ������

    void* block1 = allocMemory(100); // ��������� ����� ������
    void* block2 = allocMemory(50); // ��������� ��� ������ ����� ������
    printBitmap(); // ����� ��������� ������� �����
    printMemoryState(); // ����� ��������� ������

    freeMemory(block1, 100); // ������������ ������� ����� ������
    freeMemory(block2, 50); // ������������ ������� ����� ������
    printBitmap(); // ����� ��������� ������� ����� ����� ������������ ������
    printMemoryState(); // ����� ��������� ������ ����� ������������ ������

    return 0;
}
