#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

// Посимвольный вывод
void typeWrite(const string& text, int speed = 15) {
    for (size_t i = 0; i < text.length(); ++i) {
        cout << text[i];
        Sleep(speed);
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A); // Зеленый цвет

    cout << "==================================================" << endl;
    typeWrite("  [!] CRYPTO-AUTO v1.0 KEY-FILE ENGINE... [!]\n");
    cout << "==================================================" << endl << endl;

    string filePath;
    typeWrite("Enter target file path (e.g. C:\\test.txt): ");
    getline(cin, filePath);

    // Открываем целевой файл
    ifstream inFile(filePath.c_str(), ios::binary);
    if (!inFile) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C); // Красный
        cout << "\n[ERROR] Could not open target file!" << endl;
        system("pause");
        return 1;
    }

    // Измеряем размер файла
    inFile.seekg(0, ios::end);
    long long fileSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    if (fileSize == 0) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C);
        cout << "\n[ERROR] File is empty!" << endl;
        inFile.close();
        system("pause");
        return 1;
    }

    // Меню выбора операции
    typeWrite("\nChoose operation:\n [1] Encrypt & Auto-Generate Keyfile\n [2] Decrypt using Keyfile\nYour choice: ");
    string choice;
    getline(cin, choice);

    string key = "";
    string keyFilePath = filePath + ".key"; // Стандартное имя файла ключа (например, test.txt.key)

    if (choice == "1") {
        // --- РЕЖИМ ШИФРОВАНИЯ И СОЗДАНИЯ ФАЙЛА КЛЮЧА ---
        typeWrite("\n[+] Generating secure key bytes...\n");

        const char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        size_t alphabetSize = sizeof(alphabet) - 1;

        // Генерируем ключ
        for (long long i = 0; i < fileSize; ++i) {
            key += alphabet[rand() % alphabetSize];
        }

        // Автоматически записываем ключ в .key файл
        ofstream keyOut(keyFilePath.c_str(), ios::binary);
        if (!keyOut) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C);
            cout << "\n[ERROR] Could not create keyfile!" << endl;
            inFile.close();
            system("pause");
            return 1;
        }
        keyOut << key;
        keyOut.close();

        typeWrite("[+] Keyfile successfully dumped to disk.\n");
    }
    else if (choice == "2") {
        // --- РЕЖИМ РАСШИФРОВКИ С АВТОПОДБОРОМ ИЛИ ВВОДОМ ПУТИ ---
        typeWrite("\n[+] Checking for default keyfile: " + keyFilePath + "\n");

        // Пытаемся открыть ключ по умолчанию
        ifstream keyIn(keyFilePath.c_str(), ios::binary);

        // Если по умолчанию не нашли, просим пользователя ввести путь вручную
        if (!keyIn) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0E); // Желтый
            typeWrite("[!] Default keyfile not found. Enter custom keyfile path: ");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
            getline(cin, keyFilePath);

            keyIn.open(keyFilePath.c_str(), ios::binary);
            if (!keyIn) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C);
                cout << "\n[ERROR] Could not open specified keyfile!" << endl;
                inFile.close();
                system("pause");
                return 1;
            }
        } else {
            typeWrite("[+] Default keyfile found and hooked!\n");
        }

        // Читаем ключ из файла в строку
        getline(keyIn, key);
        keyIn.close();

        if (key.length() < static_cast<size_t>(fileSize)) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C);
            cout << "\n[ERROR] Keyfile is too short or corrupted!" << endl;
            inFile.close();
            system("pause");
            return 1;
        }
    }
    else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C);
        cout << "\n[ERROR] Invalid choice!" << endl;
        inFile.close();
        system("pause");
        return 1;
    }

    // Создаем временный файл для мутировавших данных
    string outPath = filePath + ".tmp";
    ofstream outFile(outPath.c_str(), ios::binary);

    typeWrite("[+] Executing cipher matrix...\n\n");

    char ch;
    long long byteCount = 0;

    while (inFile.get(ch)) {
        char cipherCh = ch ^ key[byteCount];
        outFile.put(cipherCh);

        byteCount++;
        if (byteCount % 500 == 0) {
            cout << "*";
            Beep(1000, 2);
        }
    }

    inFile.close();
    outFile.close();

    // Перезаписываем оригинальный файл
    remove(filePath.c_str());
    rename(outPath.c_str(), filePath.c_str());

    cout << "\n\n==================================================" << endl;
    if (choice == "1") {
        typeWrite("  SUCCESS: Cryptography sequence finished.\n");
        cout << "  Target file is now encrypted." << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0E); // Желтый
        cout << "  KEYFILE CREATED: " << keyFilePath << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
    } else {
        typeWrite("  SUCCESS: Decryption sequence finished.\n");
        cout << "  Target file restored to original state." << endl;

        // Интерактивная фишка: спрашиваем, удалить ли ключ за собой?
        typeWrite("\nWould you like to auto-delete the keyfile for safety? (y/n): ");
        string delChoice;
        getline(cin, delChoice);
        if (delChoice == "y" || delChoice == "Y") {
            remove(keyFilePath.c_str());
            typeWrite("[+] Keyfile shredded successfully.\n");
        }
    }
    cout << "==================================================" << endl;

    system("pause");
    return 0;
}
