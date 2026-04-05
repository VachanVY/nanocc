# CI Workflows

This folder contains the GitHub Actions CI workflow for nanocc.

## Workflow

- File: `test.yml`
- Name: `Build and Test`
- Runs on: `ubuntu-latest`

## When CI Runs

The workflow is triggered on:

- `push`
- `pull_request`
- `issue_comment` (new or edited comments)

## Default Behavior (push / pull_request)

For regular pushes and pull requests, CI does this:

1. Installs build tools (`build-essential`, `cmake`, `gcc-14`, `g++-14`)
2. Uses `g++-14` as the C++ compiler (C++23)
3. Builds the codebase with `./buildcc.sh compiler`
4. Reconfigures CMake with tests enabled (`-DBUILD_NANOCC_TEST=ON`)
5. Builds test targets
6. Ensures `./nanocc_tests/test_compiler` exists (clones the test suite if needed)
7. Runs tests via:
   `./nanocc_tests/test_compiler ./build/tools/nanocc_codegen -v`

`--chapter <N>` is resolved as follows:

- For PRs with `/ci` control comments: use the latest trusted comment value.
- Otherwise: use repository variable `NANOCC_DEFAULT_CHAPTER` (fallback: `20`).

If build or tests fail, the workflow fails (red status).

## Comment-Driven CI Control (pull requests)

You can control what CI runs by commenting on a pull request.

Only comments from `OWNER`, `MEMBER`, or `COLLABORATOR` are allowed to trigger comment-driven behavior.

All control comments must start with `/ci`.

For `pull_request` runs, CI reads the latest trusted `/ci ...` comment on that PR and applies it.

### 1) Run up to a chapter

Run only tests up to chapter N:

```text
/ci chapter=3
```

Equivalent form:

```text
/ci chapter 3
```

### 2) Run selected tests

Append extra arguments to the default `test_compiler` command:

```text
/ci tests=--fold-constants --int-only
```

### 3) Run an exact command (full control)

Run exactly what you provide:

```text
/ci cmd ./nanocc_tests/test_compiler ./build/tools/nanocc_codegen --chapter 19 -v --fold-constants --int-only
```

Notes:

- Exact command mode takes precedence over chapter/tests filters.
- CI ensures `./nanocc_tests/test_compiler` is available by cloning `https://github.com/VachanVY/writing-a-c-compiler-tests.git` into `nanocc_tests` when needed.

## PR Status

Workflow results appear in pull requests as standard GitHub checks (green/red).
