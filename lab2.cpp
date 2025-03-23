#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <fstream>


using namespace std;


void copyFile(){
    string initialFile, copyFile;
    cout << "Enter a file name: ";
    cin >> initialFile;
    cout << "Enter a copy file name: ";
    cin >> copyFile;

    if(initialFile == copyFile) {
        cout << "Error: File names should be different" << endl;
        return;
    }

    ifstream fin;
    size_t bufsize = 4;
    char* buf = new char[bufsize];
    fin.open(initialFile, ios::binary);
    if(fin.is_open()){
        ofstream fout;
        fout.open(copyFile, ios::binary);
        while(!fin.eof()) {
            fin.read(buf, bufsize);
            if (fin.gcount()) fout.write(buf, fin.gcount());
        }
        fin.close();
        fout.close();
        delete[] buf;
        cout << "File copied successfully" << endl;
    }else{
        return;
    }
}

void moveFile(){
    string oldFilePath, newFilePath;
    cout << "Enter a source: ";
    cin >> oldFilePath;
    cout << "Enter newFilePath: ";
    cin >> newFilePath;
    const char* oldpath = oldFilePath.c_str();
    const char* newpath = newFilePath.c_str();
    int ret = rename(oldpath, newpath);
    if(ret == 0) {
        cout << "File moved successfully" << endl;
    } else perror("Error moving file");
}

void permissionFile(){
    string filename, permission;
    cout << "Enter file name: ";
    cin >> filename;
    cout << "Enter permission (e.g 0755 for rwxr-xr-x): ";
    cin >> permission;
    int mode = stoi(permission, 0, 8);

    if (chmod(filename.c_str(), mode) == 0) {
        cout << "File permission changed successfully." << endl;
    } else {
        cerr << "Error: Unable to change file permission." << endl;
    }

}
void infoFile(){
    string name;
    cout << "Enter file name: ";
    cin >> name;
    const char* file= name.c_str();
    struct stat file_info;
    if(stat(file, &file_info) == 0) {
        cout << ((file_info.st_mode & S_IRUSR) ? "r" : "-");
        cout << ((file_info.st_mode & S_IWUSR) ? "w" : "-");
        cout << ((file_info.st_mode & S_IXUSR) ? "x" : "-");
        cout << ((file_info.st_mode & S_IRGRP) ? "r" : "-");
        cout << ((file_info.st_mode & S_IWGRP) ? "w" : "-");
        cout << ((file_info.st_mode & S_IXGRP) ? "x" : "-");
        cout << ((file_info.st_mode & S_IROTH) ? "r" : "-");
        cout << ((file_info.st_mode & S_IWOTH) ? "w" : "-");
        cout << ((file_info.st_mode & S_IXOTH) ? "x" : "-") << endl;
        cout << "File size: " << file_info.st_size << endl;
        cout << "Last modification time: " << ctime(&file_info.st_mtime);
    } else perror("Error: ");

}

void getHelp(){
    cout << "Available actions:" << endl;
    cout << "--copy: Copy file" << endl;
    cout << "--move: Move file" << endl;
    cout << "--permission: Change file permission" << endl;
    cout << "--info: Get file information" << endl; 
}

void showFile(){
    string filename;
    cout << "Enter a file name: ";
    cin >> filename;
    ifstream fin(filename);
    if(!fin.is_open()){
        cout << "Error: File not found" << endl;
        return;
    }
    string Line;
    int LineNumber = 0;

    while(getline(fin, Line)){
        LineNumber++;
        if(LineNumber %2 != 0){
            cout << Line << endl;
        }
    }
    fin.close();
}

void greeting(){
    while(true){
        string action;
        cout << "Enter action or --help for help: ";
        cin >> action;
        if(action == "--copy")copyFile();
        else if(action == "--move")moveFile();
        else if(action == "--permission")permissionFile();
        else if(action == "--info")infoFile();
        else if(action == "--help")getHelp();
        else if(action == "show")showFile();
        else if(action == "--exit")break;
        else cout << action << ": command not found" << endl;
    }
}
int main(){
    greeting();
    return 0;
}