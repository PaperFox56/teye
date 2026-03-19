# Contributing to Teye

First off, thank you for considering contributing to Teye. This project is built on the philosophy of keeping things minimal, readable, and functional. 

Before you start, please read through these guidelines to ensure your changes align with the project's goals.

## Bug Reports and Issues
If you encounter a bug or have a suggestion, please open a [GitHub Issue](https://github.com/PaperFox56/teye/issues). 
- Use a clear and descriptive title.
- Provide steps to reproduce the issue (if applicable).
- Keep the discussion constructive.

## Pull Requests
You are free to fork the repository and modify the code to suit your needs. If you believe your changes would benefit others, please open a Pull Request (PR).

- **Keep it Simple**: We prioritize minimal, maintainable code over complex optimizations.
- **One PR per Feature/Fix**: If you are fixing multiple issues, please submit separate pull requests for each. This makes code reviews significantly easier.
- **Document Your Changes**: Please document your modifications. If you are adding a new feature, update the relevant documentation so it can be reviewed effectively.

## Coding Standards and Style
While these are guidelines, following them helps keep the codebase consistent:

### Documentation
- We use **Doxygen** to generate documentation. Please ensure your comments follow Doxygen syntax so the parser can process them correctly.

### Formatting & Style
- **Indentation**: Use spaces, not tabs. Please configure your editor/formatter accordingly.
- **Naming Conventions**:
  - `snake_case`: For functions and local variables.
  - `PascalCase`: For structs and enums.
  - `ALL_UPPERCASE`: For constants and macros.
- **API Namespace**: All public API functions must follow the `TEYE_*` prefix.

### Performance
- **Readability First**: Do not over-obfuscate your code in the name of performance. 
- **Benchmarks**: If you are submitting a performance-critical optimization, **you must provide a benchmark** proving a significant improvement. Please encapsulate the complex logic as much as possible.

---

*Thank you for helping keep Teye minimal and efficient!*
