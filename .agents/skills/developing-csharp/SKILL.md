---
name: developing-csharp
description: Builds, debugs, and extends C# projects targeting .NET 10 using repo-native conventions and modern .NET practices. Use when working on .cs files, SDK-style projects, solution layout, NuGet dependencies, tests, or runtime/build failures.
---

# Developing In C#

Use this skill for day-to-day C# work on repositories that use the `dotnet` SDK, especially projects targeting `net10.0`.

## First Pass

Start by understanding the project before changing code.

1. Identify the entry points and project layout. Check for `*.sln`, `*.csproj`, `Directory.Build.props`, `Directory.Build.targets`, `global.json`, `NuGet.Config`, and test projects.
2. Confirm the SDK and target framework. Prefer the repo's pinned SDK in `global.json`; otherwise verify the local SDK with `dotnet --version` and `dotnet --info`.
3. Read the existing style before editing. Match naming, nullability usage, file-scoped namespaces, constructor patterns, async conventions, and test structure already present in the repo.
4. Find the narrowest local verification path first: solution build, single project build, targeted test project, or a specific test filter.

## Working Style

Prefer the smallest correct change.

1. Keep domain logic in plain types before introducing framework-heavy abstractions.
2. Prefer dependency injection only where the surrounding code already uses it or where object lifetime genuinely matters.
3. Keep interfaces explicit with nullable annotations, `required` members, clear DTO boundaries, and narrow public APIs.
4. Prefer async end-to-end for I/O-bound work. Do not wrap synchronous CPU work in fake async code.
5. Avoid broad refactors while fixing a local issue. Separate behavior changes from cleanup.
6. Add comments only where the intent is not obvious from the code.

## Project And Build Workflow

Prefer the repository's documented path. If none exists, use the standard `dotnet` workflow.

1. Restore dependencies when needed:
   - `dotnet restore`
2. Build the full solution or the affected project:
   - `dotnet build`
   - `dotnet build <project>.csproj`
3. When the repo uses configurations explicitly, prefer:
   - `dotnet build -c Debug`
   - `dotnet build -c Release`
4. When the repo targets `.NET 10`, keep the target framework as `net10.0` unless the task explicitly requires changing it.
5. If `global.json` pins an SDK, do not silently upgrade it. Call out any mismatch between the repo requirement and the installed SDK.

## Common Files To Check

These files usually define the effective build and runtime behavior.

1. `global.json` for the SDK version.
2. `Directory.Build.props` and `Directory.Build.targets` for shared settings.
3. `*.csproj` for target frameworks, package references, analyzers, nullable mode, implicit usings, and output type.
4. `appsettings*.json`, `.http` files, or launch settings when the app has runtime configuration.
5. Test project files for test framework and mocking setup.

## Implementation Heuristics

Use modern .NET features selectively and only when they fit the existing codebase.

1. Prefer `record` or `record struct` for value-like data models when that matches the domain semantics.
2. Prefer `sealed` for leaf classes when extension is not part of the design.
3. Use `IReadOnlyList<T>`, `IEnumerable<T>`, or concrete collections intentionally; do not expose more mutability than needed.
4. Keep LINQ readable. If a query becomes dense or allocates unnecessarily in a hot path, rewrite it plainly.
5. Prefer `TryParse`, guards, and explicit validation for user input or external data.
6. Respect nullable reference types. Fix the nullability contract rather than silencing warnings without reason.
7. Use exceptions for exceptional failures, not normal control flow.

## Debugging Build And Runtime Failures

Treat the first real failure as the main signal.

1. For restore or build failures, inspect the earliest project-level error before chasing cascades.
2. For SDK issues, verify `global.json`, installed SDKs, and whether `net10.0` is supported locally.
3. For missing types or members, check target framework, package version, conditional compilation, implicit usings, and namespace imports.
4. For runtime DI failures, inspect service registration, service lifetime, and whether required configuration is present.
5. For serialization issues, confirm naming policy, required fields, converters, and null-handling assumptions.

## Testing And Verification

Always verify with the narrowest meaningful checks, then widen if needed.

1. Run the affected test project first:
   - `dotnet test <test-project>.csproj`
2. For targeted tests, use filters:
   - `dotnet test --filter "FullyQualifiedName~<NameFragment>"`
3. After every implemented change, run at least one verification step immediately. Prefer a targeted test first; if no relevant automated test exists, add one when practical before widening the change.
4. When there are no tests yet, build the affected project and run the smallest meaningful manual verification path. Do not leave code changes unverified.
5. If the repo enables analyzers or formatting checks, run them rather than guessing:
   - `dotnet format --verify-no-changes`
   - `dotnet build -warnaserror` when the repo already treats warnings strictly

## Change Validation Loop

Every code change should follow a tight implementation and validation cycle.

1. Before editing, identify the exact command that will prove the change worked: a filtered test, a test project, or the smallest runnable path.
2. Implement the smallest coherent slice of the change.
3. Run the narrowest verification immediately after that slice lands:
   - `dotnet test <test-project>.csproj --filter "FullyQualifiedName~<NameFragment>"`
   - `dotnet test <test-project>.csproj`
   - `dotnet build <project>.csproj`
4. If the change fixes a bug and no automated test covers it, add a regression test when practical before considering the task complete.
5. Once the local slice passes, widen verification only as far as needed for confidence.
6. In the final response, report exactly which verification command was run after the change.

## Common Task Patterns

### Adding A Feature

1. Find the controller, handler, service, or endpoint where the behavior enters.
2. Trace data contracts across validation, business logic, persistence, and output.
3. Implement the smallest vertical slice that proves the feature.
4. Build and test only the touched scope first.

### Fixing A Bug

1. Reproduce the issue if feasible.
2. Decide whether the root cause is validation, nullability, async flow, DI wiring, serialization, or state handling.
3. Fix the root cause before polishing surrounding code.
4. Add a regression test when the bug is subtle or likely to return.

### Refactoring

1. Preserve behavior first.
2. Move code in small steps with frequent builds.
3. Avoid mixing namespace reshuffles, style churn, and logic changes in one edit.

## Response Expectations

When using this skill:

1. Explain the actual cause of the issue, not just the code diff.
2. Reference the `dotnet` command used for verification.
3. Call out assumptions about SDK version, target framework, analyzers, or test framework when they affect the result.
