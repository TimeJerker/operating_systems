#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <iterator>
#include <algorithm>
//#include <cstring>
//#include <cmath>

using namespace std;

int pipe_in[2];
int pipe_out[2];
pid_t pid;

struct info_matrix{
    vector<double> value;
    int size;
};

void help(){

}
#include <iostream>
#include <vector>

using namespace std;

// Функция для вычисления определителя матрицы 2x2
double determinant2x2(const vector<vector<double>>& matrix) {
    return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
}

// Функция для вычисления определителя матрицы nxn
double determinant(const vector<vector<double>>& matrix) {
    int n = matrix.size();
    if (n == 1) {
        return matrix[0][0];
    }
    if (n == 2) {
        return determinant2x2(matrix);
    }

    double det = 0;
    for (int col = 0; col < n; ++col) {
        // Создаем подматрицу для минора
        vector<vector<double>> submatrix(n - 1, vector<double>(n - 1));
        for (int i = 1; i < n; ++i) {
            int subcol = 0;
            for (int j = 0; j < n; ++j) {
                if (j == col) continue;
                submatrix[i - 1][subcol] = matrix[i][j];
                subcol++;
            }
        }

        // Рекурсивно вычисляем определитель подматрицы
        double subdet = determinant(submatrix);
        det += matrix[0][col] * subdet * ((col % 2 == 0) ? 1 : -1);
    }
    return det;
}

// Функция для вычисления матрицы алгебраических дополнений
vector<vector<double>> getCofactorMatrix(const vector<vector<double>>& matrix) {
    int n = matrix.size();
    vector<vector<double>> cofactorMatrix(n, vector<double>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            // Создаем подматрицу для минора
            vector<vector<double>> submatrix(n - 1, vector<double>(n - 1));
            int subrow = 0;
            for (int k = 0; k < n; ++k) {
                if (k == i) continue;
                int subcol = 0;
                for (int l = 0; l < n; ++l) {
                    if (l == j) continue;
                    submatrix[subrow][subcol] = matrix[k][l];
                    subcol++;
                }
                subrow++;
            }

            // Вычисляем минор и алгебраическое дополнение
            double minor = determinant(submatrix);
            cofactorMatrix[i][j] = minor * (((i + j) % 2 == 0) ? 1 : -1);
        }
    }
    return cofactorMatrix;
}

// Функция для транспонирования матрицы
vector<vector<double>> transposeMatrix(const vector<vector<double>>& matrix) {
    int n = matrix.size();
    vector<vector<double>> transposedMatrix(n, vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            transposedMatrix[j][i] = matrix[i][j];
        }
    }
    return transposedMatrix;
}

// Функция для вычисления обратной матрицы
vector<vector<double>> inverseMatrix(const vector<vector<double>>& matrix) {
    int n = matrix.size();
    double det = determinant(matrix);
    if (det == 0) {
        throw runtime_error("Матрица вырождена, обратной матрицы не существует.");
    }

    // Вычисляем матрицу алгебраических дополнений
    vector<vector<double>> cofactorMatrix = getCofactorMatrix(matrix);

    // Транспонируем матрицу алгебраических дополнений (присоединенная матрица)
    vector<vector<double>> adjugateMatrix = transposeMatrix(cofactorMatrix);

    // Вычисляем обратную матрицу
    vector<vector<double>> inverseMatrix(n, vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            inverseMatrix[i][j] = adjugateMatrix[i][j] / det;
        }
    }

    return inverseMatrix;
}

// Функция для вывода матрицы
void printMatrix(const vector<vector<double>>& matrix) {
    for (const auto& row : matrix) {
        for (double elem : row) {
            cout << elem << " ";
        }
        cout << endl;
    }
}

void frontend(){
    info_matrix info;
    cout << "Enter a size of matrix: " << endl;
    cin >> info.size;

    cout << "Enter elements of matrix: " << endl;
    copy(
        istream_iterator<int>(cin), //итератор на начало ввода
        istream_iterator<int>(), // итератор на конец ввода
        back_inserter(info.value) // вставка в вектор
    );
    write(pipe_in[1], &info, sizeof(info_matrix));
}

void backend(){
    info_matrix info;
    read(pipe_in[0], &info, sizeof(info_matrix));
    vector<vector<double>> matrix(info.size, vector<double>(info.size));

    
    for (int i = 0; i < info.size; i++){
        for (int j = 0; j < info.size; j++){
            cin >> matrix[i][j];
        }
    }

    try {
        vector<vector<double>> inverse_matrix = inverseMatrix(matrix);
        cout << "Inverse matrix: " << endl;
        printMatrix(inverse_matrix);
    } catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
    }
}
int main(){
    cout << "Hello\nThis program is designed to find the inverse of a matrix" << endl;
    pipe(pipe_in);
    pipe(pipe_out);
    pid = fork();

    if(pid < 0){
        perror("fork failed");
        exit(1);

    }
    else if(pid > 0){
        frontend();
    }
    else{
        backend();
    }
    for(int i = 0; i < 2; i++){
        close(pipe_in[i]);
        close(pipe_out[i]);
    }
        
}