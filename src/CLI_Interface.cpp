#include "CLI_Interface.hpp"

#include <algorithm>
#include <charconv>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>

CLI_Interface::CLI_Interface(ExpenseManager& expenseManager)
    : expenseManager_(expenseManager)
{
}

int CLI_Interface::run(const std::vector<std::string_view>& arguments)
{
    // No command means "show usage" rather than hard failure.
    if (arguments.empty()) {
        printHelp();
        return 0;
    }

    const std::string_view command = arguments.front();
    if (command == "help" || command == "--help" || command == "-h") {
        printHelp();
        return 0;
    }

    if (command == "add") {
        return handleAddCommand(arguments);
    }

    if (command == "list") {
        return handleListCommand(arguments);
    }

    if (command == "stats") {
        return handleStatsCommand(arguments);
    }

    if (command == "delete") {
        return handleDeleteCommand(arguments);
    }

    std::cerr << "Unknown command: " << command << '\n';
    printHelp();
    return 1;
}

void CLI_Interface::printHelp() const
{
    std::cout
        << "Usage: expense <command> [options]\n"
        << "Commands:\n"
        << "  add     Add a new expense\n"
        << "          --amount <value> --cat <category> --date <YYYY-MM-DD> --description <text>\n"
        << "  list    List all expenses\n"
        << "          [--cat <category>] [--from <YYYY-MM-DD> --to <YYYY-MM-DD>]\n"
        << "  stats   Show totals, averages and monthly summary\n"
        << "          [--cat <category>] [--from <YYYY-MM-DD> --to <YYYY-MM-DD>]\n"
        << "  delete  Delete an expense by ID\n"
        << "          --id <number>\n"
        << "Examples:\n"
        << "  expense add --amount 15.50 --cat Cibo --date 2026-04-02 --description \"Pizza\"\n"
        << "  expense list --cat Cibo\n"
        << "  expense stats --from 2026-04-01 --to 2026-04-30\n"
        << "  expense delete --id 3\n";
}

int CLI_Interface::handleAddCommand(const std::vector<std::string_view>& arguments)
{
    const auto options = parseOptions(arguments);

    const auto amountIt = options.find("amount");
    const auto categoryIt = options.find("cat");
    const auto dateIt = options.find("date");
    const auto descriptionIt = options.find("description");

    if (amountIt == options.end() || categoryIt == options.end() || dateIt == options.end() || descriptionIt == options.end()) {
        std::cerr << "Missing required options for add. Expected --amount, --cat, --date and --description.\n";
        printCommandUsage("add");
        return 1;
    }

    if (!validateAllowedOptions(options, {"amount", "cat", "date", "description"}, "add")) {
        return 1;
    }

    const std::optional<double> amount = parseAmount(amountIt->second);
    if (!amount.has_value() || *amount <= 0.0) {
        std::cerr << "Invalid amount. Expected a positive number.\n";
        printCommandUsage("add");
        return 1;
    }

    if (categoryIt->second.empty()) {
        std::cerr << "Invalid category. Expected a non-empty value.\n";
        printCommandUsage("add");
        return 1;
    }

    if (!isValidDate(dateIt->second)) {
        std::cerr << "Invalid date. Expected format YYYY-MM-DD.\n";
        printCommandUsage("add");
        return 1;
    }

    Expense expense = expenseManager_.addExpense(*amount, categoryIt->second, dateIt->second, descriptionIt->second);
    std::cout << "Added expense with ID " << expense.id << ".\n";
    return 0;
}

int CLI_Interface::handleListCommand(const std::vector<std::string_view>& arguments) const
{
    const auto options = parseOptions(arguments);
    if (!validateAllowedOptions(options, {"cat", "from", "to"}, "list")) {
        return 1;
    }

    const std::vector<Expense> expenses = collectFilteredExpenses(options);
    printExpensesTable(expenses);
    return 0;
}

