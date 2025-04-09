#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable: 4996)

using namespace std;

const int PORT = 8080;
const char IP[] = "127.0.0.1";

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Error" << endl;
        system("pause");
        return 1;
    }

    int lifetime = std::stoi(argv[1]);
    cout << "Client started. Lifetime: " << (lifetime == 0 ? "infinite" : std::to_string(lifetime) + "s") << "\n";

    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed\n";
        system("pause");
        return 1;
    }

    // Создание сокета
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Socket creation failed\n";
        WSACleanup();
        system("pause");
        return 1;
    }

    // Подключение к серверу
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(IP);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Connect failed: " << WSAGetLastError() << "\n";
        closesocket(sock);
        WSACleanup();
        system("pause");
        return 1;
    }

    // Ожидание времени жизни
    if (lifetime > 0) {
        cout << "Waiting for " << lifetime << " seconds...\n";
        for (int i = 0; i < lifetime; ++i) {
            Sleep(1000);
            cout << "Time left: " << (lifetime - i) << "s\n";
        }
    } else {
        cout << "Running in infinite mode.\n";
        while (true) {
            Sleep(1000);
        }
    }

    closesocket(sock);
    WSACleanup();
    
    // Ожидание перед закрытием
    if (lifetime > 0) {
        cout << "Client finished. Press any key to exit...\n";
        system("pause");
    }
    return 0;
}