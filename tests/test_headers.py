"""Compile public headers in isolation and prove NDEBUG cannot disable tests."""
import os
from pathlib import Path
import shlex
import subprocess
import sys

root = Path(__file__).resolve().parents[1]
cc = shlex.split(sys.argv[1] if len(sys.argv) > 1 else os.environ.get("CC", "cc"))
for header in ("bot_core.h", "bot_config.h", "bot.h"):
    source = f'#include "{header}"\n#include "{header}"\nint main(void) {{return 0;}}\n'
    result = subprocess.run([*cc, "-std=c99", "-Wall", "-Wextra", "-Werror", "-Iinc", "-x", "c", "-fsyntax-only", "-"],
                            cwd=root, input=source, text=True, capture_output=True)
    assert result.returncode == 0, result.stderr
result = subprocess.run([*cc, "-std=c99", "-DNDEBUG", "-Iinc", "-fsyntax-only", "tests/test_core.c"],
                        cwd=root, text=True, capture_output=True)
assert result.returncode != 0 and "Tests require assertions" in result.stderr
print("headers: isolated includes and NDEBUG guard passed")
