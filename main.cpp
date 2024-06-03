#include <filesystem>
#include <iostream>
#include <string.h>
#include <fstream>
#include <stack>
#include <queue>
using namespace std;

char* getUserText() {
    cout << "Enter text to append: " << endl;
    int allocated_size = 10;
    int len = 0;
    auto userText = static_cast<char *>(calloc(allocated_size, sizeof(char)));
    char ch;
    do {
        ch = getchar();
        userText[len] = ch;
        len++;

        if (len > allocated_size) {
            allocated_size *= 2;
            char *temp = static_cast<char *>(realloc(userText, allocated_size * sizeof(char)));
            if (temp == nullptr) {
                free(userText);
                exit(1);
            }
            userText = temp;
        }
    } while(ch != '\n');
    if (userText[len-1] == '\n'){
        userText[len-1] = '\0';}
    return userText;
    free(userText);
}

class Text {
    char* text;
    char* buffer;
    stack<char*> undoStack;
    queue<char*> redoStack;
public:
    Text() {
        text = static_cast<char *>(calloc(10, sizeof(char)));
        buffer = static_cast<char *>(calloc(10, sizeof(char)));
    }
    void append(char* userText) {
        char* undoCopy = new char[strlen(text) + 1];
        strcpy(undoCopy, text);
        undoStack.push(undoCopy);
        if (strlen(text) + strlen(userText) > sizeof(text)) {
            text = static_cast<char *>(realloc(text, (strlen(text) + strlen(userText)) * sizeof(char)));
        }
        strcat(text, userText);
        free(userText);
        char* redoCopy = new char[strlen(text) + 1];
        strcpy(redoCopy, text);
        redoStack.push(redoCopy);
    }

    void show() const {
        cout << text << endl;
    }

    void newline() const {
        strcat(text, "\n");
    }

    void insert(int line, int index, char* userText) {
        int k = 0;

        if (strlen(text) + strlen(userText) > sizeof(text)) {
            text = (char*)realloc(text, (strlen(text) + strlen(userText))*2 * sizeof(char));
        }

        int sizeInput = strlen(userText);
        int sizeText = strlen(text);

        for (int i = 0; i < sizeText; i++) {
            if (k==line) {
                for (int j = sizeText + sizeInput; j > i + index; j--) {
                    text[j] = text[j - sizeInput];
                }
                for (int m = i + index; m < i + index + sizeInput; m++) {
                    text[m] = userText[m - i - index];
                }
                break;
            }
            else {
                if (text[i] == '\n') {k++;}
            }
        }
        free(userText);
    }

    void search(char* userText) {
        int k = 0;
        int line = 0;
        int n = 0;
        int indexes = 0;

        int sizeText = strlen(text);
        int sizeInput = strlen(userText);
        for (int i=0; i < sizeText; i++) {
            if (text[i] == '\n') {line++; indexes += n; n=0;}
            else {n++;}
            if (text[i] == userText[k]) {
                k++;
                if (k == sizeInput) {
                    cout<< line<< i - sizeInput + 1 - line - indexes << endl;
                    k = 0;
                }
            }
            else{
                if (text[i] == userText[0]){k = 1;}
                else {k = 0;}
            }
        }
    }

    void pack(char *nameFile, char mode) {
        ofstream file;
        if (mode == 'w')
        {file.open(nameFile, std::ios_base::out);}
        else
        {file.open(nameFile, std::ios_base::app);}

        if (file.is_open()) { 
            file << text; 
            file.close(); 
            std::cout << "Text has been saved successfully \n";
        }
        else {
            std::cerr << "Error opening file\n";
        }
    }

    void load(char* nameFile) {
        ifstream file (nameFile);

        if (file.is_open())
        {
            int allocated_size = 10;
            int len = 0;

            char ch;
            while(file.get(ch)) {
                text[len] = ch;
                len++;

                if (len >= allocated_size) {
                    allocated_size *= 2;
                    char *temp = static_cast<char *>(realloc(text, allocated_size * sizeof(char)));
                    if (temp == nullptr) {
                        free(text);
                    }
                    text = temp;
                }
            }
            text[len] = '\0';
            file.close();
        }
        else cout << "Error opening file";
    }

