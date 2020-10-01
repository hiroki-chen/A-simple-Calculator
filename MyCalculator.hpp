#ifndef _MYCALCULATOR_HPP_
#define _MYCALCULATOR_HPP_

#include <sstream>
#include <iostream>
#include <string>
#include <stack>
#include <exception>
#include <queue>
#include <cmath>
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

long getTime(high_resolution_clock::time_point beginTime) {
    high_resolution_clock::time_point endTime = high_resolution_clock::now();
    milliseconds timeInterval = std::chrono::duration_cast<milliseconds>(endTime - beginTime);
    return timeInterval.count();
}

class InputMismatchException : public std::exception {
    const char* info;

public:
    const char* what() const noexcept override { return this->info; }
    InputMismatchException() : info("Wrong Input! Perhaps you have input an unvalid number or you forgot to add necessary spaces.") { }
};

class DividendZeroException : public std::exception {
const char* info;

public:
    const char* what() const noexcept override { return this->info; }
    DividendZeroException() : info("Divident cannot be zero!!!") { }
};

class Calculator {
    std::string input;
    std::stack<char> operators;
    

    double res;

    void checkInputValid(void);
    bool checkParenthesis(void);
    int getPriority(const char& op);
    double calculate(double num1, double num2, const char& op);
    void ManyCalculate(std::queue<char>& tmp);
    void computationPreamble(void);

public:
    std::stack<double> numbers;
    Calculator(const std::string& input);

    void doCalculate(void);
    double ans(void);
};

Calculator::Calculator(const std::string& input) {
    this->input = input;
    this->checkInputValid();
    this->operators.push('#');
}

bool Calculator::checkParenthesis(void) {
    int cnt = 0;

    for (char c : this->input) {
        if (c == '(') { cnt++; }
        else if (c == ')') { cnt--;}
    }

    return cnt == 0;
}

void Calculator::ManyCalculate(std::queue<char>& tmp) {
    //std::cout << "Empty?" << tmp.empty() << std::endl;
    while (!tmp.empty()) {
        double num1 = this->numbers.top(); this->numbers.pop();
        double num2 = this->numbers.top(); this->numbers.pop();
        char op = tmp.front();
        tmp.pop();
        //std::cout << "pushing number: " << this->calculate(num1, num2, op) << std::endl;
        this->numbers.push(this->calculate(num1, num2, op));
    }
}

void Calculator::computationPreamble(void) {
    char op = this->operators.top(); this->operators.pop();
    double num1 = this->numbers.top(); this->numbers.pop();
    double num2 = this->numbers.top(); this->numbers.pop();
    this->numbers.push(this->calculate(num1, num2, op));
}

void Calculator::checkInputValid(void) {
    if (!this->checkParenthesis()) { throw InputMismatchException(); }

    std::istringstream iss(this->input + " ");
    std::string buf;

    int op = 0, num = 0;

    while (getline(iss, buf, ' ')) {
        bool isNum = true;
        
        if (buf.compare(" ") == 0) { continue; }
        //A negative number.
        if (buf.size() > 1 && buf[0] == '-') {
            bool decimal = false;
            for (int i = 1; i < buf.size(); i ++) {
                if (!isdigit(buf[i])) {
                    if (buf[i] != '.' || (buf[i] == '.' && decimal)) { throw InputMismatchException(); }
                    else { decimal = true; }
                }
            }
            num += 1;
        } else {
            bool decimal = false;
            for (auto c : buf) {
                if (c == ' ') { continue; }
                if (!isdigit(c)) {
                    isNum = c == '.' ? true : false;
                    if ((c != '+') && (c != '-') && (c != '*')
                    && (c != '/') && (c != '(') && (c != ')') && (c != '.') && (c != '^')) { throw InputMismatchException(); }
                    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') { op++; }

                    //decimal
                    if (c == '.') {
                        if (decimal) { throw InputMismatchException(); }
                        else { decimal = true; }
                    }
                }
            }
            num += isNum ? 1 : 0;
        }
    }

    if (num - 1 != op) { throw InputMismatchException(); }
}

void Calculator::doCalculate(void) {
    std::istringstream iss(this->input);
    std::string buf;

    while (getline(iss, buf, ' ')) {
        if (buf.compare(".") == 0) {
            buf = "0.0";
        }
        if (buf.compare(" ") == 0) { continue; }

        //Ignore the parenthesis.
        if (buf.compare("(") == 0) {
            //std::cout << "pushing operator: " << buf[0] << std::endl;
            this->operators.push(buf[0]);
        } else if (buf.compare(")") == 0) {
            std::queue<char> tmp;
            while (this->operators.top() != '(') {
                //std::cout << "pushing operator: " << buf[0] << std::endl;
                tmp.push(this->operators.top());
                this->operators.pop();
            }
            this->operators.pop(); //'(' is useless.

            this->ManyCalculate(tmp);
           
        } else if (buf.compare("*") == 0 || buf.compare("/") == 0 || buf.compare("+") == 0 || buf.compare("-") == 0 || buf.compare("^") == 0) {
            if (this->getPriority(buf[0]) > this->getPriority(this->operators.top())) {
                //std::cout << "pushing operator: " << buf[0] << std::endl;
                this->operators.push(buf[0]);
            } else {
                while (this->getPriority(buf[0]) <= this->getPriority(this->operators.top())) {
                    this->computationPreamble();
                }

                this->operators.push(buf[0]);
            }
        } else {
            //std::cout << "pushing number: " << buf << std::endl;
            this->numbers.push(stod(buf));
        }
    }
    while (numbers.size() >= 2) {
        this->computationPreamble();
    }
    this->res = this->numbers.top();
}

int Calculator::getPriority(const char& op) {
    switch(op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '^':
            return 3;
        default:
            return 0;
    }
}

double Calculator::calculate(double num1, double num2, const char& op) {
    switch(op) {
        case '+':
            return num1 + num2;
        case '-':
            return num2 - num1;
        case '*':
            return num2 * num1;
        case '/':
            try {
                if (num1 == 0) {
                    throw DividendZeroException();
                }
                return num2 / num1;
            } catch (DividendZeroException e) {
                std::cout << e.what() << std::endl;
                abort();
            }
        case '^':
            if (num2 == num1 && num1 == 0) {
                std::cerr << "0 ^ 0 is not defined." << std::endl;
                abort();
            }
            return pow(num2, num1);
    }

}

double Calculator::ans(void) {
    return this->res;
}

#endif