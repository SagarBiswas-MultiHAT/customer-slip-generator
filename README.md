
# Customer Slip Generator

A C++ application designed to capture customer details, calculate discounted prices, and generate a product slip. The generated slip is printed to the console and saved to a file for record-keeping.

## Features
- Captures customer details including name, phone number, address, price, and discount.
- Calculates the total price after applying the discount.
- Prints a detailed product slip.
- Saves the product slip details to a file (`slips.txt`).

## How to Use
1. **Clone the repository** or download the source code.
2. **Compile the code** using a C++ compiler (e.g., `g++`).
   ```bash
   g++ -o customer_slip customer_slip.cpp
   ```
3. **Run the compiled program**.
   ```bash
   ./customer_slip
   ```
4. Follow the prompts to enter customer details and view the generated slip.

## Script Overview
```cpp
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void getInput(string& name, string& phone, string& address, double& price, double& discount);
double calculateTotalPrice(double price, double discount);
void printSlip(const string& name, const string& phone, const string& address, double price, double discount, double totalPrice);
void saveSlipToFile(const string& name, const string& phone, const string& address, double price, double discount, double totalPrice);

int main() {
    string name, phone, address;
    double price, discount;

    getInput(name, phone, address, price, discount);
    double totalPrice = calculateTotalPrice(price, discount);
    printSlip(name, phone, address, price, discount, totalPrice);
    saveSlipToFile(name, phone, address, price, discount, totalPrice);

    return 0;
}
```

### Functions Breakdown
- `getInput()`: Captures customer details from the user.
- `calculateTotalPrice()`: Computes the final price after applying the discount.
- `printSlip()`: Displays the customer details and computed total price.
- `saveSlipToFile()`: Writes the slip details to `slips.txt` for record-keeping.

## File Output
The details are saved to a file named `slips.txt` in the following format:
```
===== Enter Customer Details =====
Name                        : [Customer Name]
Phone Number                : [Phone Number]
Address                     : [Address]
Product Price               : [Price]-/
Discount Rate               : [Discount]%
Price after discount        : [Total Price]-/
```



## Author
[SagarBiswas-MultiHAT](https://github.com/SagarBiswas-MultiHAT)
```

This `README.md` provides a comprehensive overview of your C++ application, offering instructions for compilation, usage, and an explanation of its functionality. Let me know if you need any further modifications!
