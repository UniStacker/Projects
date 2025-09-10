#include <iostream>

char toLower(char ch) {
    if (ch >= 'a')
        return ch;
    else if (ch < 'A' || ch > 'Z') {
        // Red = "\033[31m"
        // Bold = "\033[1m"
        std::cout << "\033[1m\033[31m" // Styling
                  << "\ntoLower(): Invalid character: '"
                  << ch << "'\033[0m\n";
        exit(1);
    }
    return ch - 'A' + 'a';
}

bool confirm() {
    char reply {};
    while (reply != 'y' && reply != 'n') {
        std::cout << "(y/N) ";
        std::cin >> reply;
        reply = toLower(reply);
    }
    return reply == 'y';
}

int get_answer(char guess) {
    std::cout << "Is '" << guess << "' the character you are thinking? ";

    if (!confirm()) {
        std::cout << "Ok... does you'r character come after '" << guess << "'? ";
        if (confirm()) return 1;
        return -1;
    }
    return 0;
}

int main() {
    std::cout << "Think of any character in the alphabet, and I will guess it in under 10 questions.\n\n";

    bool run = true;
    int count = 0;

    char left = 'A';
    char right = 'Z';
    char guess {};

    while (run) {
        count += 2;
        guess = (left + right) / 2;
        if (left == right) {
            std::cout << "\nYou'r character must be '"
                      << guess
                      << "', if not, I can't guess it, this is already my "
                      << count-1 << "th turn, Sorry.\n";
            run = false;
            continue;
        }

        int answer = get_answer(guess);

        if (answer == 1)
            left = guess + 1;
        else if (answer == -1)
            right = guess - 1;
        else {
            count -= 1; // Minus the starting question
            std::cout << "\nTold you I could guess it, ";
            if (count == 1)
                std::cout << "and I did it in a single turn. Take that!!\n";
            else
                std::cout << "though it took me " << count << " turns. I could do better.\n";
            run = false;
        }
    }

    return 0;
}
