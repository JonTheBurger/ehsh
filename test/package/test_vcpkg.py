#!/usr/bin/env -S uv --quiet run --script
# /// script
# requires-python = ">=3.10"
# dependencies = [
#     "pytest",
# ]
# ///
import sys
from subprocess import CompletedProcess, run

import pytest


def test_vcpkg() -> None:
    run([
        "cmake", "--preset=vcpkg"
    ])


if __name__ == "__main__":
    sys.exit(pytest.main())
