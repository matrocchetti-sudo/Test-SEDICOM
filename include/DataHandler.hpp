#pragma once

#include "Expense.hpp"

#include <filesystem>
#include <vector>

class DataHandler {
public:
    explicit DataHandler(std::filesystem::path filePath);

    std::vector<Expense> loadExpenses() const;
    void saveExpenses(const std::vector<Expense>& expenses) const;

    const std::filesystem::path& getFilePath() const noexcept;

private:
    std::filesystem::path filePath_;
};
