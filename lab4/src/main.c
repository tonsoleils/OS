#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define READ 0
#define WRITE 1

void *append(int fd, char const *data, size_t nbytes, void *map, size_t* len) {
    size_t written = write(fd, data, nbytes);
    munmap(map, *len);
    *len += written;
    return mmap(NULL, *len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

void reverse(char *string) { // Функция для инвертирования строки
    int length = strlen(string); // получаем длинну строки
    int middle = (length - 1) / 2; // получаем индекс середины строки (игнорируя последний терминальный символ)ы

    char temp;

    for (int i = 0; i < middle; i++) {
        // переставляем местами символы
        temp = string[i];
        string[i] = string[length - i - 2];  // -2 потому что игнорируем последний терминальный символ
        string[length - i - 2] = temp;
    }
}

int main() {
    pid_t child_a, child_b; // идентификаторы потоков
    int fd_a[2], fd_b[2]; // файловые дескрипторы для pipe'ов
    int file_a; // дескриптор файла A
    int file_b; // дескриптор файла B

    if (pipe(fd_a) == -1) { // Создаем пайп А, если ошибка - завершаем работу
        fprintf(stderr, "Pipe Failed");
        exit(EXIT_FAILURE);
    }
    if (pipe(fd_b) == -1) { // Создаем пайп B, если ошибка - завершаем работу
        fprintf(stderr, "Pipe Failed");
        exit(EXIT_FAILURE);
    }

    char file_name_a[256]; // Имя файла A
    char file_name_b[256]; // Имя файла B

    fgets(file_name_a, sizeof file_name_a, stdin); // Считываем первое имя файла
    fgets(file_name_b, sizeof file_name_b, stdin); // Считываем второе имя файла

    file_name_a[strcspn(file_name_a, "\n")] = 0; // удаляем символ перевода каретки
    file_name_b[strcspn(file_name_b, "\n")] = 0;

    file_a = open(file_name_a, O_RDWR | O_CREAT | O_TRUNC, 0777); // открываем файл для записи
    if (!file_a) {
        fprintf(stderr, "Could not open first file\n");
        exit(1);
    }
    file_b = open(file_name_b, O_RDWR | O_CREAT | O_TRUNC, 0777); // открываем файл для записи
    if (!file_b) {
        fprintf(stderr, "Could not open second file\n");
        exit(2);
    }

    size_t cur_len_a = 0;
    size_t cur_len_b = 0;

    lseek(file_a, 0, SEEK_SET);
    write(file_a, "", 1);

    lseek(file_b, 0, SEEK_SET);
    write(file_b, "", 1);

    child_a = fork(); // Создаём fork

    if (child_a < 0) { // Если при создании fork произошла ошибка
        fprintf(stderr, "child process A not created\n");
        exit(3);
    } else if (child_a == 0) {
        /* ------- CHILD A ------- */

        printf("Run child A\n");

        int size; // Размер строки, которую передал родительский процесс
        while(read(fd_a[READ], &size, sizeof(int)) > 0) { // пока в пайпе есть размер строки
            // т.е. когда в пайп попадает размер строки, значи в пайп попала и строка этого размера
            // и мы считываем эти параметры
            char buffer_a[255];
            read(fd_a[READ], &buffer_a, sizeof(char) * size); // считываем строку размера size
            reverse(buffer_a); // инвертируем строку
            size_t text_size = strlen(buffer_a) + 1;
            char* map1 = mmap(NULL, text_size + cur_len_a, PROT_READ | PROT_WRITE, MAP_SHARED, file_a, 0);
            if (map1 == MAP_FAILED) {
                fprintf(stderr, "Error map1");
                exit(5);
            }
            ssize_t written = write(file_a, buffer_a, text_size);
            munmap(map1, text_size + cur_len_a);
            cur_len_a += written;
            // append(file_a, buffer_a, text_size, map1, current_len_a);
        }
    } else {
        // Всё то же самое и для второго потомка родительского процесса
        child_b = fork();

        if (child_b < 0) {
            fprintf(stderr, "child process B not created\n");
        } else if (child_b == 0) {
            /* ------- CHILD B ------- */

            printf("Run child B\n");
            int size;
            while(read(fd_b[READ], &size, sizeof(int)) > 0) {
                char buffer_b[255];
                read(fd_b[READ], &buffer_b, sizeof(char) * size);
                reverse(buffer_b);
                size_t text_size = strlen(buffer_b) + 1;
                char* map2 = mmap(NULL, text_size + cur_len_b, PROT_READ | PROT_WRITE, MAP_SHARED, file_b, 0);
                if (map2 == MAP_FAILED) {
                    fprintf(stderr, "Error map2");
                    exit(5);
                }
                ssize_t written = write(file_b, buffer_b, text_size);
                munmap(map2, text_size + cur_len_b);
                cur_len_b += written;
                // append(file_b, buffer_b, text_size, map2, current_len_b);
            }
        } else {
            /* ------- PARENT ------- */

            char buffer[255];
            memset(buffer, 0, sizeof(char) * 255); // отчищаем строку
            close(fd_a[READ]); // закрываем дескрипторы пайпа для чтения, так как родитель ничего не читает
            close(fd_b[READ]);
            while(fgets(buffer, sizeof(char) * 255, stdin) != NULL) { // считываем строку
                if (strlen(buffer) > 10) { // если ее длинна больше 10 - отправляем в потомка B
                    int size = strlen(buffer) + 1; // размер строки + терминальный символ (как мне объяснил индус)
                    write(fd_b[WRITE], &size, sizeof(int)); // записываем в пайп размер строки
                    write(fd_b[WRITE], &buffer, sizeof(char) * size); // записываем в пайп саму строку
                } else { // длинна строки меньше или равна 10 - всё то же самое, что и с потомком B, только A
                    int size = strlen(buffer);
                    write(fd_a[WRITE], &size, sizeof(int));
                    write(fd_a[WRITE], &buffer, sizeof(char) * size);
                }
            }
            close(fd_a[WRITE]); // закрываем пайпы для записи
            close(fd_b[WRITE]);
            close(file_a); // закрываем файловые дескрипторы
            close(file_b);
        }
    }
}
