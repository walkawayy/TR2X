#!/usr/bin/env python3
from pathlib import Path

from shared.docker import BaseGameEntrypoint


class WindowsEntrypoint(BaseGameEntrypoint):
    BUILD_ROOT = Path("/app/build/win/")
    COMPILE_ARGS = [
        "--cross",
        "/app/tools/docker/game-win/meson_linux_mingw32.txt",
    ]
    RELEASE_ZIP_SUFFIX = "Windows"
    RELEASE_ZIP_FILES = [
        (BUILD_ROOT / "TR2X.exe", "TR2X.exe"),
        (BUILD_ROOT / "TR2X.dll", "TR2X.dll"),
    ]

    def post_compile(self) -> None:
        if self.target == "release":
            for path in self.BUILD_ROOT.glob("*.exe"):
                self.compress_exe(path)
            for path in self.BUILD_ROOT.glob("*.dll"):
                self.compress_exe(path)


if __name__ == "__main__":
    WindowsEntrypoint().run()
