# Contributing

Thanks for contributing! This project favors small, focused changes that are easy to review.

## How to file issues

- Search existing issues first.
- Include OS, compiler version, and steps to reproduce.
- Add expected vs actual behavior and any logs.

## Branching strategy

- feature/\* for new functionality
- fix/\* for bug fixes
- chore/\* for tooling/docs updates

## Commit message format

Use conventional commits:

- feat: add new capability
- fix: correct a bug
- test: update or add tests
- docs: documentation changes
- chore: maintenance

Example:

```
feat: add csv export

Include JSON export and update README usage examples.
```

## Coding style

- C++17 only
- No `using namespace std;`
- Prefer `std::getline` for user input
- Keep functions small and single-purpose
- Favor clear, precise error messages

## Tests required

- All unit tests must pass
- Add tests for new behavior or bug fixes

## Pull request checklist

- [ ] Code builds on Windows, macOS, and Linux
- [ ] Tests pass via `ctest --output-on-failure`
- [ ] New behavior has tests
- [ ] README updated if user-facing changes
- [ ] No new warnings with `-Wall -Wextra -Wpedantic`
