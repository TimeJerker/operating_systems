#include <iostream>
#include <limits>
#include <string>
using namespace std;


void Program() {
    cout << "Hello!\n";
    cout << "This program find a max and min number in input" << endl;
}

void uncorrect() {
    // Возврат оператора ввода в состояние без ошибок
    cin.clear();
    
    // Очистка потока ввода до конца текущей строки, игнорируя оставшиеся символы
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Invalid input! Please, try again." << endl;
}


void findMaxAndMinDigits(long long number, int &maxDigit, int &minDigit, int &mediumDigit) {

    cout << "Enter a positive number: ";
    cin >> number;

    
    // Инициализация переменных
    maxDigit = 0;
    minDigit = 9;
    mediumDigit = -1;

    // Обработка случая, когда число равно 0
    if (number == 0) {
        maxDigit = 0;
        minDigit = 0;
        return;
    }

    int new_number = number;
    string numStr = to_string(new_number);
    
    int lenght = numStr.length();

    if(lenght % 2 == 1) {
        int middleDigit = lenght / 2;
        mediumDigit = numStr[middleDigit] - '0';
    }

    // Проверка на положительность введенного числа
    while (number < 0 || typeid(number)!= typeid(long long) || cin.fail()) {
        uncorrect();
        cout << "Enter a positive number: ";
        cin >> number;
    }

    // Поиск максимальной и минимальной цифры
    while (number > 0) {
        int currentDigit = number % 10;
        if (currentDigit > maxDigit) {
            maxDigit = currentDigit;
        }
        if (currentDigit < minDigit) {
            minDigit = currentDigit;
        }
        number /= 10;
    }
}



char valid_continue() {
    cout << "Do you want to continue? (y/n) >> ";
    char answer;
    cin >> answer;

    // Приведение к нижнему регистру для удобства сравнения
    answer = tolower(answer);

    // Проверка ввода на допустимые значения
    while (answer != 'y' && answer != 'n' || cin.peek() != '\n') {
        uncorrect();
        cout << "Do you want to continue? (y/n) >> ";
        cin >> answer;
    }
    return answer;
}

// Функция для нахождения максимальной и минимальной цифры в числе
int main() {
    char answer;
    Program();

    do{
        long long number;
        int maxDigit, minDigit, mediumDigit;

        // Поиск максимальной и минимальной цифры в числе
        findMaxAndMinDigits(number, maxDigit, minDigit, mediumDigit);

        // Вывод результата
        cout << "Max digit: " << maxDigit << endl;
        cout << "Min digit: " << minDigit << endl;
        if(mediumDigit!= -1){
            cout << "Medium digit: " << mediumDigit << endl;
        }
        else{
            cout << "Medium Digit: Число имеет четное количество цифр" << endl;
        }

        answer = valid_continue();
    }while (answer == 'y');
    
    cout << "Thanks for using this program.\n" << "Goodbye!";
    return 0;
}