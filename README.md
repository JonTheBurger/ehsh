# ehsh

Introducing `ehsh`! The world's most okay shell for embedded systems!

_**TODO:**_ Cool Gif ;)

## Features

Be amazed by state-of-the-art (for embedded) features, such as:

- Enough example gifs to blow out your 1.2k baud modem (one) - see above!
- No heap allocation or global memory (multiple terminals)!
- Select input EOL (CR or LF) and output EOL (CR, LF, CR+LF) at runtime!
- Select tty mode (echo typed characters or not) at runtime!
- Tab completion (Coming soon?)!
- Context pointers!
- Only requires C99!
    - `memset`
    - `strchr`
    - `strncmp`
    - `<stdbool.h>`
    - `<stdint.h>`
- Actually tested & packaged (Pinky swear!)!
- Desktop examples!
- Super permissive license!

Enjoy thrilling, pragmatic limitations, including:

- Current command line & cursor are stored in fixed buffers!

## Usage

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

## TODO:

- Tests & Coverage
- Docs
- Fuzz
- Sanitizers
- devbox.json
- Docker
- CI
- Warnings
- Packages / Update
    - Arduino: https://arduino.github.io/arduino-cli/1.0/library-specification/#libraryproperties-file-format
    - Platformio: https://docs.platformio.org/en/latest/manifests/library-json/index.html
    - Conan: https://docs.conan.io/2/tutorial/creating_packages/create_your_first_package.html
    - Vcpkg: https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-packaging
    - pkgconfig: https://people.freedesktop.org/~dbn/pkg-config-guide.html
    - ipcf
    - Makefile
    - deb?
