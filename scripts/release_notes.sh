#!/usr/bin/env bash
#
# Builds the release body for a dev build and writes it to stdout.
#
#   scripts/release_notes.sh <previous release ref> <version>
#
# The summary is the set of CHANGELOG_LATEST.md lines added since the previous
# dev release, kept under their own headings. The full changelog and the commit
# list follow, both collapsed.

set -euo pipefail

PREV_REF=${1:?usage: release_notes.sh <previous release ref> <version>}
VERSION=${2:?usage: release_notes.sh <previous release ref> <version>}

CHANGELOG=CHANGELOG_LATEST.md
REPO_URL="https://github.com/${GITHUB_REPOSITORY:-emsesp/EMS-ESP32}"
MAX_COMMITS=250

prev_version=$(git show "${PREV_REF}:src/emsesp_version.h" 2>/dev/null |
    awk -F'"' '/EMSESP_APP_VERSION/ { print $2 }')

added=$(mktemp)
trap 'rm -f "${added}"' EXIT
git diff -U0 "${PREV_REF}..HEAD" -- "${CHANGELOG}" |
    sed -n 's/^+\([^+].*\)/\1/p' >"${added}"

# walk the changelog so the added lines keep the order and headings they have there
summary=$(awk '
    NR == FNR { if (NF) wanted[$0] = 1; next }
    /^#+ / { sub(/^#+ /, ""); heading = $0; next }
    $0 in wanted && !seen[$0]++ {
        if (heading != shown) { printf "\n### %s\n\n", heading; shown = heading }
        print
    }
' "${added}" "${CHANGELOG}")

echo "Development build of EMS-ESP v${VERSION}."
echo

if [ -n "${summary}" ]; then
    echo "## Changes since ${prev_version:-the previous dev build}"
    echo "${summary}"
    echo
elif [ -n "${prev_version}" ]; then
    echo "No changelog entries were added since ${prev_version}, see the commits below."
    echo
fi

echo "<details>"
echo "<summary>Full v${VERSION%%-*} changelog so far</summary>"
echo
sed '1,/^For more details go to/d' "${CHANGELOG}" | awk 'NF || seen { print; seen = 1 }'
echo
echo "</details>"
echo

count=$(git rev-list --no-merges --count "${PREV_REF}..HEAD")
echo "<details>"
echo "<summary>Commits (${count})</summary>"
echo
git log --no-merges --max-count="${MAX_COMMITS}" \
    --pretty=format:"- [\`%h\`](${REPO_URL}/commit/%H) %s (%an)" "${PREV_REF}..HEAD"
echo
if [ "${count}" -gt "${MAX_COMMITS}" ]; then
    echo
    echo "... and $((count - MAX_COMMITS)) more."
fi
echo
echo "</details>"
echo
echo "[Compare all changes](${REPO_URL}/compare/${PREV_REF}...$(git rev-parse HEAD))"
