#pragma once

#include "ExpenseManager.hpp"

#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// Presentation layer for the command-line interface.
// It interprets CLI arguments, dispatches commands, and prints output.
class CLI_Interface {
public:
    // Creates the CLI front-end bound to the application business layer.
    // The manager reference must remain valid for the entire CLI lifetime.
    explicit CLI_Interface(ExpenseManager& expenseManager);

    // Executes one CLI invocation using arguments already stripped of argv[0].
    // Returns 0 on success and a non-zero exit code on command/usage errors.
    int run(const std::vector<std::string_view>& arguments);

    // Prints the currently supported command synopsis to standard output.
    void printHelp() const;

private:
    int handleAddCommand(const std::vector<std::string_view>& arguments);
    int handleListCommand(const std::vector<std::string_view>& arguments) const;
    int handleStatsCommand(const std::vector<std::string_view>& arguments) const;
    int handleDeleteCommand(const std::vector<std::string_view>& arguments);

    std::vector<Expense> collectFilteredExpenses(const std::unordered_map<std::string, std::string>& options) const;
    std::optional<double> parseAmount(std::string_view value) const;
    std::optional<int> parseId(std::string_view value) const;
    bool isValidDate(std::string_view value) const;
    std::unordered_map<std::string, std::string> parseOptions(const std::vector<std::string_view>& arguments) const;
    bool validateAllowedOptions(
        const std::unordered_map<std::string, std::string>& options,
        std::initializer_list<std::string_view> allowedOptions,
        std::string_view commandName) const;
    void printCommandUsage(std::string_view commandName) const;
    void printExpensesTable(std::vector<Expense> expenses) const;

    ExpenseManager& expenseManager_;
};
