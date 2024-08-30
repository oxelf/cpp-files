#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <thread>
using namespace std;
using clock_type = std::chrono::high_resolution_clock;

int getRandomNumber(int max) {
    return rand() % max;
}

void showWin(float won) {
    //prints 50 - for the border of the rectangle
    cout << "\x1B[32m ";
    for (int i = 0; i < 50; i++) {
        cout << "-";
    }
    cout << endl;

    // looping because we need to add the spaces to make the text centered
    cout << "";
    for (int i = 0; i < 50; i++) {
        if (i == 20) {
            string text = "YOU WON!!!";
            cout << text;
            i += text.length();
        }
        cout << " ";
    }
    cout << " " << endl;

    // print the num centered
    cout << "";
    for (int i = 0; i < 50; i++) {
        if (i == 21) {
            printf("%.2f", won);
            i += 6;
        }
        cout << " ";
    }
    cout << " " << endl;

    //print 50 - for the bottom border
    cout << " ";
    for (int i = 0; i < 50; i++) {
        cout << "-";
    }
    cout << "\033[0m\t\t" << endl;
}

class SlotMachine {
public:
    int num1{}, num2{}, num3 = 0;
    float balance{};
    float pod{};
    vector<string> colors = {
        "\x1B[31m", "\x1B[32m", "\x1B[33m", "\x1B[34m", "\x1B[35m", "\x1B[36m", "\x1B[37m", "\x1B[92m", "\x1B[96m",
        "\x1B[95m"
    };

    void generateNewNumbers() {
        num1 = getRandomNumber(10);
        num2 = getRandomNumber(10);
        num3 = getRandomNumber(10);
    }

    void readBalance() {
        string fileContent;

        ifstream MyReadFile("slots.txt");
        while (getline(MyReadFile, fileContent)) {
            cout << fileContent;
        }
        // Close the file
        MyReadFile.close();

        cout << "file content: " << fileContent << endl;

        if (fileContent.empty()) {
            balance = 500;
        } else {
            float newBalance = stof(fileContent);

            balance = newBalance;
        }
    }

    void writeBalance() const {
        ofstream MyFile("slots.txt");

        // Write to the file
        MyFile << balance;

        // Close the file
        MyFile.close();
    }
};

void clearScreen() {
#if defined(__linux__) // Or #if __linux__
    system("clear");
#elif _WIN32
   system("cls");
#else
    system("clear");
#endif
}

void buildSlotMachine(SlotMachine const *slot_machine) {
    cout << " _____\n|";
    cout << slot_machine->colors[slot_machine->num1] << slot_machine->num1 << "\033[0m|";
    cout << slot_machine->colors[slot_machine->num2] << slot_machine->num2 << "\033[0m|";
    cout << slot_machine->colors[slot_machine->num3] << slot_machine->num3 << "\033[0m|";
    cout << "\n -----\n";
}

void showBalance(SlotMachine const *slot_machine) {
    cout << "| balance: " << slot_machine->balance << " | ";
    if (slot_machine->pod != 0) {
        cout << "pod: " << slot_machine->pod << " | " << endl;
    } else {
        cout << endl;
    }
}


void runGame(SlotMachine *slot_machine) {
    auto const when_started = clock_type::now();
    auto target_time = when_started + 500ms;
    float won = 0;
    for (int i = 0; i < 12; i++) {
        slot_machine->generateNewNumbers();
        clearScreen();
        if (i == 11) {
            if (slot_machine->num1 == 7 && slot_machine->num2 == 7 && slot_machine->num3 == 7) {
                won = slot_machine->pod * 100;
                showWin(won);
                slot_machine->balance = slot_machine->balance + won;
            } else if (slot_machine->num1 == slot_machine->num2 && slot_machine->num2 == slot_machine->num3) {
                won = slot_machine->pod * 10;
                slot_machine->balance = slot_machine->balance + won;
                showWin(won);
            } else {
                slot_machine->balance = slot_machine->balance + (slot_machine->pod * -1);
            }
        }
        showBalance(slot_machine);
        buildSlotMachine(slot_machine);
        std::this_thread::sleep_until(target_time);
        target_time += 500ms;
    }
    slot_machine->writeBalance();
}

int main() {
    auto slot_machine = SlotMachine();
    slot_machine.readBalance();
    slot_machine.pod = 0;

    bool afterGame = false;

    while (true) {
        if (!afterGame) {
            clearScreen();
            showBalance(&slot_machine);
            buildSlotMachine(&slot_machine);
        }
        cout << "with how much you wanna play? ";
        float newPod;
        cin >> newPod;
        if (newPod > slot_machine.balance) {
            cout << "thats more than you have. try a smaller amount. \n";
        } else {
            slot_machine.pod = newPod;
            runGame(&slot_machine);
            afterGame = true;
        }
    }
}
