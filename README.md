# ehsh

Introducing `ehsh`! The world's most okay shell for embedded systems! `ehsh`
aims to be simple and boring. It's probably similar to what you might write if
you had a spare day or two to write a shell, plus time to make docs, tests, etc.
Has niceties like tab completion, as seen below:

![Simple example gif](docs/ehsh.gif)

Users register commands as function pointers with a single parameter,
`EhShell_t*`, which contains the current command line with the arguments
tokenized, as shown below:

![](docs/args.drawio.svg)

The arguments are stored as offsets into the `CmdLine`, and each argument is
null terminated.

## Features

Be amazed by state-of-the-art (for embedded) features, such as:

- Statically allocated (no heap, no global state)!
- Only requires C99!
    - `memset`
    - `strchr`
    - `strncmp`
    - `strncpy`
    - `<stdbool.h>`
    - `<stdint.h>`
- Context pointers (Allows users to extend the shell without global state)!
- Desktop examples!
- Select tty mode (echo typed characters or not) at runtime!
- Select input EOL (CR or LF) and output EOL (CR, LF, CR+LF) at runtime!
- Tab completion!
- Actually tested & packaged (Pinky swear!)!
- Super permissive license!

## Usage

```cmake
add_subdirectory(ehsh)
target_link_libraries(main
  PRIVATE
    ehsh::ehsh
    # Or, if you want to override the weak symbols:
    $<LINK_LIBRARY:WHOLE_ARCHIVE,ehsh::ehsh>
)
```

## Building

This project ships with a convenience `Makefile` used for running common tasks.
See `make help` for more details.

```bash
# Builds, tests, and packages the debug version of the code.
$ make debug
# Rebuild by running the clean target first
$ make clean debug
# Format code
$ make format
# Run linters
$ make lint
```

## Contributing

Pinned dependencies can be installed via [devbox](https://www.jetify.com/devbox/docs/).
Simply run `devbox shell`, followed by one of the convenience `make` targets.

## TODO:

- Tests & Coverage
- Docs
- Fuzz
- Sanitizers
- Docker
- CI
- Warnings
- Packages / Update
    - Vcpkg: https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-packaging
    - deb?

@dir src
@brief src

@dir src/ehsh
@brief src/ehsh

@dir src/ehsh/extra
@brief src/ehsh/extra

@dir src/ehsh/platform
@brief src/ehsh/platform
