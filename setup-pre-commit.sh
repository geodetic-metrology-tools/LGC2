#!/bin/bash
# Setup script for pre-commit hooks in LGC2
# This script installs pre-commit and sets up the git hooks

set -e  # Exit on error

echo "========================================="
echo "LGC2 Pre-commit Hooks Setup"
echo "========================================="
echo ""

# Check if Python is installed
if ! command -v python3 &> /dev/null && ! command -v python &> /dev/null; then
    echo "[ERROR] Python is not installed."
    echo "Please install Python 3.6+ and try again."
    echo "Download from: https://www.python.org/downloads/"
    exit 1
fi

# Determine Python command
if command -v python3 &> /dev/null; then
    PYTHON_CMD=python3
else
    PYTHON_CMD=python
fi

echo "[OK] Found Python: $($PYTHON_CMD --version)"
echo ""

# Check if pip is installed
if ! $PYTHON_CMD -m pip --version &> /dev/null; then
    echo "[ERROR] pip is not installed."
    echo "Please install pip and try again."
    exit 1
fi

echo "[OK] Found pip: $($PYTHON_CMD -m pip --version)"
echo ""

# Install pre-commit
echo "Installing pre-commit..."
$PYTHON_CMD -m pip install --user pre-commit

# Verify installation
if ! command -v pre-commit &> /dev/null; then
    echo "[WARNING] pre-commit command not found in PATH."
    echo "You may need to add Python user scripts directory to your PATH."
    echo ""
    echo "For Windows (Git Bash), add this to your ~/.bashrc:"
    echo "  export PATH=\"\$PATH:\$APPDATA/Python/Python3x/Scripts\""
    echo ""
    echo "Then run: source ~/.bashrc"
    echo ""
    read -p "Press Enter to continue..."
fi

echo "[OK] pre-commit installed successfully"
echo ""

# Install git hooks
echo "Installing git hooks..."
if command -v pre-commit &> /dev/null; then
    pre-commit install
    pre-commit install --hook-type commit-msg
    echo "[OK] Git hooks installed successfully"
    echo "[OK] Commit message hooks installed (for Signed-off-by check)"
    echo ""

    # Optional: Run hooks on all files
    read -p "Do you want to run pre-commit hooks on all existing files? (y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Running pre-commit on all files..."
        pre-commit run --all-files || true
        echo ""
        echo "Note: Some hooks may have modified files."
        echo "Please review the changes before committing."
    fi
else
    echo "[WARNING] Could not run 'pre-commit install' automatically."
    echo "Please run it manually after adding pre-commit to your PATH."
fi

echo ""
echo "========================================="
echo "Setup Complete!"
echo "========================================="
echo ""
echo "Pre-commit hooks will now run automatically before each commit."
echo ""
echo "IMPORTANT: Always commit with the -s flag:"
echo "  git commit -s -m \"Your commit message\""
echo ""
echo "This adds the required Signed-off-by line to your commits."
echo ""
echo "Useful commands:"
echo "  - Commit with sign-off:     git commit -s"
echo "  - Run hooks manually:       pre-commit run --all-files"
echo "  - Skip hooks (if needed):   git commit --no-verify"
echo "  - Update hooks:             pre-commit autoupdate"
echo ""
