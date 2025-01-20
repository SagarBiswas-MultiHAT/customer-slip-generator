#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void getInput(string& name, string& phone, string& address, double& price, double& discount) {
    cout << "\n\n\t\t===== Enter Customer Details =====\t\t \n\n";
    cout << "..:: Enter name\t\t\t: ";
    getline(cin, name);
    cout << "..:: Enter phone number\t\t: ";
    getline(cin, phone);
    cout << "..:: Enter address\t\t: ";
    getline(cin, address);
    cout << "..:: Enter price\t\t: ";
    cin >> price;
    cout << "..:: Enter discount\t\t: ";
    cin >> discount;
    cout << "\n";
}

double calculateTotalPrice(double price, double discount) {
    double discountAmount = price * discount / 100;
    return price - discountAmount;
}

void printSlip(const string& name, const string& phone, const string& address, double price, double discount, double totalPrice) {
    cout << "\n\n\t\t===== Product Slip =====" << "\n\n";
    cout << "Name\t\t\t\t: " << name << endl;
    cout << "Phone number\t\t\t: " << phone << endl;
    cout << "Address\t\t\t\t: " << address << endl;
    cout << "Price\t\t\t\t: " << price << endl;
    cout << "Discount\t\t\t: " << discount << "%" << endl;
    cout << "Price after discount\t\t: " << totalPrice << "\n\n";
}

void saveSlipToFile(const string& name, const string& phone, const string& address, double price, double discount, double totalPrice) {
    ofstream outFile("slips.txt", ios::app);
    if (outFile.is_open()) {
        outFile << "\n\t\t===== Enter Customer Details =====\t\t \n"
            << "\n\tName\t\t\t\t\t: " << name
            << "\n\tPhone Number\t\t\t: " << phone
            << "\n\tAddress\t\t\t\t\t: " << address
            << "\n\tProduct Price\t\t\t: " << price << "-/"
            << "\n\tDiscount Rate\t\t\t: " << discount << "%"
            << "\n\tPrice after discount\t: " << totalPrice << "-/" << "\n";
        outFile.close();
        cout << "\nSlip details saved to file." << endl;
    }
    else {
        cout << "\nError: Could not save slip details to file." << endl;
    }
}

int main() {
    string name, phone, address;
    double price, discount;

    getInput(name, phone, address, price, discount);
    double totalPrice = calculateTotalPrice(price, discount);
    printSlip(name, phone, address, price, discount, totalPrice);
    saveSlipToFile(name, phone, address, price, discount, totalPrice);

    return 0;
}
