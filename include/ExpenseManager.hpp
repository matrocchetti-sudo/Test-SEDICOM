#pragma once

#include "DataHandler.hpp"
#include "Expense.hpp"

#include <map>
#include <optional>
#include <string>
#include <vector>

// Business-logic layer for expense management.
// This class owns the in-memory collection of expenses and delegates file I/O
// to DataHandler.
class ExpenseManager {
public:
    // Creates a manager that will use the provided persistence service.
    // The handler is stored by reference, so it must outlive the manager.
    // The constructor immediately loads any previously persisted expenses.
    explicit ExpenseManager(DataHandler& dataHandler);

    // Replaces the current in-memory state with the content loaded from the
    // configured persistence file.
    void load();

    // Persists the current in-memory state through the associated DataHandler.
    void save() const;

    // Returns a read-only view of all currently loaded expenses.
    // No copy is made; callers must not retain the reference past the manager
    // lifetime.
    const std::vector<Expense>& getExpenses() const noexcept;

    // Computes the next available positive ID from the current collection.
    // This is based on the maximum existing ID plus one.
    int getNextExpenseId() const noexcept;

    // Creates a new expense in memory using the next available ID and the
    // provided field values.
    // The new in-memory state is persisted automatically before returning.
    Expense addExpense(double amount, std::string category, std::string date, std::string description);

    // Removes the expense identified by id from the in-memory collection.
    // Returns true when an expense was found and removed, false otherwise.
    // Successful deletions are persisted automatically.
    bool deleteExpenseById(int id);

    // Searches an expense by ID in the in-memory collection.
    // Returns std::nullopt when no matching record exists.
    std::optional<Expense> findExpenseById(int id) const;

    // Returns a new collection containing only expenses whose category matches
    // the provided value exactly.
    std::vector<Expense> filterByCategory(const std::string& category) const;

    // Returns a new collection containing expenses whose dates fall inside the
    // inclusive range [startDate, endDate].
    // The current implementation relies on lexical ordering of YYYY-MM-DD
    // strings, which is valid for normalized ISO-like dates.
    std::vector<Expense> filterByDateRange(const std::string& startDate, const std::string& endDate) const;

    // Returns a filtered view that can combine category and date-range
    // constraints in one place, keeping query rules out of the CLI layer.
    std::vector<Expense> filterExpenses(
        const std::optional<std::string>& category,
        const std::optional<std::string>& startDate,
        const std::optional<std::string>& endDate) const;

    // Computes the total amount of every currently loaded expense.
    double getTotalAmount() const noexcept;

    // Computes the arithmetic mean of all currently loaded expenses.
    // Returns 0.0 when no expenses are present.
    double getAverageAmount() const noexcept;

    // Aggregates total amount by category and returns one entry per category.
    std::map<std::string, double> getTotalAmountByCategory() const;

    // Aggregates total amount by YYYY-MM month key.
    std::map<std::string, double> getMonthlyTotals() const;

private:
    DataHandler& dataHandler_;
    std::vector<Expense> expenses_;
};
