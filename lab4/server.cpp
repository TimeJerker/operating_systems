#include <iostream>
#include <winsock2.h>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable: 4996)

using namespace std;

const int PORT = 8080;
const char IP[] = "127.0.0.1";

// Глобальный счётчик клиентов
atomic<int> clientCounter{1};
mutex coutMutex;  // Мьютекс для синхронизации вывода

void StartClient(int lifetime) {
    STARTUPINFOW si = { sizeof(si) };  // Явно используем Unicode-версию
    PROCESS_INFORMATION pi;
    wstring cmd = L"client.exe " + to_wstring(lifetime);
    CreateProcessW(
        NULL,
        &cmd[0],
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi
    );
}

void SafePrint(const string& message) {
    lock_guard<mutex> lock(coutMutex);
    cout << message << endl;
}

void HandleClient(SOCKET clientSocket, int clientId, int lifetime) {
    SafePrint("[Client #" + to_string(clientId) + "] Started (Lifetime: " + (lifetime == 0 ? "infinite)" : std::to_string(lifetime) + "s)"));

    char buffer[1024];
    auto start = chrono::steady_clock::now();
    while (true) {
        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(now - start).count();
        

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            if(bytesReceived == 0){
                SafePrint("[Client #" + to_string(clientId) + "] Disconnected (Gracefull)");
            }else{
                SafePrint("[Client #" + to_string(clientId) + "] Disconnected (Error: " + to_string(WSAGetLastError()) + ")");
            }
            break;
        }
    }
    closesocket(clientSocket);
}

int main() {
    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed\n";
        return 1;
    }

    // Настройка сервера
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(IP);
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Ввод параметров
    vector<int> lifetimes;
    int clientCount;
    cout << "Enter number of clients: ";
    cin >> clientCount;

    for (int i = 0; i < clientCount; ++i) {
        int lifetime;
        cout << "Enter lifetime for Client #" << i + 1 << ": ";
        cin >> lifetime;
        lifetimes.push_back(lifetime);
        StartClient(lifetime);
    }

    // Приём подключений
    vector<thread> threads;
    for (int i = 0; i < clientCount; ++i) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Accept failed\n";
            continue;
        }
        threads.emplace_back(HandleClient, clientSocket, clientCounter++, lifetimes[i]);
    }

    // Ожидание завершения
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}