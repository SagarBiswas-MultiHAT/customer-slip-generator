#pragma once

#include <string>

struct Customer {
    std::string name;
    std::string phone;
    std::string address;
    double price = 0.0;
    double discount = 0.0;
};

bool parseDouble(const std::string& text, double& value);
Customer promptCustomer();
double calculateTotalPrice(double price, double discount);
std::string formatCurrency(double value);
void printSlipHumanReadable(const Customer& customer, double total);
void appendSlipCSV(const Customer& customer, double total);
void appendSlipJsonl(const Customer& customer, double total);
void atomicAppend(const std::string& path, const std::string& content);
int runApp(int argc, char** argv);
