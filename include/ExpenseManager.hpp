#pragma once

#include "DataHandler.hpp"
#include "Expense.hpp"

#include <map>
#include <optional>
#include <string>
#include <vector>

class ExpenseManager {
public:
    explicit ExpenseManager(DataHandler& dataHandler);

    void load();
    void save() const;

    const std::vector<Expense>& getExpenses() const noexcept;
    int getNextExpenseId() const noexcept;

    Expense addExpense(double amount, std::string category, std::string date, std::string description);
    bool deleteExpenseById(int id);

    std::optional<Expense> findExpenseById(int id) const;
    std::vector<Expense> filterByCategory(const std::string& category) const;
    std::vector<Expense> filterByDateRange(const std::string& startDate, const std::string& endDate) const;

    double getTotalAmount() const noexcept;
    std::map<std::string, double> getTotalAmountByCategory() const;

private:
    DataHandler& dataHandler_;
    std::vector<Expense> expenses_;
};
