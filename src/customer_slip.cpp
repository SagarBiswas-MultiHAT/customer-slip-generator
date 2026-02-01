#include "customer_slip.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

namespace {

    constexpr int kExitOk = 0;
    constexpr int kExitUsage = 2;
    constexpr int kExitIo = 3;
    constexpr int kExitNotFound = 4;

    std::string trim(const std::string& input) {
        std::string result = input;
        auto isSpace = [](unsigned char ch) { return std::isspace(ch) != 0; };
        result.erase(result.begin(), std::find_if(result.begin(), result.end(), [&](unsigned char ch) { return !isSpace(ch); }));
        result.erase(std::find_if(result.rbegin(), result.rend(), [&](unsigned char ch) { return !isSpace(ch); }).base(), result.end());
        return result;
    }

    std::string toLower(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        return value;
    }

    bool containsInsensitive(const std::string& haystack, const std::string& needle) {
        return toLower(haystack).find(toLower(needle)) != std::string::npos;
    }

    std::string formatFixed(double value) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(2) << value;
        return out.str();
    }

    std::string escapeCsv(const std::string& value) {
        bool needsQuotes = value.find_first_of(",\"\n\r") != std::string::npos;
        if (!needsQuotes) {
            return value;
        }
        std::string escaped;
        escaped.reserve(value.size() + 2);
        escaped.push_back('"');
        for (char ch : value) {
            if (ch == '"') {
                escaped.push_back('"');
            }
            escaped.push_back(ch);
        }
        escaped.push_back('"');
        return escaped;
    }

    std::string escapeJson(const std::string& value) {
        std::ostringstream out;
        for (unsigned char ch : value) {
            switch (ch) {
            case '\\': out << "\\\\"; break;
            case '"': out << "\\\""; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if (ch < 0x20) {
                    out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(ch) << std::dec;
                }
                else {
                    out << static_cast<char>(ch);
                }
                break;
            }
        }
        return out.str();
    }

    std::string timestampUtc() {
        const auto now = std::chrono::system_clock::now();
        const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm utcTime{};
#ifdef _WIN32
        gmtime_s(&utcTime, &nowTime);
#else
        gmtime_r(&nowTime, &utcTime);
#endif
        std::ostringstream out;
        out << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
        return out.str();
    }

    std::vector<std::string> parseCsvLine(const std::string& line) {
        std::vector<std::string> fields;
        std::string current;
        bool inQuotes = false;

        for (size_t i = 0; i < line.size(); ++i) {
            char ch = line[i];
            if (inQuotes) {
                if (ch == '"') {
                    if (i + 1 < line.size() && line[i + 1] == '"') {
                        current.push_back('"');
                        ++i;
                    }
                    else {
                        inQuotes = false;
                    }
                }
                else {
                    current.push_back(ch);
                }
            }
            else {
                if (ch == '"') {
                    inQuotes = true;
                }
                else if (ch == ',') {
                    fields.push_back(current);
                    current.clear();
                }
                else {
                    current.push_back(ch);
                }
            }
        }
        fields.push_back(current);
        return fields;
    }

    void printUsage(std::ostream& out) {
        out << "Usage:\n"
            << "  customer_slip               Run interactive mode\n"
            << "  customer_slip --view NAME   Search slips by name (case-insensitive)\n"
            << "  customer_slip --export-json Export CSV as pretty JSON array\n"
            << "  customer_slip --help        Show this help message\n";
    }

    int viewByName(const std::string& query) {
        const std::filesystem::path csvPath = std::filesystem::path("data") / "slips.csv";
        std::ifstream in(csvPath);
        if (!in.is_open()) {
            std::cerr << "No slips found. Expected file: " << csvPath.string() << "\n";
            return kExitNotFound;
        }

        std::string line;
        if (!std::getline(in, line)) {
            std::cout << "No records available.\n";
            return kExitOk;
        }

        std::vector<std::vector<std::string>> matches;
        while (std::getline(in, line)) {
            if (line.empty()) {
                continue;
            }
            auto fields = parseCsvLine(line);
            if (fields.empty()) {
                continue;
            }
            const std::string& nameField = fields[0];
            if (containsInsensitive(nameField, query)) {
                matches.push_back(fields);
            }
        }

        if (matches.empty()) {
            std::cout << "No matches found for: " << query << "\n";
            return kExitOk;
        }

        const std::vector<std::string> headers = {
            "Name", "Phone", "Address", "Price", "Discount", "Total", "Timestamp"
        };
        std::vector<size_t> widths(headers.size(), 0);
        for (size_t i = 0; i < headers.size(); ++i) {
            widths[i] = headers[i].size();
        }
        for (const auto& row : matches) {
            for (size_t i = 0; i < widths.size(); ++i) {
                if (i < row.size()) {
                    widths[i] = std::max(widths[i], row[i].size());
                }
            }
        }

        auto printRow = [&](const std::vector<std::string>& row) {
            for (size_t i = 0; i < widths.size(); ++i) {
                std::string value = (i < row.size()) ? row[i] : "";
                std::cout << std::left << std::setw(static_cast<int>(widths[i] + 2)) << value;
            }
            std::cout << "\n";
            };

        printRow(headers);
        std::cout << std::string(std::accumulate(widths.begin(), widths.end(), static_cast<size_t>(0)) + widths.size() * 2, '-') << "\n";
        for (const auto& row : matches) {
            printRow(row);
        }

        return kExitOk;
    }

    int exportJson() {
        const std::filesystem::path csvPath = std::filesystem::path("data") / "slips.csv";
        std::ifstream in(csvPath);
        if (!in.is_open()) {
            std::cerr << "No slips found. Expected file: " << csvPath.string() << "\n";
            return kExitNotFound;
        }

        std::string line;
        if (!std::getline(in, line)) {
            std::cout << "[]\n";
            return kExitOk;
        }

        std::vector<std::vector<std::string>> records;
        while (std::getline(in, line)) {
            if (line.empty()) {
                continue;
            }
            records.push_back(parseCsvLine(line));
        }

        std::cout << "[\n";
        for (size_t i = 0; i < records.size(); ++i) {
            const auto& row = records[i];
            std::cout << "  {\n";
            std::cout << "    \"name\": \"" << escapeJson(row.size() > 0 ? row[0] : "") << "\",\n";
            std::cout << "    \"phone\": \"" << escapeJson(row.size() > 1 ? row[1] : "") << "\",\n";
            std::cout << "    \"address\": \"" << escapeJson(row.size() > 2 ? row[2] : "") << "\",\n";
            std::cout << "    \"price\": " << (row.size() > 3 ? row[3] : "0.00") << ",\n";
            std::cout << "    \"discount\": " << (row.size() > 4 ? row[4] : "0.00") << ",\n";
            std::cout << "    \"total\": " << (row.size() > 5 ? row[5] : "0.00") << ",\n";
            std::cout << "    \"timestamp\": \"" << escapeJson(row.size() > 6 ? row[6] : "") << "\"\n";
            std::cout << "  }" << (i + 1 == records.size() ? "\n" : ",\n");
        }
        std::cout << "]\n";

        return kExitOk;
    }

} // namespace

