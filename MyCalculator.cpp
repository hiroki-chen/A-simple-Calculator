#include "MyCalculator.hpp"

int main(int argc, const char** argv) {
    std::string in;
    std::cout << "Please input an expession. You should add a space between each operator including parenthesis, "
    << "but do not split the minus sign!" << std::endl;
    getline(std::cin, in);

    std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now();

    try {
        Calculator c(in);
        c.doCalculate();
        std::cout << c.ans() << std::endl;
    } catch (InputMismatchException e) {
        std::cout << e.what() << std::endl;
    }

    std::cout << getTime(beginTime) << " ms" << std::endl;

    return 0;
}