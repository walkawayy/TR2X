from pathlib import Path

TOOLS_DIR = Path(__file__).parent.parent
REPO_DIR = TOOLS_DIR.parent
DOCS_DIR = REPO_DIR / "docs"
DATA_DIR = REPO_DIR / "data"
SRC_DIR = REPO_DIR / "src"

PROGRESS_FILE = DOCS_DIR / "progress.txt"
