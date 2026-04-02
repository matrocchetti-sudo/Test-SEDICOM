---
name: developing-cpp
description: Builds, debugs, and extends C++ projects using repo-native conventions and modern C++ practices. Use when working on C++ source files, compiler or linker errors, CMake configuration, tests, performance tuning, or memory-safety issues.
---

# Developing In C++

Use this skill for day-to-day C++ development work: feature changes, bug fixes, refactors, build failures, test failures, linker errors, and CMake updates.

## First Pass

Start by understanding the project before changing code.

1. Identify the build system and entry points. Check for `CMakeLists.txt`, `CMakePresets.json`, `compile_commands.json`, `vcpkg.json`, `conanfile.*`, `Makefile`, and test directories.
2. Detect the active C++ standard from the build files. Stay within the standard the repo already uses unless the task explicitly requires raising it.
3. Inspect existing code style before editing. Match naming, header layout, namespaces, include ordering, error-handling style, and ownership patterns already present in the repo.
4. Look for local verification paths first: documented build commands, test targets, formatter configs, sanitizer targets, or CI scripts.

## Working Style

Prefer the smallest correct change.

1. Use the standard library before adding new abstractions.
2. Prefer stack allocation and RAII. Avoid raw `new` and `delete` unless the surrounding code already relies on them.
3. Prefer value semantics by default. Introduce shared ownership only when ownership is genuinely shared.
4. Make interfaces explicit with `const`, references, `std::span`, `std::string_view`, `enum class`, and strong types where that improves correctness without fighting the existing codebase.
5. Keep headers lean. Include only what is needed, prefer forward declarations when they reduce coupling, and avoid putting heavy implementation details into headers unless the project intentionally uses header-only patterns.
6. Do not extract helpers aggressively. Inline simple logic unless reuse or complexity clearly justifies a new function or class.
7. Prefer `struct` over `class` for POD-style domain data and passive data carriers. Use `class` only when the type owns behavior, invariants, or encapsulation that materially benefits from access control.

## Build Workflow

Prefer the repository's documented build path. If none exists, use an out-of-source CMake build.

1. If the repo has presets, use them first:
   - `cmake --preset <preset>`
   - `cmake --build --preset <build-preset>`
2. If there are no presets, configure a local build directory:
   - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
   - `cmake --build build -j`
3. For a single target, build only that target when possible to shorten feedback loops:
   - `cmake --build build --target <target> -j`
4. When tests exist, prefer targeted test execution before full-suite runs.

## Debugging Compiler And Linker Errors

Treat the first real error as the primary signal.

1. For compiler errors, fix the earliest relevant diagnostic before chasing later cascades.
2. For template-heavy errors, find the first project file in the instantiation trace and reason outward from there.
3. For linker errors:
   - Undefined reference usually means a missing definition, wrong signature, missing source file in the target, or missing linked library.
   - Multiple definition usually means non-inline definitions in headers, duplicate object inclusion, or ODR violations.
4. For missing type or symbol errors, verify namespace, include visibility, forward declarations, and conditional compilation flags.
5. When a change affects headers, expect wider rebuilds and possible ABI-sensitive fallout.

## Implementation Heuristics

Apply modern C++ selectively, not performatively.

1. Prefer `std::unique_ptr` for exclusive ownership.
2. Use `std::optional` for maybe-values when absence is a valid state.
3. Use `std::variant` only when the domain truly has a closed set of alternatives.
4. Use exceptions only if the project already uses them for control flow or error propagation.
5. Use `noexcept` only when you can maintain the guarantee.
6. Prefer `std::vector` and contiguous storage by default; reach for more specialized containers only with a clear reason.
7. Keep object lifetimes obvious. If ownership is unclear, simplify before adding indirection.

## CMake Guidance

When editing CMake, preserve the project's style and keep changes local.

1. Prefer target-scoped commands such as `target_include_directories`, `target_link_libraries`, `target_compile_definitions`, and `target_compile_features`.
2. Avoid global flags unless the repo already centralizes them that way.
3. Keep private, public, and interface dependencies accurate.
4. When adding files, ensure they are attached to the correct target instead of only existing on disk.
5. If tests are target-based, register them through the existing `add_test` or helper macros instead of inventing a parallel structure.

## Verification

Always verify with the narrowest meaningful checks, then widen if needed.

1. Rebuild the affected target.
2. Run the most relevant tests.
3. If the repo supports them, use warnings, sanitizers, or static analysis to catch regressions:
   - `-Wall -Wextra -Wpedantic`
   - AddressSanitizer or UndefinedBehaviorSanitizer builds
   - `clang-tidy` or repo-specific lint steps
4. If a failure is hard to reproduce, add or run the smallest regression test that proves the fix.

## Common Task Patterns

### Adding A Feature

1. Find the entry point for the behavior.
2. Trace the data flow and ownership boundaries.
3. Implement the smallest vertical slice.
4. Build and test the touched target.

### Fixing A Bug

1. Reproduce the issue if feasible.
2. Identify whether the bug is caused by ownership, lifetime, invalid state, conversion, bounds, or build configuration.
3. Fix the root cause, not just the symptom.
4. Add a regression test only when it protects important behavior or the bug was subtle.

### Refactoring

1. Preserve behavior first.
2. Move code in small steps with frequent builds.
3. Avoid mixing broad renames, style churn, and logic changes in one edit.

### Performance Work

1. Measure before optimizing.
2. Focus on allocations, copies, virtual dispatch, poor data locality, and unnecessary string formatting only when they matter.
3. Prefer simple structural wins over clever micro-optimizations.

## Response Expectations

When using this skill:

1. Explain the actual cause of the issue, not just the final code diff.
2. Reference the build or test command used for verification.
3. Call out any assumptions about compiler version, standard level, platform, or dependency manager if they affect the result.
