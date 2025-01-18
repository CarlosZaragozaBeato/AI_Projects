#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <stdexcept>
using namespace std;

// Structure to store calculation patterns
struct CalculationPattern {
    double num1;
    double num2;
    char operation;
    double result;
};

class AICalculator {
private:
    vector<CalculationPattern> history;
    map<char, double> operationWeights;
    const int MIN_PATTERNS = 3;

    // Basic operations
    double add(double a, double b) { return a + b; }
    double subtract(double a, double b) { return a - b; }
    double multiply(double a, double b) { return a * b; }
    double divide(double a, double b) {
        if (b == 0) throw runtime_error("Division by zero error!");
        return a / b;
    }

    // AI prediction methods
    double predictNextNumber(double num1, char op) {
        if (history.size() < MIN_PATTERNS) return 0.0;

        double sumWeighted = 0.0;
        double weightSum = 0.0;
        
        for (const auto& pattern : history) {
            if (pattern.operation == op) {
                // Calculate similarity weight based on first number
                double similarity = 1.0 / (1.0 + abs(pattern.num1 - num1));
                sumWeighted += pattern.num2 * similarity;
                weightSum += similarity;
            }
        }

        return weightSum > 0 ? sumWeighted / weightSum : 0.0;
    }

    void updateOperationWeights(char op, bool wasCorrect) {
        double learningRate = 0.1;
        if (wasCorrect) {
            operationWeights[op] += learningRate * (1.0 - operationWeights[op]);
        } else {
            operationWeights[op] -= learningRate * operationWeights[op];
        }
    }

public:
    AICalculator() {
        // Initialize operation weights
        operationWeights['+'] = 0.5;
        operationWeights['-'] = 0.5;
        operationWeights['*'] = 0.5;
        operationWeights['/'] = 0.5;
    }

    double calculate(double num1, double num2, char operation) {
        double result;
        switch (operation) {
            case '+': result = add(num1, num2); break;
            case '-': result = subtract(num1, num2); break;
            case '*': result = multiply(num1, num2); break;
            case '/': result = divide(num1, num2); break;
            default: throw runtime_error("Invalid operation!");
        }

        // Store the pattern
        history.push_back({num1, num2, operation, result});
        return result;
    }

    double suggestNumber(double num1, char operation) {
        if (history.size() < MIN_PATTERNS) {
            return 0.0;
        }
        return predictNextNumber(num1, operation);
    }

    void getStats() {
        cout << "\nAI Statistics:" << endl;
        cout << "Patterns learned: " << history.size() << endl;
        cout << "Operation weights:" << endl;
        for (const auto& weight : operationWeights) {
            cout << weight.first << ": " << weight.second << endl;
        }
    }
};

int main() {
    AICalculator calc;
    double num1, num2;
    char operation;
    bool continueCalculating = true;

    cout << "Welcome to AI-Enhanced Calculator!" << endl;

    while (continueCalculating) {
        // Get first number
        cout << "\nEnter first number: ";
        while (!(cin >> num1)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(10000, '\n');
        }

        // Get operation
        cout << "Enter operation (+, -, *, /): ";
        cin >> operation;

        // AI suggestion
        double suggested = calc.suggestNumber(num1, operation);
        if (suggested != 0.0) {
            cout << "AI suggests second number might be: " << suggested << endl;
        }

        // Get second number
        cout << "Enter second number: ";
        while (!(cin >> num2)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(10000, '\n');
        }

        // Perform calculation
        try {
            double result = calc.calculate(num1, num2, operation);
            cout << "Result: " << result << endl;
            calc.getStats();
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }

        // Ask if user wants to continue
        char continue_choice;
        cout << "\nDo you want to perform another calculation? (y/n): ";
        cin >> continue_choice;
        continueCalculating = (continue_choice == 'y' || continue_choice == 'Y');
    }

    cout << "\nThank you for using AI-Enhanced Calculator!" << endl;
    return 0;
}