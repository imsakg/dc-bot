"""Offline command-line smoke tests: no Discord connection is attempted."""
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
BOT = ROOT / "build" / "dc_bot"

def run(*args):
    env = dict(os.environ)
    env.pop("DISCORD_TOKEN", None)
    return subprocess.run([str(BOT), *args], cwd=ROOT, env=env,
                          text=True, capture_output=True, timeout=5)

assert run("--help").returncode == 0
assert "Usage:" in run("--help").stdout
assert run("--version").stdout.strip() == "dc-bot 0.2.0"
assert run("--unknown").returncode != 0
assert run("--config", "/nonexistent-dc-bot-file").returncode == 1
assert run("--check-config").returncode == 0
with tempfile.TemporaryDirectory(dir=os.environ.get("TMPDIR")) as directory:
    settings = Path(directory) / "bot.conf"
    settings.write_text("prefix=??\ncooldown_seconds=0\n")
    result = run("--check-config", "--settings", str(settings))
    assert result.returncode == 0 and "prefix=??" in result.stdout, result
    settings.write_text("prefix=\n")
    assert run("--check-config", "--settings", str(settings)).returncode != 0
print("cli: all assertions passed")
