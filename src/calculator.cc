#include "../include/calculator.h"

 namespace calculator{
  // Function to split the input string into tokens
  std::vector<std::string> tokenize(const std::string& str) {
      std::istringstream iss(str);
      std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                      std::istream_iterator<std::string>{}};
      return tokens;
  }

  // Function to convert infix expression to postfix (RPN) using the Shunting Yard algorithm
  std::vector<std::string> infixToPostfix(const std::vector<std::string>& tokens) {
      std::stack<std::string> stack;
      std::vector<std::string> output;
      std::map<std::string, int> precedence{
          {"+", 1}, {"-", 1},
          {"*", 2}, {"/", 2}
      };

      for (const auto& token : tokens) {
          if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) { // Number
              output.push_back(token);
          } else if (token == "(") {
              stack.push(token);
          } else if (token == ")") {
              while (!stack.empty() && stack.top() != "(") {
                  output.push_back(stack.top());
                  stack.pop();
              }
              stack.pop(); // Pop the "("
          } else { // Operator
              while (!stack.empty() && precedence[stack.top()] >= precedence[token]) {
                  output.push_back(stack.top());
                  stack.pop();
              }
              stack.push(token);
          }
      }

      while (!stack.empty()) {
          output.push_back(stack.top());
          stack.pop();
      }

      return output;
  }

  // Function to evaluate a postfix (RPN) expression
  double evaluatePostfix(const std::vector<std::string>& postfix) {
      std::stack<double> stack;

      for (const auto& token : postfix) {
          if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) { // Number
              stack.push(std::stod(token));
          } else { // Operator
              double op2 = stack.top(); stack.pop();
              double op1 = stack.top(); stack.pop();

              if (token == "+") stack.push(op1 + op2);
              else if (token == "-") stack.push(op1 - op2);
              else if (token == "*") stack.push(op1 * op2);
              else if (token == "/") stack.push(op1 / op2);
          }
      }

      return stack.top();
  }

  double calculate_from_string(const string &expression){
    string processed_expression = expression;
    replace(processed_expression.begin(), processed_expression.end(), 'x', '*');
    auto tokens = tokenize(processed_expression);
    auto postfix = infixToPostfix(tokens);
    double result = evaluatePostfix(postfix);
    return result;
  }
}