bool parseDouble(const std::string& text, double& value) {
    const std::string trimmed = trim(text);
    if (trimmed.empty()) {
        return false;
    }
    try {
        size_t index = 0;
        value = std::stod(trimmed, &index);
        if (index != trimmed.size() || !std::isfinite(value)) {
            return false;
        }
    }
    catch (const std::exception&) {
        return false;
    }
    return true;
}

Customer promptCustomer() {
    Customer customer;
    std::string input;

    auto promptText = [&](const std::string& label, std::string& target) {
        while (true) {
            std::cout << label;
            std::getline(std::cin, input);
            input = trim(input);
            if (!input.empty()) {
                target = input;
                return;
            }
            std::cout << "Please enter a non-empty value.\n";
        }
        };

    std::cout << "\n===== Enter Customer Details =====\n";
    promptText("Name: ", customer.name);
    promptText("Phone: ", customer.phone);
    promptText("Address: ", customer.address);

    while (true) {
        std::cout << "Price: ";
        std::getline(std::cin, input);
        double value = 0.0;
        if (parseDouble(input, value) && value >= 0.0) {
            customer.price = value;
            break;
        }
        std::cout << "Please enter a valid non-negative price.\n";
    }

    while (true) {
        std::cout << "Discount (%): ";
        std::getline(std::cin, input);
        double value = 0.0;
        if (parseDouble(input, value) && value >= 0.0 && value <= 100.0) {
            customer.discount = value;
            break;
        }
        std::cout << "Please enter a discount between 0 and 100.\n";
    }

    return customer;
}

double calculateTotalPrice(double price, double discount) {
    return price - (price * discount / 100.0);
}

std::string formatCurrency(double value) {
    std::ostringstream out;
    out << "$" << std::fixed << std::setprecision(2) << value;
    return out.str();
}

