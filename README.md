# ehsh

Introducing `ehsh`! The world's most okay shell for embedded systems!

_**TODO:**_ Cool Gif ;)

## Features

Be amazed by state-of-the-art (for embedded) features, such as:

- Enough example gifs to blow out your 1.2k baud modem (one) - see above!
- No heap allocation or global memory (multiple terminals)!
- Select input EOL (CR or LF) and output EOL (CR, LF, CR+LF) at runtime!
- Select tty mode (echo typed characters or not) at runtime!
- Tab completion!
- Context pointers!
- Only requires C99!
    - `memset`
    - `strchr`
    - `strncmp`
    - `strncpy`
    - `<stdbool.h>`
    - `<stdint.h>`
- Actually tested & packaged (Pinky swear!)!
- Desktop examples!
- Super permissive license!

Enjoy thrilling, pragmatic limitations, including:

- Current command line & cursor are stored in fixed buffers!

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
