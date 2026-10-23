// Helpers.h
// Small functions that read input safely from the user.
// Each one keeps asking until the user types something valid.
#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

// ---------------------------------------------------------------------------
//  TIME helpers (used for the rental date, due time and late fee)
// ---------------------------------------------------------------------------

// The admin can move this "demo clock" forward to show a late return in class.
// It starts at 0 every time the program starts.
int clockOffsetHours = 0;

// Current time = real time + the demo clock offset.
time_t nowTime() {
    return time(NULL) + (time_t)clockOffsetHours * 3600;
}

// Turns a time into text like  23-Oct-2026 10:30
string formatTime(time_t t) {
    char buffer[40];
    struct tm *info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%d-%b-%Y %H:%M", info);
    return string(buffer);
}

// If the input has ended (for example Ctrl+Z), stop the program politely.
void checkInputEnded() {
    if (cin.eof()) {
        cout << "\nInput ended. Exiting.\n";
        exit(0);
    }
}

// Removes spaces / carriage return from both ends of a text.
string trimText(string s) {
    while (!s.empty() && (s[0] == ' ' || s[0] == '\t')) {
        s = s.substr(1);
    }
    while (!s.empty() && (s[s.size() - 1] == ' ' || s[s.size() - 1] == '\r' ||
                          s[s.size() - 1] == '\t' || s[s.size() - 1] == '\n')) {
        s = s.substr(0, s.size() - 1);
    }
    return s;
}

// Makes a text UPPER CASE (used for vehicle IDs).
string toUpperCase(string s) {
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            s[i] = s[i] - 'a' + 'A';
        }
    }
    return s;
}

// Reads a whole number between low and high.
int readInt(string prompt, int low, int high) {
    int x;
    while (true) {
        cout << prompt;
        cin >> x;
        if (cin.fail()) {
            checkInputEnded();
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  Invalid input. Enter a number from " << low << " to " << high << ".\n";
        } else {
            cin.ignore(1000, '\n');
            if (x >= low && x <= high) {
                return x;
            }
            cout << "  Invalid input. Enter a number from " << low << " to " << high << ".\n";
        }
    }
}

// Reads a number greater than 0 (it can have a decimal part).
double readPositiveDouble(string prompt) {
    double x;
    while (true) {
        cout << prompt;
        cin >> x;
        if (cin.fail()) {
            checkInputEnded();
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  Invalid input. Enter a number greater than 0.\n";
        } else {
            cin.ignore(1000, '\n');
            if (x > 0) {
                return x;
            }
            cout << "  Invalid input. Enter a number greater than 0.\n";
        }
    }
}

// Reads a line of text that is not empty (spaces are allowed).
string readText(string prompt) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (cin.fail()) {
            checkInputEnded();
        }
        s = trimText(s);
        if (!s.empty()) {
            return s;
        }
        cout << "  Input cannot be empty. Try again.\n";
    }
}

// Reads y or n.
bool readYesNo(string prompt) {
    while (true) {
        string s = toUpperCase(readText(prompt));
        if (s == "Y" || s == "YES") return true;
        if (s == "N" || s == "NO") return false;
        cout << "  Please type y or n.\n";
    }
}

// Reads a phone number: exactly 10 digits.
string readPhone(string prompt) {
    while (true) {
        string s = readText(prompt);
        bool ok = (s.size() == 10);
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i] < '0' || s[i] > '9') {
                ok = false;
            }
        }
        if (ok) {
            return s;
        }
        cout << "  Phone number must be exactly 10 digits.\n";
    }
}

#endif
