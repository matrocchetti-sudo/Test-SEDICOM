#pragma once

#include <string>

// Passive domain data for a single expense record.
// This type is intentionally a struct because it only groups data and does not
// enforce behavior or invariants internally.
struct Expense {
    // Unique identifier used to reference and delete a specific expense.
    int id{};
    // Monetary amount associated with the expense.
    double amount{};
    // Logical category such as "Food" or "Transport".
    std::string category;
    // Expense date in YYYY-MM-DD format.
    std::string date;
    // Free-form user description.
    std::string description;
};
