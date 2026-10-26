#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

if ! command -v cmake >/dev/null 2>&1; then
    printf 'ERROR: CMake is required. Run ./install-deps.sh first.\n' >&2
    exit 1
fi

cmake_args=()
if [[ -x "$project_dir/.venv/bin/python" ]]; then
    cmake_args+=("-DPython3_EXECUTABLE=$project_dir/.venv/bin/python")
fi

cmake -S "$project_dir" -B "$project_dir/bin" "${cmake_args[@]}"
cmake --build "$project_dir/bin" --target all --parallel "${BUILD_JOBS:-4}"
exec "$project_dir/bin/axilang" "$@"

