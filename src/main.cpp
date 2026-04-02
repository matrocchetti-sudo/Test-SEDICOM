#include "CLI_Interface.hpp"
#include "DataHandler.hpp"
#include "ExpenseManager.hpp"

#include <filesystem>
#include <iostream>
#include <string_view>
#include <vector>

namespace {

// Places expenses.csv beside the executable, matching the current project
// specification.
std::filesystem::path resolveDataFilePath(const char* executablePath)
{
    std::filesystem::path path = executablePath != nullptr ? std::filesystem::path(executablePath) : std::filesystem::path{};
    if (path.has_parent_path()) {
        return std::filesystem::absolute(path).parent_path() / "expenses.csv";
    }

    return std::filesystem::current_path() / "expenses.csv";
}

// Converts argv into a lighter non-owning view expected by the CLI layer.
std::vector<std::string_view> makeArgumentsView(int argc, char* argv[])
{
    std::vector<std::string_view> arguments;
    arguments.reserve(argc > 1 ? static_cast<std::size_t>(argc - 1) : 0U);

    for (int index = 1; index < argc; ++index) {
        arguments.emplace_back(argv[index]);
    }

    return arguments;
}

} // namespace

int main(int argc, char* argv[])
{
    try {
        // Bootstrap the application layers in dependency order:
        // persistence -> business logic -> CLI.
        DataHandler dataHandler(resolveDataFilePath(argc > 0 ? argv[0] : nullptr));
        ExpenseManager expenseManager(dataHandler);
        CLI_Interface cli(expenseManager);

        return cli.run(makeArgumentsView(argc, argv));
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}
