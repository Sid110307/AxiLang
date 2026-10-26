#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$project_dir"

case "${1:-}" in
    "")
        if ! command -v apt-get >/dev/null 2>&1; then
            printf 'This installer requires Debian or Ubuntu with apt-get.\n' >&2
            exit 1
        fi
        elevate=()
        if (( EUID != 0 )); then
            elevate=(sudo)
        fi
        "${elevate[@]}" apt-get update
        "${elevate[@]}" apt-get install -y \
            build-essential cmake python3-dev python3-venv \
            libboost-python-dev libboost-program-options-dev \
            libboost-filesystem-dev libcurl4-openssl-dev
        ;;
    --skip-system) ;;
    *)
        printf 'Usage: %s [--skip-system]\n' "$0" >&2
        exit 1
        ;;
esac

/usr/bin/python3 -m venv .venv
.venv/bin/python -m pip install https://cdn.evilmadscientist.com/dl/ad/public/AxiDraw_API.zip
.venv/bin/python -m pip check
