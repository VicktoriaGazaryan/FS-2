#include <iostream>
#include <fcntl.h>      
#include <unistd.h>    
#include <sys/stat.h> 
#include <string>
#include <cstring>      

using namespace std;

const size_t BUFFER_SIZE = 16;

void copyFile(const string& sourcePath, const string& destinationPath) {
    int sourceFd = open(sourcePath.c_str(), O_RDONLY);
    if (sourceFd == -1) {
        throw runtime_error("Ошибка: не удалось открыть исходный файл: " + sourcePath + " (" + strerror(errno) + ")");
    }

    int destFd = open(destinationPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (destFd == -1) {
        close(sourceFd);
        throw runtime_error("Ошибка: не удалось открыть файл назначения: " + destinationPath + " (" + strerror(errno) + ")");
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    size_t overallBytes = 0;

    while ((bytesRead = read(sourceFd, buffer, BUFFER_SIZE)) > 0) {
        ssize_t bytesWritten = write(destFd, buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            close(sourceFd);
            close(destFd);
            throw runtime_error("Ошибка записи в файл назначения");
        }
        overallBytes += bytesWritten;
    }

    if (bytesRead == -1) {
        close(sourceFd);
        close(destFd);
        throw runtime_error("Ошибка чтения из исходного файла");
    }

    close(sourceFd);
    close(destFd);

    cout << "Копирование завершено. Всего скопировано байтов: " << overallBytes << '\n';
}

int main(int argc, char* argv[]) {
    // Проверка входных аргументов
    if (argc < 3) {
      cout << "Использование: " << argv[0] << " <исходный файл> <файл назначения>\n";
        return 1;
    }

    try {
        copyFile(argv[1], argv[2]);
    } catch (const exception& e) {
       cout << "Произошла ошибка: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
