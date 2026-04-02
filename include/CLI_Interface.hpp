#pragma once

#include "ExpenseManager.hpp"

#include <string_view>
#include <vector>

class CLI_Interface {
public:
    explicit CLI_Interface(ExpenseManager& expenseManager);

    int run(const std::vector<std::string_view>& arguments);
    void printHelp() const;

private:
    ExpenseManager& expenseManager_;
};
