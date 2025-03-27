#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <vector>


using namespace std;

int pipe_in[2];
int pipe_out[2];
pid_t pid;

struct info_matrix{
    double* value;
    int size;
};

void help(){
    cout << "this code converts a matrix to an inverse matrix via the direct algebraic path" << endl;
    cout << "To get started you must enter --start, enter the size of the matrix and fill it with values" << endl;
}

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
            double val = adjugateMatrix[i][j] / det;

            inverseMatrix[i][j] = (val == 0) ? 0 : val;
        }
    }

    return inverseMatrix;
}

// Функция для вывода матрицы
void printMatrix(const vector<vector<double>>& matrix) {
    for (const auto& row : matrix) {
        for (double elem : row) {
            cout << (elem == 0 ? 0 : elem) << " ";
        }
        cout << endl;
    }
}

void frontend(){
    info_matrix info;
    cout << "Enter a size of matrix: " << endl;
    cin >> info.size;

    int total_elements = info.size * info.size;
    info.value = new double[total_elements];

    cout << "Enter " << total_elements << " elements of matrix: " << endl;
    for(int i = 0; i < total_elements; i++){
        cin >> info.value[i];
    }
    write(pipe_in[1], &info.size, sizeof(int));
    write(pipe_in[1], info.value, sizeof(double) * total_elements);
    delete info.value;

    // Получение результата из backend
    info_matrix result_info;
    read(pipe_out[0], &result_info.size, sizeof(int));
    
    int result_elements = result_info.size * result_info.size;
    result_info.value = new double[result_elements];
    read(pipe_out[0], result_info.value, sizeof(double) * result_elements);

    // Вывод результата в консоль
    cout << "Inverse matrix:" << endl;
    int index = 0;
    for (int i = 0; i < result_info.size; i++) {
        for (int j = 0; j < result_info.size; j++) {
            double val = result_info.value[index++];
            cout << val << " ";
        }
        cout << endl;
    }
    delete[] result_info.value;
    exit(0);
}

void backend(){
    info_matrix info;
    read(pipe_in[0], &info.size, sizeof(int));

    int total_elements = info.size * info.size;
    info.value = new double[total_elements];
    read(pipe_in[0], info.value, sizeof(double) * total_elements);

    vector<vector<double>> matrix(info.size, vector<double>(info.size));

    int index = 0;
    for (int i = 0; i < info.size; i++){
        for (int j = 0; j < info.size; j++){
            matrix[i][j] = info.value[index++];
        }
    }

    delete info.value;

    vector<vector<double>> inverse_matrix;
    try {
        inverse_matrix = inverseMatrix(matrix);
    } catch (const exception& e) {
        info_matrix error_info;
        error_info.size = 0;
        error_info.value = nullptr;
        write(pipe_out[1], &error_info.size, sizeof(int));
        cerr << "Error: " << e.what() << endl;
        exit(1);
    }

    // Подготовка результата для отправки
    info_matrix result_info;
    result_info.size = inverse_matrix.size();
    int result_elements = result_info.size * result_info.size;
    result_info.value = new double[result_elements];
    
    index = 0;
    for (const auto& row : inverse_matrix) {
        for (double val : row) {
            result_info.value[index++] = val;
        }
    }

    write(pipe_out[1], &result_info.size, sizeof(int));
    write(pipe_out[1], result_info.value, sizeof(double) * result_elements);
    delete[] result_info.value;
}

int main(){
    cout << "Hello\nThis program is designed to find the inverse of a matrix" << endl;

    cout << "Enter --help or --start or anything else for exit ";

    string action;
    cin >> action;

    if (action == "--help") {
        help();
        exit(0);
    }
    else if(action == "--start"){
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
    else{
        exit(0);
    }
    return 0;
}