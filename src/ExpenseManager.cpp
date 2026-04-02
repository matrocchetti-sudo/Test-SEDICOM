#include "ExpenseManager.hpp"

#include <algorithm>
#include <numeric>
#include <utility>

ExpenseManager::ExpenseManager(DataHandler& dataHandler)
    : dataHandler_(dataHandler)
{
    load();
}

void ExpenseManager::load()
{
    expenses_ = dataHandler_.loadExpenses();
}

void ExpenseManager::save() const
{
    dataHandler_.saveExpenses(expenses_);
}

const std::vector<Expense>& ExpenseManager::getExpenses() const noexcept
{
    return expenses_;
}

int ExpenseManager::getNextExpenseId() const noexcept
{
    int maxId = 0;

    // IDs are generated from the current maximum to avoid collisions after
    // loading persisted records.
    for (const Expense& expense : expenses_) {
        maxId = std::max(maxId, expense.id);
    }

    return maxId + 1;
}

Expense ExpenseManager::addExpense(double amount, std::string category, std::string date, std::string description)
{
    Expense expense{getNextExpenseId(), amount, std::move(category), std::move(date), std::move(description)};

    expenses_.push_back(expense);
    save();
    return expense;
}

bool ExpenseManager::deleteExpenseById(int id)
{
    const auto iterator = std::remove_if(expenses_.begin(), expenses_.end(), [id](const Expense& expense) {
        return expense.id == id;
    });

    if (iterator == expenses_.end()) {
        return false;
    }

    expenses_.erase(iterator, expenses_.end());
    save();
    return true;
}

std::optional<Expense> ExpenseManager::findExpenseById(int id) const
{
    const auto iterator = std::find_if(expenses_.begin(), expenses_.end(), [id](const Expense& expense) {
        return expense.id == id;
    });

    if (iterator == expenses_.end()) {
        return std::nullopt;
    }

    return *iterator;
}

std::vector<Expense> ExpenseManager::filterByCategory(const std::string& category) const
{
    std::vector<Expense> filteredExpenses;

    for (const Expense& expense : expenses_) {
        if (expense.category == category) {
            filteredExpenses.push_back(expense);
        }
    }

    return filteredExpenses;
}

std::vector<Expense> ExpenseManager::filterByDateRange(const std::string& startDate, const std::string& endDate) const
{
    std::vector<Expense> filteredExpenses;

    // YYYY-MM-DD can be compared lexicographically when the format is
    // normalized and zero-padded.
    for (const Expense& expense : expenses_) {
        if (expense.date >= startDate && expense.date <= endDate) {
            filteredExpenses.push_back(expense);
        }
    }

    return filteredExpenses;
}

std::vector<Expense> ExpenseManager::filterExpenses(
    const std::optional<std::string>& category,
    const std::optional<std::string>& startDate,
    const std::optional<std::string>& endDate) const
{
    std::vector<Expense> filteredExpenses;

    for (const Expense& expense : expenses_) {
        if (category.has_value() && expense.category != *category) {
            continue;
        }

        if (startDate.has_value() && expense.date < *startDate) {
            continue;
        }

        if (endDate.has_value() && expense.date > *endDate) {
            continue;
        }

        filteredExpenses.push_back(expense);
    }

    return filteredExpenses;
}

double ExpenseManager::getTotalAmount() const noexcept
{
    return std::accumulate(expenses_.begin(), expenses_.end(), 0.0, [](double total, const Expense& expense) {
        return total + expense.amount;
    });
}

double ExpenseManager::getAverageAmount() const noexcept
{
    if (expenses_.empty()) {
        return 0.0;
    }

    return getTotalAmount() / static_cast<double>(expenses_.size());
}

std::map<std::string, double> ExpenseManager::getTotalAmountByCategory() const
{
    std::map<std::string, double> totalsByCategory;

    for (const Expense& expense : expenses_) {
        totalsByCategory[expense.category] += expense.amount;
    }

    return totalsByCategory;
}

std::map<std::string, double> ExpenseManager::getMonthlyTotals() const
{
    std::map<std::string, double> monthlyTotals;

    for (const Expense& expense : expenses_) {
        if (expense.date.size() >= 7) {
            monthlyTotals[expense.date.substr(0, 7)] += expense.amount;
        }
    }

    return monthlyTotals;
}
