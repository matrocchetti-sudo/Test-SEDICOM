#pragma once

#include <string>

struct Expense {
    int id{};
    double amount{};
    std::string category;
    std::string date;
    std::string description;
};
