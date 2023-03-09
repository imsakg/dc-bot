"""Offline command-line smoke tests: no Discord connection is attempted."""
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
BOT = ROOT / "build" / "dc_bot"

def run(*args, env_updates=None):
    env = dict(os.environ)
    env.pop("DISCORD_TOKEN", None)
    env.update(env_updates or {})
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
    invalid = ["prefix=\n", "prefix=123456789\n", "prefix=!\nprefix=?\n",
               "unknown=1\n", "cooldown_seconds=+2\n", "cooldown_seconds=3601\n",
               "welcome_guild=123\n", "welcome_channel=18446744073709551616\n",
               "prefix=" + "!" * 300 + "\n"]
    for text in invalid:
        settings.write_text(text)
        result = run("--check-config", "--settings", str(settings))
        assert result.returncode == 1, (text, result)
        assert "Invalid bot setting" in result.stderr
    settings.write_bytes(b"# comment\r\nprefix=!!\r\ncooldown_seconds=3600\r\n")
    assert run("--check-config", "--settings", str(settings)).returncode == 0
for option in ("--settings", "--config"):
    assert run(option).returncode == 1
assert run("--check-config", "--settings", "/nonexistent-dc-bot-settings").returncode == 1
secret_probe = "not a real token, must not be echoed"
result = run(env_updates={"DISCORD_TOKEN": secret_probe})
assert result.returncode == 1 and secret_probe not in result.stdout + result.stderr
assert run(env_updates={"DISCORD_TOKEN": ""}).returncode == 1
assert run("--help", env_updates={"DISCORD_TOKEN": "invalid token"}).returncode == 0
print("cli: all assertions passed")
