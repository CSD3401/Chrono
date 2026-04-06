"""
Chrono - Upload installer to GitHub Release via `gh` CLI.

Usage:
    python upload_release.py [--version VERSION] [--notes "Release notes"]

If --version is omitted, it defaults to the current date (YYYY.MM.DD).
If --notes is omitted, a default message is used.

Prerequisites:
    - GitHub CLI (gh) installed and authenticated: https://cli.github.com/
    - Run: gh auth login
"""

import argparse
import os
import subprocess
import sys
from datetime import datetime
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parent
INSTALLER_DIR = PROJECT_ROOT / "Installer Submission" / "INSTALLER"


def run(cmd, check=True):
    """Run a shell command and print it."""
    print(f"  > {cmd}")
    result = subprocess.run(cmd, shell=True, check=check,
                            capture_output=True, text=True)
    if result.stdout.strip():
        print(f"    {result.stdout.strip()}")
    if result.returncode != 0 and result.stderr.strip():
        print(f"    STDERR: {result.stderr.strip()}", file=sys.stderr)
    return result


def find_installer():
    """Find the built installer executable."""
    if not INSTALLER_DIR.exists():
        return None
    for f in INSTALLER_DIR.iterdir():
        if f.suffix == ".exe" and f.name.startswith("Chrono_Setup"):
            return f
    return None


def main():
    parser = argparse.ArgumentParser(description="Upload Chrono installer to GitHub Release")
    parser.add_argument("--version", default=None,
                        help="Release version tag (default: YYYY.MM.DD)")
    parser.add_argument("--notes", default=None,
                        help="Release notes body")
    parser.add_argument("--prerelease", action="store_true",
                        help="Mark as a pre-release")
    parser.add_argument("--draft", action="store_true",
                        help="Create as a draft release")
    args = parser.parse_args()

    # --- Check gh is available ---
    result = run("gh --version", check=False)
    if result.returncode != 0:
        print("ERROR: GitHub CLI (gh) not found. Install it from https://cli.github.com/")
        sys.exit(1)

    # --- Find the installer ---
    installer = find_installer()
    if not installer:
        print(f"ERROR: No Chrono_Setup*.exe found in {INSTALLER_DIR}")
        print("Run build_installer.bat first.")
        sys.exit(1)
    print(f"Found installer: {installer}")

    # --- Determine version tag ---
    version = args.version or datetime.now().strftime("%Y.%m.%d")
    tag = f"v{version}"

    # --- Determine the repo ---
    repo_result = run("gh repo view --json nameWithOwner -q .nameWithOwner", check=False)
    if repo_result.returncode != 0 or not repo_result.stdout.strip():
        print("ERROR: Could not detect GitHub repo. Ensure you're in a git repo with a GitHub remote.")
        sys.exit(1)
    repo = repo_result.stdout.strip()
    print(f"Repository: {repo}")

    # --- Build release notes ---
    notes = args.notes or f"Chrono build {tag}\nAuto-uploaded from CI."

    # --- Create the release and upload ---
    title = f"Chrono {tag}"
    cmd = (
        f'gh release create "{tag}"'
        f' "{installer}"'
        f' --repo "{repo}"'
        f' --title "{title}"'
        f' --notes "{notes}"'
    )
    if args.prerelease:
        cmd += " --prerelease"
    if args.draft:
        cmd += " --draft"

    print(f"\nCreating release {tag} and uploading {installer.name}...")
    result = run(cmd, check=False)
    if result.returncode != 0:
        print("ERROR: Release creation/upload failed.")
        sys.exit(1)

    print(f"\nRelease {tag} created successfully!")
    print(f"  https://github.com/{repo}/releases/tag/{tag}")


if __name__ == "__main__":
    main()
