#include "DataHandler.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

// Parses one CSV row supporting quoted fields and escaped quotes.
std::vector<std::string> parseCsvRow(const std::string& line)
{
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (std::size_t index = 0; index < line.size(); ++index) {
        const char character = line[index];

        if (character == '"') {
            if (inQuotes && index + 1 < line.size() && line[index + 1] == '"') {
                field.push_back('"');
                ++index;
            } else {
                inQuotes = !inQuotes;
            }
            continue;
        }

        if (character == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
            continue;
        }

        field.push_back(character);
    }

    if (inQuotes) {
        throw std::runtime_error("Malformed CSV row: unmatched quote");
    }

    fields.push_back(field);
    return fields;
}

// Escapes a field so it can be written safely to CSV.
std::string escapeCsvField(const std::string& value)
{
    const bool requiresQuotes = value.find_first_of(",\"\n\r") != std::string::npos;
    if (!requiresQuotes) {
        return value;
    }

    std::string escaped;
    escaped.reserve(value.size() + 2);
    escaped.push_back('"');

    for (const char character : value) {
        if (character == '"') {
            escaped.push_back('"');
        }
        escaped.push_back(character);
    }

    escaped.push_back('"');
    return escaped;
}

// Converts a parsed CSV row into the domain struct expected by the manager.
int parseIntegerField(const std::string& value, const std::string& fieldName, std::size_t lineNumber)
{
    try {
        std::size_t parsedCharacters = 0;
        const int parsedValue = std::stoi(value, &parsedCharacters);
        if (parsedCharacters != value.size()) {
            throw std::runtime_error("");
        }

        return parsedValue;
    } catch (const std::exception&) {
        throw std::runtime_error(
            "Malformed CSV row at line " + std::to_string(lineNumber) +
            ": invalid integer value for " + fieldName);
    }
}

double parseAmountField(const std::string& value, std::size_t lineNumber)
{
    try {
        std::size_t parsedCharacters = 0;
        const double parsedValue = std::stod(value, &parsedCharacters);
        if (parsedCharacters != value.size()) {
            throw std::runtime_error("");
        }

        return parsedValue;
    } catch (const std::exception&) {
        throw std::runtime_error(
            "Malformed CSV row at line " + std::to_string(lineNumber) +
            ": invalid numeric value for amount");
    }
}

Expense parseExpense(const std::vector<std::string>& fields, std::size_t lineNumber)
{
    if (fields.size() != 5) {
        throw std::runtime_error(
            "Malformed CSV row at line " + std::to_string(lineNumber) +
            ": expected 5 fields");
    }

    Expense expense{};
    expense.id = parseIntegerField(fields[0], "id", lineNumber);
    expense.amount = parseAmountField(fields[1], lineNumber);
    expense.category = fields[2];
    expense.date = fields[3];
    expense.description = fields[4];

    if (expense.id <= 0) {
        throw std::runtime_error(
            "Malformed CSV row at line " + std::to_string(lineNumber) +
            ": id must be a positive integer");
    }

    if (expense.amount < 0.0) {
        throw std::runtime_error(
            "Malformed CSV row at line " + std::to_string(lineNumber) +
            ": amount cannot be negative");
    }

    if (expense.category.empty()) {
        throw std::runtime_error(
            "Malformed CSV row at line " + std::to_string(lineNumber) +
            ": category cannot be empty");
    }

    if (expense.date.empty()) {
        throw std::runtime_error(
            "Malformed CSV row at line " + std::to_string(lineNumber) +
            ": date cannot be empty");
    }

    return expense;
}

} // namespace

DataHandler::DataHandler(std::filesystem::path filePath)
    : filePath_(std::move(filePath))
{
}

std::vector<Expense> DataHandler::loadExpenses() const
{
    // Missing file is treated as "no persisted data yet", not as an error.
    if (!std::filesystem::exists(filePath_)) {
        return {};
    }

    std::ifstream input(filePath_);
    if (!input) {
        throw std::runtime_error("Unable to open expense data file for reading: " + filePath_.string());
    }

    std::string line;
    if (!std::getline(input, line)) {
        return {};
    }

    if (line != "id,amount,category,date,description") {
        throw std::runtime_error(
            "Unexpected CSV header in expense data file. Expected: id,amount,category,date,description");
    }

    std::vector<Expense> expenses;
    std::size_t lineNumber = 1;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (line.empty()) {
            continue;
        }

        expenses.push_back(parseExpense(parseCsvRow(line), lineNumber));
    }

    return expenses;
}

void DataHandler::saveExpenses(const std::vector<Expense>& expenses) const
{
    // Allow callers to save into a path whose parent directory does not exist.
    if (!filePath_.parent_path().empty()) {
        std::filesystem::create_directories(filePath_.parent_path());
    }

    std::ofstream output(filePath_, std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Unable to open expense data file for writing: " + filePath_.string());
    }

    output << "id,amount,category,date,description\n";
    for (const Expense& expense : expenses) {
        output << expense.id << ','
               << expense.amount << ','
               << escapeCsvField(expense.category) << ','
               << escapeCsvField(expense.date) << ','
               << escapeCsvField(expense.description) << '\n';
    }
}

const std::filesystem::path& DataHandler::getFilePath() const noexcept
{
    return filePath_;
}
