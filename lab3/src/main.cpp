#include <iostream>
#include <string>
#include <cmath>

pthread_mutex_t mutex; // Переменная для мьютекса

void naivePatternSearch(void* inArgs) { // Функция наивного поиска подстроки в строке, принимаем аргументы
    pthread_mutex_lock(&mutex); // закрываем мьютекс
    struct threadArgs { // структура принимаемых аргументов
        std::string mainString;
        std::string pattern;
        int index;
        size_t size;
    };
    threadArgs args = *(threadArgs*)inArgs; // приводим принятые аргументы к типу структуры аргументов
    std::string mainString = args.mainString; // и распаковываем каждый аргумент. Это оригинальная строка
    std::string pattern = args.pattern; // подстрока
    int index = args.index; // индекс (по факту порядковый номер текущего потока)
    size_t size = args.size; // размер, по которому пробегает поток и ищет подстроку в строке

    size_t patLen = pattern.size(); // размер подстроки (образца)

    for (int i = index * (int)size; i < (index * (int)size + (int)size); i++) { // index * size - откуда будет начинать поиск поток
                                                                                // index * size + size - где поток закончит поиск

        int j;
        for (j = 0; j < patLen; j++) { // проверяем, совпадает ли каждый символ подстроки с символами самой строки
            if (mainString[i + j] != pattern[j]) {
                break; // если какой-то символ не совпал - прерываем цикл и переходим к следующему i
            }
        }

        if (j == patLen) { // если всё же у нас подстрока совпала с символами строки - выводим индекс, с которой есть вхождение
            std::cout << "[" << index << "] " << "Pattern found at position: " << i << std::endl;
        }

    }
    free(inArgs); // освобождаем переданные данные
    pthread_mutex_unlock(&mutex); // открываем мьютекс
}

int main(int argc, char** argv) {
    size_t inThreads; // переменная для введенного количества потоков

    if (argc != 2) { // если мы не указали количество потоков при запуске
        std::cerr << "Incorrect num of arguments, expected thread count" << std::endl; // выводим ошибки
        std::cerr << "Setting thread count to 1" << std::endl;
        inThreads = 1; // приравниваем количество потоков к 1
    } else { // иначе
        std::string temp = argv[1]; // считваем введенное количество потоков
        inThreads = std::stoi(temp); // преобразует в int
    }

    std::string mainString = "catdogcathousecatdoga"; // Строка, в которой ищем
    std::string pattern = "cat"; // подстрока, которую ищем

    size_t threadCount, size; // переменная для количества потоков и размера области, в которой каждый поток будет искать вхождение
    if (inThreads <= mainString.size() - pattern.size() + 1) { // если кол-во введенных потоков меньше, чем размер строки - размер образца
                                                               // + 1 (иначе у нас будут лишние потоки, которым нечего обрабатывать)
        size = mainString.size() / inThreads; // вычислаем размер области поиска для каждого из потоков
        threadCount = inThreads; // кол-во потоков
    } else {
        threadCount = mainString.size() - pattern.size() + 1; // иначе мы делаем количество потоков такое, чтобы каждый поток обработал
                                                              // только 1 вхождение в строку
        size = mainString.size() / threadCount; // и вычисляем размер области поиска (по факту, она всегда будет равна 1)
    }

    pthread_t th[threadCount]; // переменная для потоков. количество потоков = threadCount
    int i;
    pthread_mutex_init(&mutex, nullptr); // инициализируем мьютекс
    for (i = 0; i < threadCount; i++) { // цикл, в котором мы запускаем каждый поток
        pthread_mutex_lock(&mutex); // закрываем мьютекс
        struct threadArgs { // подготавливаем структуру для аругментов
            std::string mainString;
            std::string pattern;
            int index;
            size_t size;
        };
        auto *arguments = new threadArgs; // выделяем место в памяти для структуры. для каждого потока будет выделено отдельное место
        arguments->mainString = mainString; // заполняем структуру данными
        arguments->pattern = pattern;
        arguments->index = i;
        arguments->size = size;
        // запускаем поток, в котором выполняем функцию naivePatternSearch с параметрами arguments
        if (pthread_create(&th[i], nullptr, reinterpret_cast<void *(*)(void *)>(&naivePatternSearch), arguments) != 0) {
            std::cerr << "error while creating pthread" << std::endl; // если поток не создался - выводим ошибку
        }
        pthread_mutex_unlock(&mutex); // открываем мьютекс
    }

    for (i = 0; i < threadCount; i++) {
        if (pthread_join(th[i], nullptr) != 0) { // тут мы ждём, когда потоки завершатся. Если кто-то вернул код != 0 - выводим ошибку
            std::cerr << "error while launching pthread" << std::endl;
        }
    }
    pthread_mutex_destroy(&mutex); // удаляем мьютекс
    return 0;
}
