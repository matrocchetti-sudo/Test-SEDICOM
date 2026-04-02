#pragma once

#include "Expense.hpp"

#include <filesystem>
#include <vector>

// Persistence service responsible only for reading and writing expense data.
// It does not apply business rules and does not own in-memory application
// state beyond the configured file path.
class DataHandler {
public:
    // Binds the handler to the CSV file that will be used for persistence.
    // The path is stored as-is and used by load/save operations.
    explicit DataHandler(std::filesystem::path filePath);

    // Loads every expense from the configured CSV file.
    // Returns an empty collection when the file does not exist or is empty.
    // Throws std::runtime_error if the file cannot be opened or its format is
    // not compatible with the expected CSV schema.
    std::vector<Expense> loadExpenses() const;

    // Rewrites the configured CSV file with the provided expenses.
    // The method always writes the canonical header first, then one row per
    // expense. It creates parent directories when necessary.
    // Throws std::runtime_error if the file cannot be opened for writing.
    void saveExpenses(const std::vector<Expense>& expenses) const;

    // Exposes the exact path currently used for persistence.
    // Useful for diagnostics, testing, and user-facing error reporting.
    const std::filesystem::path& getFilePath() const noexcept;

private:
    std::filesystem::path filePath_;
};