int CLI_Interface::handleStatsCommand(const std::vector<std::string_view>& arguments) const
{
    const auto options = parseOptions(arguments);
    if (!validateAllowedOptions(options, {"cat", "from", "to"}, "stats")) {
        return 1;
    }

    double total = 0.0;
    double average = 0.0;
    std::map<std::string, double> totalsByCategory;
    std::map<std::string, double> monthlyTotals;
    if (options.empty()) {
        total = expenseManager_.getTotalAmount();
        average = expenseManager_.getAverageAmount();
        totalsByCategory = expenseManager_.getTotalAmountByCategory();
        monthlyTotals = expenseManager_.getMonthlyTotals();
    } else {
        const std::vector<Expense> expenses = collectFilteredExpenses(options);
        for (const Expense& expense : expenses) {
            total += expense.amount;
            totalsByCategory[expense.category] += expense.amount;
            if (expense.date.size() >= 7) {
                monthlyTotals[expense.date.substr(0, 7)] += expense.amount;
            }
        }

        average = expenses.empty() ? 0.0 : total / static_cast<double>(expenses.size());
    }

    std::cout << std::fixed << std::setprecision(2)
              << "Total spent: " << total << '\n'
              << "Average expense: " << average << '\n';

    if (totalsByCategory.empty()) {
        return 0;
    }

    std::cout << "By category:\n";
    for (const auto& [category, categoryTotal] : totalsByCategory) {
        std::cout << "  " << category << ": " << categoryTotal << '\n';
    }

    std::cout << "By month:\n";
    for (const auto& [month, monthTotal] : monthlyTotals) {
        std::cout << "  " << month << ": " << monthTotal << '\n';
    }

    return 0;
}

int CLI_Interface::handleDeleteCommand(const std::vector<std::string_view>& arguments)
{
    const auto options = parseOptions(arguments);
    if (!validateAllowedOptions(options, {"id"}, "delete")) {
        return 1;
    }

    const auto idIt = options.find("id");
    if (idIt == options.end()) {
        std::cerr << "Missing required option --id for delete.\n";
        printCommandUsage("delete");
        return 1;
    }

    const std::optional<int> id = parseId(idIt->second);
    if (!id.has_value() || *id <= 0) {
        std::cerr << "Invalid ID. Expected a positive integer.\n";
        printCommandUsage("delete");
        return 1;
    }

    if (!expenseManager_.deleteExpenseById(*id)) {
        std::cerr << "Expense with ID " << *id << " not found.\n";
        return 1;
    }

    std::cout << "Deleted expense with ID " << *id << ".\n";
    return 0;
}

std::optional<double> CLI_Interface::parseAmount(std::string_view value) const
{
    std::stringstream stream;
    stream << value;

    double amount = 0.0;
    char trailing = '\0';
    if (!(stream >> amount) || (stream >> trailing)) {
        return std::nullopt;
    }

    return amount;
}

std::optional<int> CLI_Interface::parseId(std::string_view value) const
{
    int parsedId = 0;
    const char* begin = value.data();
    const char* end = value.data() + value.size();
    const auto [ptr, ec] = std::from_chars(begin, end, parsedId);
    if (ec != std::errc{} || ptr != end) {
        return std::nullopt;
    }

    return parsedId;
}

std::vector<Expense> CLI_Interface::collectFilteredExpenses(const std::unordered_map<std::string, std::string>& options) const
{
    const auto categoryIt = options.find("cat");
    std::optional<std::string> category;
    if (categoryIt != options.end()) {
        if (categoryIt->second.empty()) {
            throw std::runtime_error("Invalid category. Expected a non-empty value after --cat.");
        }

        category = categoryIt->second;
    }

    const auto fromIt = options.find("from");
    const auto toIt = options.find("to");
    std::optional<std::string> startDate;
    std::optional<std::string> endDate;
    if (fromIt != options.end() || toIt != options.end()) {
        if (fromIt == options.end() || toIt == options.end()) {
            throw std::runtime_error("Date range filtering requires both --from and --to.");
        }

        if (!isValidDate(fromIt->second) || !isValidDate(toIt->second)) {
            throw std::runtime_error("Invalid date range. Expected format YYYY-MM-DD for --from and --to.");
        }

        if (fromIt->second > toIt->second) {
            throw std::runtime_error("Invalid date range. --from must be earlier than or equal to --to.");
        }

        startDate = fromIt->second;
        endDate = toIt->second;
    }

    return expenseManager_.filterExpenses(category, startDate, endDate);
}

