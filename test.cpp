#include <iostream>
#include <vector>
using namespace std;

// Функция для вычисления определителя матрицы
double determinant(vector<vector<double>> mat) {
    int n = mat.size();
    double det = 1;
    for (int i = 0; i < n; i++) {
        for (int k = i + 1; k < n; k++) {
            double factor = mat[k][i] / mat[i][i];
            for (int j = i; j < n; j++)
                mat[k][j] -= factor * mat[i][j];
        }
        det *= mat[i][i];
    }
    return det;
}

// Функция для нахождения обратной матрицы
vector<vector<double>> inverse(vector<vector<double>> mat) {
    int n = mat.size();
    vector<vector<double>> inv(n, vector<double>(n, 0));
    
    // Создаем единичную матрицу (это будет наша обратная матрица в процессе)
    for (int i = 0; i < n; i++) inv[i][i] = 1;
    
    // Прямой ход метода Гаусса
    for (int i = 0; i < n; i++) {
        double diag = mat[i][i];
        for (int j = 0; j < n; j++) {
            mat[i][j] /= diag;
            inv[i][j] /= diag;
        }
        for (int k = i + 1; k < n; k++) {
            double factor = mat[k][i];
            for (int j = 0; j < n; j++) {
                mat[k][j] -= factor * mat[i][j];
                inv[k][j] -= factor * inv[i][j];
            }
        }
    }
    
    // Обратный ход метода Гаусса
    for (int i = n - 1; i >= 0; i--) {
        for (int k = i - 1; k >= 0; k--) {
            double factor = mat[k][i];
            for (int j = 0; j < n; j++) {
                mat[k][j] -= factor * mat[i][j];
                inv[k][j] -= factor * inv[i][j];
            }
        }
    }
    
    return inv;
}

int main() {
    int n; cout << "Размер матрицы: "; cin >> n;
    vector<vector<double>> mat(n, vector<double>(n));
    
    cout << "Введите матрицу:\n";
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            cin >> mat[i][j];
    
    if (determinant(mat) == 0) {
        cout << "Обратной матрицы не существует!\n";
        return 0;
    }
    
    vector<vector<double>> inv = inverse(mat);
    
    cout << "Обратная матрица:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << inv[i][j] << " ";
        cout << "\n";
    }
    return 0;
}