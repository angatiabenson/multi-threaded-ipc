#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <iostream>
#include <sstream>
#include <algorithm>
#include <stack>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <iterator>

using namespace std;

namespace calculator
{
// Function to split the input string into tokens
std::vector<std::string> tokenize(const std::string& str);

// Function to convert infix expression to postfix (RPN) using the Shunting Yard algorithm
std::vector<std::string> infixToPostfix(const std::vector<std::string>& tokens);

// Function to evaluate a postfix (RPN) expression
double evaluatePostfix(const std::vector<std::string>& postfix);

// Calculates the result of operations specified in a string.
double calculate_from_string(const string &input);
}
#endif