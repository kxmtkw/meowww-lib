import re
from pathlib import Path

LIBRARY_DIR = Path("meowww")
DIST_DIR = Path("dist")
LOCAL_INCLUDE_RE = re.compile(r'^\s*#include\s+"([^"]+)"')
PRAGMA_ONCE_RE = re.compile(r'^\s*#pragma\s+once\s*$', re.IGNORECASE)
VISITED = set()


def resolve_includes(filepath: str | Path) -> str:
    path = Path(filepath)

    if path in VISITED:
        return ""

    VISITED.add(path)

    output = []
    output.append("\n")
    current_dir = path.parent

    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            if PRAGMA_ONCE_RE.match(line):
                continue

            match = LOCAL_INCLUDE_RE.match(line)

            if match:
                rel_path = match.group(1)

                target_path = (current_dir / rel_path).resolve()

                if not target_path.exists():
                    target_path = (LIBRARY_DIR / rel_path).resolve()

                output.append(resolve_includes(target_path))
            else:
                output.append(line)

    output.append("\n")
    return "".join(output)


if __name__ == "__main__":
    DIST_DIR.mkdir(parents=True, exist_ok=True)

    bundled = resolve_includes(LIBRARY_DIR / "meowww.h")
    with open(DIST_DIR / "meowww.h", "w", encoding="utf-8") as f:
        f.write(bundled)