bool CLI_Interface::isValidDate(std::string_view value) const
{
    if (value.size() != 10 || value[4] != '-' || value[7] != '-') {
        return false;
    }

    const auto isDigit = [](char character) {
        return character >= '0' && character <= '9';
    };

    for (std::size_t index = 0; index < value.size(); ++index) {
        if (index == 4 || index == 7) {
            continue;
        }

        if (!isDigit(value[index])) {
            return false;
        }
    }

    const int month = std::stoi(std::string{value.substr(5, 2)});
    const int day = std::stoi(std::string{value.substr(8, 2)});
    return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

std::unordered_map<std::string, std::string> CLI_Interface::parseOptions(const std::vector<std::string_view>& arguments) const
{
    std::unordered_map<std::string, std::string> options;

    for (std::size_t index = 1; index < arguments.size(); ++index) {
        const std::string_view argument = arguments[index];
        if (argument.size() < 3 || argument[0] != '-' || argument[1] != '-') {
            continue;
        }

        const std::string key{argument.substr(2)};
        if (index + 1 >= arguments.size()) {
            options[key] = "";
            continue;
        }

        options[key] = std::string{arguments[index + 1]};
        ++index;
    }

    return options;
}

bool CLI_Interface::validateAllowedOptions(
    const std::unordered_map<std::string, std::string>& options,
    std::initializer_list<std::string_view> allowedOptions,
    std::string_view commandName) const
{
    for (const auto& [key, _] : options) {
        const bool isAllowed = std::any_of(allowedOptions.begin(), allowedOptions.end(), [&key](std::string_view allowedOption) {
            return key == allowedOption;
        });

        if (!isAllowed) {
            std::cerr << "Unsupported option '--" << key << "' for command " << commandName << ".\n";
            printCommandUsage(commandName);
            return false;
        }
    }

    return true;
}

void CLI_Interface::printCommandUsage(std::string_view commandName) const
{
    if (commandName == "add") {
        std::cerr << "Usage: expense add --amount <value> --cat <category> --date <YYYY-MM-DD> --description <text>\n";
        return;
    }

    if (commandName == "list") {
        std::cerr << "Usage: expense list [--cat <category>] [--from <YYYY-MM-DD> --to <YYYY-MM-DD>]\n";
        return;
    }

    if (commandName == "stats") {
        std::cerr << "Usage: expense stats [--cat <category>] [--from <YYYY-MM-DD> --to <YYYY-MM-DD>]\n";
        return;
    }

    if (commandName == "delete") {
        std::cerr << "Usage: expense delete --id <number>\n";
    }
}

void CLI_Interface::printExpensesTable(std::vector<Expense> expenses) const
{
    std::sort(expenses.begin(), expenses.end(), [](const Expense& left, const Expense& right) {
        return left.id < right.id;
    });

    std::cout << std::left
              << std::setw(6) << "ID"
              << std::setw(12) << "Amount"
              << std::setw(18) << "Category"
              << std::setw(14) << "Date"
              << "Description\n";

    if (expenses.empty()) {
        return;
    }

    std::cout << std::fixed << std::setprecision(2);
    for (const Expense& expense : expenses) {
        std::cout << std::left
                  << std::setw(6) << expense.id
                  << std::setw(12) << expense.amount
                  << std::setw(18) << expense.category
                  << std::setw(14) << expense.date
                  << expense.description << '\n';
    }
}
