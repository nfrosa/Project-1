using namespace std;
#include <iostream> 
#include <string> 
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <math.h>

unordered_map<string,double> variables;
bool error = 0;

int priority(char n) {
	int currentRank = 0;
	if (n == '-' || n == '+')
		currentRank = 1;
	else if (n == '/' || n == '*')
		currentRank = 2;
	else if (n == '^')
		currentRank = 3;
	return currentRank;
}

queue<string> stringToQueue(string Function) {
	queue<string> functionQueue;
	int length = Function.length();
	for (int i = 0; i < length; i++) {
		string number;
		string word;
		if (!isdigit(Function.at(i)) && Function.at(i) != ' ' && !isalpha(Function.at(i))) {
			string toPush(1, Function.at(i));
			functionQueue.push(toPush);
		}
		else if (isalpha(Function.at(i))) {
				if (Function.substr(i, 3) == "sin") {
					functionQueue.push("sin");
					i += 2;
				} else if (Function.substr(i, 3) == "cos") {
					functionQueue.push("cos");
					i += 2;
				} else if (Function.substr(i, 3) == "log") {
					functionQueue.push("log");
					i += 2;
				} else {
					//get word
					string variableName;
					while (i<length && isalpha(Function.at(i)) ) {
						variableName.push_back(Function.at(i));
						i++;
					}
					i--;
					if (variables.find(variableName) == variables.end()) {
						cout << "Undeclared-Variable" << endl;
						error = 1;
						return functionQueue;
					} else {
						functionQueue.push(to_string(variables.at(variableName)));
					}
				}
		} else if (isdigit(Function.at(i))) {
			while (i < length && (isdigit(Function.at(i)) || Function.at(i) == '.')) {
				number.push_back(Function.at(i));
				if (i + 1 < length && (isdigit(Function.at(i + 1)) || Function.at(i+1) == '.'))
					i++;
				else
					break;
			}
			functionQueue.push(number);
		}
	}
	return functionQueue;
}
//stringtoqueue is O(n)

double solve(queue<string> Function) {
	//to use variables just check your map for them. if they aren't found you can return the error
	double solution = 0;
	int originalSize = Function.size();
	queue<string> postfix;
	int rank = 0;
	stack<string> ops;
	for (int i = 0; i < originalSize; i++) {
		//getting function into postfix 
		string curr = Function.front();
		Function.pop();
		if (isalpha(curr.at(0))) {
			//getting function
			string funct = curr;
			curr = Function.front();
			Function.pop();
			i++;
			queue<string> subQueue;
			int leftParenthesesCount = 1;
			int rightParenthesesCount = 0;
			while (rightParenthesesCount != leftParenthesesCount) {
				curr = Function.front();
				Function.pop();
				if (curr.at(0) == '(')
					leftParenthesesCount++;
				if (curr.at(0) == ')')
					rightParenthesesCount++;
				if (rightParenthesesCount != leftParenthesesCount)
					subQueue.push(curr);
				i++;
			}
			if (funct == "sin") {
				postfix.push(to_string(sin(solve(subQueue))));
			} else if (funct == "cos") {
				postfix.push(to_string(cos(solve(subQueue))));
			} else if (funct == "log") {
				postfix.push(to_string(log(solve(subQueue))));
			}
		} else if(curr.at(0) == '(') {
			//make a queue for everything from left parentheses to right parentheses, solve that. 
			queue<string> subQueue;
			int leftParenthesesCount = 1;
			int rightParenthesesCount = 0;
			while (rightParenthesesCount != leftParenthesesCount) {
				curr = Function.front();
				Function.pop();
				if (curr.at(0) == '(')
					leftParenthesesCount++;
				if (curr.at(0) == ')')
					rightParenthesesCount++;
				if (rightParenthesesCount != leftParenthesesCount)
					subQueue.push(curr);
				i++;
			}
				postfix.push(to_string(solve(subQueue)));
		} else if (isdigit(curr.at(0))) {
			//parses number
			postfix.push(curr);
		} else if (curr.at(0) == '-' && postfix.empty()) {
			//getting negative numbers
			curr = Function.front();
			double dblcurr = stod(curr);
			dblcurr *= -1;
			Function.pop();
			i++;
			postfix.push(to_string(dblcurr));
		} else {
			int currentRank = 0;
			//operations
			currentRank = priority(curr.at(0));
			if (ops.empty()) {
				ops.push(curr);
				rank = currentRank;
			} else {
				if (currentRank > rank) {
					//get the next number and then place the operation after it
					//string nextnum = Function.front();
					//Function.pop();
					//i++;
					ops.push(curr);
					rank = currentRank;
				} else {
					//place the operation
					while (currentRank <= rank && !ops.empty()) {
						postfix.push(ops.top());
						ops.pop();
						if (!ops.empty())
							rank = priority(ops.top().at(0));
						else
							rank = 0;
					}
					ops.push(curr);
					rank = currentRank;
				}
			}
		}
	}
	while (!ops.empty()) {
		//places the last operation if it hasnt been done yet
		postfix.push(ops.top());
		ops.pop();
	}
	stack<double> nums;
	//solving the postfix expression
	originalSize = postfix.size();
	for (int i = 0; i < originalSize; i++) {
		string curr = postfix.front();
		postfix.pop();
		if (isdigit(curr.at(0))) {
			nums.push(stod(curr));
		} else if (curr.at(0) == '-' && curr.length() != 1) {
			nums.push(stod(curr));
		} else {
			double number = nums.top();
			nums.pop();
			double number2 = nums.top();
			nums.pop();
			if (curr.at(0) == '-')
				nums.push(number2 - number);
			else if (curr.at(0) == '+')
				nums.push(number2 + number);
			else if (curr.at(0) == '/') {
				if (number == 0) {
					cout << "Division-By-Zero" << endl;
					error = 1;
					break;
				} else
					nums.push(number2 / number);
			} else if (curr.at(0) == '*')
				nums.push(number2 * number);
			else if (curr.at(0) == '^') {
				nums.push(pow(number2, number));
			}
		}
		solution = nums.top();
	}
	return solution;
}
int main()
{
	variables["PI"] = 3.14169;
	variables["e"] = 2.718;
	string input;
	string varName;
	string varValue;
	double varVal;
	queue<string> varValueQ;
	bool exitloop = 0;
	while (!exitloop) {
		cin.clear();
		getline(cin, input);
		if (input.find("let") != input.npos) {
			//get the next line and store the variable
			int variableStart= input.find("let") + 4;
			while (!isalpha(input.at(variableStart))) {
				variableStart++;
			}
			int i = variableStart;
			int variableLength = 0;
			while (isalpha(input.at(i))) {
				variableLength++;
				i++;
			}
			int valueStart = input.find('=') + 1;
			varName = input.substr(variableStart, variableLength);
			varValue = input.substr(valueStart);
			varValueQ = stringToQueue(varValue);
			if (error == 1) {
				error = 0;
				continue;
			} else {
				varVal = solve(varValueQ);
			}
			if (!varName.empty() && variables.find(varName) == variables.end()) {
				variables[varName]= varVal;
			} else if (variables.find(varName) != variables.end()) {
				variables[varName] = varVal;
			}

		}
		else if (input == "quit"){
			return 0;
	    } else {
			//solving the variable if it isn't already
			double solution;
			queue<string> Function = stringToQueue(input);
			if (error == 1) {
				error = 0;
				continue;
			} else {
				solution = solve(Function);
			}
			if (error == 1) {
				error = 0;
			}
			else {
				cout << solution << endl;
			}
		}
	}
    return 0;
}