void printSlipHumanReadable(const Customer& customer, double total) {
    std::cout << "\n===== Product Slip =====\n";
    std::cout << "Name:     " << customer.name << "\n";
    std::cout << "Phone:    " << customer.phone << "\n";
    std::cout << "Address:  " << customer.address << "\n";
    std::cout << "Price:    " << formatCurrency(customer.price) << "\n";
    std::cout << "Discount: " << formatFixed(customer.discount) << "%\n";
    std::cout << "Total:    " << formatCurrency(total) << "\n\n";
}

void atomicAppend(const std::string& path, const std::string& content) {
    const std::filesystem::path targetPath(path);
    std::filesystem::create_directories(targetPath.parent_path());

    std::string existing;
    {
        std::ifstream in(targetPath, std::ios::binary);
        if (in) {
            std::ostringstream buffer;
            buffer << in.rdbuf();
            existing = buffer.str();
        }
    }

    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto stamp = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
    const std::filesystem::path tempPath = targetPath.string() + ".tmp." + std::to_string(stamp);

    {
        std::ofstream out(tempPath, std::ios::binary | std::ios::trunc);
        if (!out.is_open()) {
            throw std::runtime_error("Failed to open temporary file for writing.");
        }
        out << existing;
        out << content;
        out.flush();
        if (!out) {
            throw std::runtime_error("Failed to write slip data.");
        }
    }

    std::error_code ec;
    std::filesystem::rename(tempPath, targetPath, ec);
    if (ec) {
        if (std::filesystem::exists(targetPath)) {
            std::filesystem::remove(targetPath, ec);
        }
        ec.clear();
        std::filesystem::rename(tempPath, targetPath, ec);
        if (ec) {
            throw std::runtime_error("Failed to finalize slip file update.");
        }
    }
}

void appendSlipCSV(const Customer& customer, double total) {
    const std::filesystem::path csvPath = std::filesystem::path("data") / "slips.csv";
    std::error_code ec;
    const bool fileExists = std::filesystem::exists(csvPath, ec) && std::filesystem::file_size(csvPath, ec) > 0;
    const std::string timestamp = timestampUtc();

    std::ostringstream line;
    if (!fileExists) {
        line << "name,phone,address,price,discount,total,timestamp\n";
    }
    line << escapeCsv(customer.name) << ','
        << escapeCsv(customer.phone) << ','
        << escapeCsv(customer.address) << ','
        << formatFixed(customer.price) << ','
        << formatFixed(customer.discount) << ','
        << formatFixed(total) << ','
        << escapeCsv(timestamp) << "\n";

    atomicAppend(csvPath.string(), line.str());
}

void appendSlipJsonl(const Customer& customer, double total) {
    const std::filesystem::path jsonlPath = std::filesystem::path("data") / "slips.jsonl";
    const std::string timestamp = timestampUtc();

    std::ostringstream line;
    line << "{"
        << "\"name\":\"" << escapeJson(customer.name) << "\","
        << "\"phone\":\"" << escapeJson(customer.phone) << "\","
        << "\"address\":\"" << escapeJson(customer.address) << "\","
        << "\"price\":" << formatFixed(customer.price) << ','
        << "\"discount\":" << formatFixed(customer.discount) << ','
        << "\"total\":" << formatFixed(total) << ','
        << "\"timestamp\":\"" << escapeJson(timestamp) << "\""
        << "}\n";

    atomicAppend(jsonlPath.string(), line.str());
}

int runApp(int argc, char** argv) {
    if (argc > 1) {
        const std::string arg1 = argv[1];
        if (arg1 == "--help" || arg1 == "-h") {
            printUsage(std::cout);
            return kExitOk;
        }
        if (arg1 == "--view") {
            if (argc < 3) {
                std::cerr << "Missing name for --view.\n";
                printUsage(std::cerr);
                return kExitUsage;
            }
            return viewByName(argv[2]);
        }
        if (arg1 == "--export-json") {
            return exportJson();
        }
        std::cerr << "Unknown option: " << arg1 << "\n";
        printUsage(std::cerr);
        return kExitUsage;
    }

    try {
        Customer customer = promptCustomer();
        const double total = calculateTotalPrice(customer.price, customer.discount);
        printSlipHumanReadable(customer, total);
        appendSlipCSV(customer, total);
        appendSlipJsonl(customer, total);
        std::cout << "Slip saved to data/slips.csv and data/slips.jsonl.\n\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return kExitIo;
    }

    return kExitOk;
}

#ifndef CUSTOMER_SLIP_TEST
int main(int argc, char** argv) {
    return runApp(argc, argv);
}
#endif
