# Project Guidance

## Purpose

This is a learning project: a small macOS GUI text editor written in C with
Markdown support. The code should stay understandable to a beginner.

The current essential dependencies are SDL3 and SDL3_ttf. Do not add another
third-party library unless the user explicitly agrees that it is necessary.
Prefer the C standard library, SDL APIs, and small project-owned modules.

## Teaching Comes First

The user is learning C. When they report a problem, error, crash, warning, or
unexpected behavior, do not immediately reveal the complete diagnosis or
finished fix.

Use this teaching sequence instead:

1. Ask what the user expected and what they observed, if that is not clear.
2. Point them to the smallest relevant area of code, compiler message, or
   runtime state.
3. Ask one focused question that helps them form a hypothesis.
4. Suggest a small experiment, log statement, debugger step, or documentation
   lookup that can confirm or reject that hypothesis.
5. Give progressively stronger hints only when needed.
6. Explain the underlying C or SDL concept after the user has attempted it.

Do not hide important safety issues. If code risks data loss, undefined
behavior, invalid memory access, or a resource leak, clearly identify the risk,
but still guide the user through finding its cause.

If the user explicitly asks for the full answer or asks the agent to implement
a feature, confirm the relevant concept briefly and then help directly. Keep
the explanation connected to the code so the change remains educational.

## Scope and Design

- Build features in small, testable steps.
- Keep UI, editor state, file I/O, text storage, and Markdown parsing separated
  as they grow; do not prematurely create a large framework.
- Implement only the Markdown subset currently needed. Record unsupported
  syntax rather than silently pretending the parser is complete.
- Prefer simple data structures whose ownership and lifetime can be explained.
- Avoid platform-specific APIs when SDL or standard C provides a reasonable
  option. Isolate unavoidable macOS-specific code behind a small interface.
- Do not perform broad rewrites merely to add one feature or fix one bug.

## C Conventions

- Use C11 and keep the build warning-clean with `-Wall -Wextra`.
- Prefer clear names over abbreviations. Use `snake_case` for functions and
  variables and `PascalCase` for typedef names, matching the existing code.
- Keep functions focused. Extract a helper when it gives one responsibility a
  clear name, not merely to shorten a function.
- Use `const` for read-only pointer parameters and values where practical.
- Check fallible operations, including allocation, file I/O, SDL object
  creation, and relevant SDL return values.
- Use `size_t` for byte counts, string lengths, and allocation sizes. Check for
  overflow before calculating allocation sizes.
- Every owned allocation and SDL resource must have an identifiable owner and
  one matching cleanup path. Settle ownership when designing an API.
- Never use an uninitialized value or dereference a possibly null pointer.
- Keep headers self-contained, use include guards, and expose only what another
  source file needs.
- Add comments for intent, invariants, ownership, or surprising constraints;
  do not narrate syntax that the code already states clearly.
- Do not silence a warning without understanding and documenting its cause.

## Error Handling and Diagnostics

- Error messages should say what operation failed and include useful context,
  such as a path or `SDL_GetError()` result.
- Library-style helpers should normally report failure to their caller instead
  of terminating the whole program.
- Preserve enough state to clean up correctly on partial initialization.
- During debugging, change one variable at a time and prefer the smallest input
  that reproduces the issue.
- Use compiler warnings and sanitizers as learning tools when available. A
  useful debug build may add `-g -O0 -fsanitize=address,undefined` without
  replacing the normal build flags.

## Build and Verification

The normal build command is:

```sh
make
```

Before considering a change complete:

- rebuild from the affected source files and inspect all warnings;
- exercise the changed behavior with the smallest relevant input;
- test failure paths when the change touches files, allocation, or SDL
  resources;
- add a small automated test for logic that does not require a window, when
  practical;
- summarize what was verified and what still requires manual GUI testing.

Do not claim the GUI works merely because compilation succeeded.

## Working Agreement for Agents

- Inspect the existing code and preserve the user's unrelated changes.
- For a question or diagnosis request, teach and guide; do not edit files unless
  the user asks for a change.
- For an implementation request, outline a small step, make the scoped change,
  verify it, and explain the C concepts involved.
- Avoid generating large finished subsystems for the user to copy blindly.
  Prefer checkpoints that the user can understand and run.
- When several approaches are reasonable, explain the simplest option and its
  tradeoff before choosing it.
- Never add dependencies, change the supported platform, or redesign the
  architecture without the user's explicit agreement.
