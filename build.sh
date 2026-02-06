#!/bin/sh

set -eu

ROOT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
cd "${ROOT_DIR}"

BUILD_TYPE="${1:-Release}"
BUILD_ROOT="build/${BUILD_TYPE}"
CONAN_DIR="${BUILD_ROOT}/conan"
CMAKE_DIR="${BUILD_ROOT}/cmake"
INSTALL_DIR="${BUILD_ROOT}/install"

CONAN_BIN="${CONAN_BIN:-conan}"
if [ -x "${ROOT_DIR}/.venv/bin/conan" ]; then
  CONAN_BIN="${ROOT_DIR}/.venv/bin/conan"
fi
if ! command -v "${CONAN_BIN}" >/dev/null 2>&1; then
  echo "ERROR: conan not found. Install Conan 2 or set CONAN_BIN." >&2
  exit 1
fi
export CONAN_HOME="${CONAN_HOME:-${ROOT_DIR}/.conan2}"

cmake --version >/dev/null

# Safe to run repeatedly; ensures local default profile exists.
"${CONAN_BIN}" profile detect --force >/dev/null

"${CONAN_BIN}" install . \
  --output-folder "${CONAN_DIR}" \
  --build=missing \
  -s build_type="${BUILD_TYPE}"

cmake -S . -B "${CMAKE_DIR}" \
  -DCMAKE_TOOLCHAIN_FILE="${CONAN_DIR}/conan_toolchain.cmake" \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

cmake --build "${CMAKE_DIR}" -j
cmake --install "${CMAKE_DIR}" --prefix "${INSTALL_DIR}"

echo "Artifacts: ${BUILD_ROOT}/bin"
echo "Install:   ${INSTALL_DIR}/bin"
