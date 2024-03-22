#include <complex>
#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <gmpxx.h>
#include <unordered_map>
#include <stack>
#include <queue>//运用到C++的栈和队列
using namespace std;
#define MAXSIZE 1024

// 修改 evaluateRPN 函数，使用复数类进行运算
std::complex<mpf_class> evaluateRPN(const std::vector<std::string>& rpnExpr) {
    std::stack<std::complex<mpf_class>> operands;

    for (const std::string& token : rpnExpr) {
        if (isdigit(token[0]) || token[0] == '.') {
            mpf_class realPart;
            realPart.set_str(token, 10);
            operands.push({ realPart, 0 });
        } else if (token == "i") {
            operands.push({ 0, 1 });
        } else if (isOperator(token[0])) {
            std::complex<mpf_class> op2 = operands.top();
            operands.pop();
            std::complex<mpf_class> op1 = operands.top();
            operands.pop();
            if (token == "+") {
                operands.push(op1 + op2);
            } else if (token == "-") {
                operands.push(op1 - op2);
            } else if (token == "*") {
                operands.push(op1 * op2);
            } else if (token == "/") {
                operands.push(op1 / op2);
            }
        }
    }

    return operands.top();
}

int main() {
    std::string expression = "3.14159 + 2.71828i";  // 示例输入为一个复数

    std::vector<std::string> rpnExpr = toRPN(expression);

    for(const std::string &s : rpnExpr) {
        std::cout << s << " ";
    }
    std::cout << std::endl;

    std::complex<mpf_class> result = evaluateRPN(rpnExpr);

    std::cout << "Result: " << result << std::endl;

    return 0;
}