    void replacement(int line, int index, char* userText) {
        int k = 0;
        int sizeInput = strlen(userText);
        int sizeText = strlen(text);
        for (int i = 0; i < sizeText; i++) {
            if (k==line) {
                for (int m = i + index; m < i + index + sizeInput; m++) {
                    text[m] = userText[m - i - index];
                }
                break;
            }
            else {
                if (text[i] == '\n') {k++;}
            }
        }
        free(userText);
    }

    void del(int line, int index, int number){
        int k = 0;
        int sizeText = strlen(text);
        if (sizeText + number > sizeof(text)) {
            text = static_cast<char *>(realloc(text, (strlen(text) + number) * 2 * sizeof(char)));
        }
        for (int i = 0; i < sizeText; i++) {
            if (k==line) {
                for (int m = i + index + number; m < i + sizeText + number + index; m++) {
                    text[m - number] = text[m];
                }
                break;
            }
            else {
                if (text[i] == '\n') {k++;}
            }
        }
    }

    void copy(int line, int index, int number) {
        int k = 0;
        //char* buffer = static_cast<char *>(calloc(10, sizeof(char)));
        int sizeText = strlen(text);
        for (int i = 0; i < sizeText; i++) {
            if (k==line) {
                for (int m = i + index; m < i + index + number; m++) {
                    buffer[m - i - index] = text[m];
                }
                for (int j = number; j < strlen(buffer); j++) {
                    buffer[j] = '\0';
                }
                break;
            }
            else {
                if (text[i] == '\n') {k++;}
            }
        }
        cout << buffer << endl;
    }

    char* getBuffer() {
        return buffer;
    }

    void undo() {
        if (!undoStack.empty()) {
            free(text);
            text = undoStack.top();
            undoStack.pop();
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            free(text);
            text = redoStack.front();
            redoStack.pop();
        }
    }
    ~Text() {
        free(text);
    }
};



int main()
{
    Text text;
    for (;;) {
        int c;
        cout <<"\nChoose the command: " << endl;
        cin >> c;
        fflush(stdin);
        switch (c) {
            case 1:
                text.append(getUserText());
                break;
            case 2:
                text.newline();
                break;
            case 3:
                char nameFile[20];
                char mode;
                cout << "\nEnter the file name for saving and the mode (w/a): " << endl;
                cin >> nameFile >> mode;
                fflush(stdin);
                text.pack(nameFile, mode);
                break;
            case 4:
                char nameFile1[20];
                cout << "\nEnter the file name for loading: " << endl;
                cin >> nameFile1;
                text.load(nameFile1);
                break;
            case 5:
                text.show();
                break;
            case 6: {
                int line, index;
                cout << "\nChoose line and index: "<<endl;
                cin >> line >> index;
                fflush(stdin);
                text.insert(line, index, getUserText());
                break;
            }
            case 7:
                text.search(getUserText());
                break;

            case 8:
                int line2, index2, number;
                cout << "Choose line, index and number of symbols: "<<endl;
                cin >> line2 >> index2 >> number;
                text.del(line2, index2, number);
                break;
            case 9:
                text.undo();
                break;
            case 10:
                text.redo();
                break;
            case 11:
                int line4, index4, number2;
                cout << "Choose line, index and number of symbols: "<<endl;
                cin >> line4 >> index4 >> number2;
                text.copy(line4, index4, number2);
                text.del(line4, index4, number2);
                break;
            case 12:
                int line3, index3, number1;
                cout << "Choose line, index and number of symbols: "<<endl;
                cin >> line3 >> index3 >> number1;
                text.copy(line3, index3, number1);
                break;
            case 13:
                int line5, index5;
                cout << "Choose line, index: "<<endl;
                cin >> line5 >> index5;
                text.insert(line5, index5, text.getBuffer());
                break;
            case 14:
                int line1, index1;
                cout << "\nChoose line and index: "<<endl;
                cin >> line1 >> index1;
                fflush(stdin);
                text.replacement(line1, index1, getUserText());
                break;
            default:
                cout << "This command is not avaible" <<endl;
        }
    }
    return 0;
}
