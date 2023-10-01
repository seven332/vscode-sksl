#include <cstdlib>
#include <iostream>
#include <array>
#include <string>
#include <cstring>

static std::string getLine() {
    std::string result;
    while (true) {
        char c = std::cin.get();
        if (c == 10) {
            return result;
        } else {
            result += c;
        }
    }
}

extern "C" {

int main(void) {
    while(true) {
        auto method = getLine();
        auto params = getLine();
        std::cout << "method: " << method << ", params: " << params << std::endl;
        // TOOD:
    }
    return 0;
}

}
