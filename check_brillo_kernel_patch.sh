#!/bin/bash
#
# Verifies whether Brillo kernel patches are well-formed w.r.t. the guidelines
# in device/generic/brillo/KernelDevelopmentGuide.md
#
# The checks are certainly not comprehensive, but should cover the basic
# requirements.
#
# There is some additional checking implemented that allows to verify whether a
# change that's an annotated amalgamation of some history looks good, i.e.
# the resulting changes are the same, signoff information got retained etc.

# Globals to track the number of commits and number of defects.
ncommits=0
ndefects=0

# Print an error message and exit.
abort() {
  echo -e "$@" 1>&2
  exit -1
}

# Prints a diagnostic and increments the defects count. This should be called
# once for each check that fails.
diag() {
  let "ndefects++"
  [[ -n "${QUIET}" ]] || echo -e "  $@" 1>&2
}

# Checks whether a given git revision is valid.
is_valid_rev() {
  git rev-parse -q --verify "$1" 1>/dev/null
}

# Verifies whether ${commit} title is prefixed as stated by the rules.
check_subject_prefix() {
  git log -1 --pretty=%s "${commit}" \
    | grep -q \
        -e '^UPSTREAM:' \
        -e '^FROMLIST:' \
        -e '^BACKPORT:' \
        -e '^RFC:' \
        -e '^ANDROID:' \
        -e '^BRILLO:' \
        -e '^CHROMIUM:' \
        -e '^VENDOR: [A-Za-z0-9]\+:' \
    || diag "Missing or bad commit subject prefix"
}

# Verifies that there's a Bug: line in ${commit}.
check_bug_info() {
  git log -1 --pretty=%B "${commit}" \
    | grep -qi '^Bug[:=]\s*[A-Za-z0-9_:-]\+\(\s\|$\)' \
    || diag "Missing bug annotation"
}

# Verifies that ${commit} declares the patchset name in a Patchset: annotation
# and that it matches the current branch. If there's no current branch, skips
# the check (this is useful to check a detached master branch as checked out by
# repo).
check_patchset() {
  local patchset="$(
    git log -1 --pretty=%B "${commit}" \
      | grep '^Patchset:\s*[A-Za-z0-9_-]\+\(\s\|$\)' \
      | awk '{ print $2 }')"
  if [[ -z "${patchset}" ]]; then
    diag "Missing patchset annotation"
    return
  fi

  local branch="$(git rev-parse --abbrev-ref "$HEAD_REV")"
  if [[ -n "${branch}" ]]; then
    [[ "${branch}" == "${patchset}" ]] \
      || diag "Patchset name ${patchset} doesn't match branch name ${branch}"
  fi
}

# Verifies that ${commit} has a sign-off line for the author.
check_signed_off_by_author() {
  local committer="$(git log -1 --pretty='%cn <%ce>' "${commit}")"
  git log -1 --pretty=%B "${commit}" \
    | grep -q "^Signed-off-by:\s*${committer}\(\s\|$\)" \
    || diag "Signed-off-by line for committer is missing"
}

# Verifies that ${commit} was correctly constructed from reference commits
# appearing in ${REF_HEAD_REV}.
check_commit_provenance() {
  [[ -n "${REF_HEAD_REV}" ]] || return

  local ref_commits=
  for ref_commit in $(git log --reverse --pretty=%h \
                      "${BASE_REV}..${REF_HEAD_REV}"); do
    # See whether ${ref_commit} got rolled into ${commit}. We check whether the
    # commit title appears. If it does, assume the commit got included.
    local ref_title="$(git log -1 --pretty=%s "${ref_commit}")"
    if git log -1 --pretty=%B "${commit}" | grep -q "${ref_title}"; then
      ref_commits+="${ref_commit} "
      continue
    fi
  done

  if [[ -z "${ref_commits}" ]]; then
    diag "Failed to find reference commit"
    return
  fi

  # Check that all signoff lines are retained, output per-commit status if
  # signoff information got dropped.
  local signoff="$(
      git log -1 --pretty=%B "${commit}" | grep "^Signed-off-by:" | sort -u)"
  local ref_signoff="$(echo "${ref_commits}" \
      | xargs -n 1 git log -1 --pretty=%B | grep "^Signed-off-by:" | sort -u)"
  local signoff_diff="$(
      comm -2 -3 <(echo "${ref_signoff}") <(echo "${signoff}"))"
  if [[ -n "${signoff_diff}" ]]; then
    local signoff_diag="Signed-off-by lines lost:\\n"
    signoff_diag+="$(echo "${signoff_diag}" | awk '{ print "    " $0 }')"
    signoff_diag+="\\n  Suggested sign-off lines:\\n"
    sigonff_diag+="$(echo "$ref_commits" | xargs -n 1 git log -1 --pretty=%B \
        | grep "^Signed-off-by:" | awk '!x[$0]++' | awk '{ print "    " $0 }')"
    diag "${signoff_diag}"
  fi
}

usage() {
  cat 1>&2 <<END
Usage: $(basename $0) <options> [<commit>]

Checks commits from the branch head <commit> (or HEAD if not specified) for
compliance with Brillo kernel commit guidelines.

Options include:
  -b <commit>   Specify the base commit to start checking commits from. If this
                is not specified, use the head <commit>'s upstream.
  -h            Print usage information and exit.
  -q            Quiet mode. Do not print diagnostics, but set exit status.
  -r <commit>   A commit to use as a reference to validate checked commits
                against. Makes sure that the resulting diff between the common
                base and <head> vs. the diff between base and reference are
                identical and checks that signoff information got preserved.
END
  exit -2
}

# Parses command line options and stores them in constants.
parse_options() {
  local progname="$(basename $0)"
  while [[ $# -gt 0 ]]; do
    case "$1" in
      -b)
        BASE_REV="$2"
        shift
        ;;
      -h)
        usage
        ;;
      -q)
        QUIET=1
        ;;
      -r)
        REF_HEAD_REV="$2"
        shift
        ;;
      -*)
        abort "Unknown option $1"
        ;;
      *)
        break
        ;;
    esac
    shift
  done

  # Process remaining args, if any.
  HEAD_REV="${1:-HEAD}"
  is_valid_rev "${HEAD_REV}" || abort "${HEAD_REV} is not a valid git rev!"
  readonly HEAD_REV

  if [[ -z "${BASE_REV}" ]]; then
    BASE_REV="$(git rev-parse --abbrev-ref --symbolic-full-name \
                "${HEAD_REV}@{u}")"
  fi
  is_valid_rev "${BASE_REV}" || abort "${BASE_REV} is not a valid git rev!"
  readonly BASE_REV

  if [[ -n "${REF_HEAD_REV}" ]]; then
    is_valid_rev "${REF_HEAD_REV}" \
      || abort "{$REF_HEAD_REV} is not a valid git rev!"
  fi
  readonly REF_HEAD_REV

  readonly QUIET
}

main() {
  parse_options "$@"

  # Go through all commits from base to head revision.
  for commit in $(git log --reverse --pretty=%h "${BASE_REV}..${HEAD_REV}"); do
    let "ncommits++"

    [[ -n "${QUIET}" ]] || git log -1 --oneline "${commit}" 1>&2

    # Check the commit for basic adherence to the rules.
    check_subject_prefix
    check_bug_info
    check_patchset
    check_signed_off_by_author

    # Verify that the commit was correctly derived from reference commit(s).
    check_commit_provenance
  done

  # Check that the final tree matches the reference tree.
  if [[ -n "${REF_HEAD_REV}" ]]; then
    local tree_diff=$(git diff -w "${REF_HEAD_REV}..${HEAD_REV}")
    if [[ -n "$tree_diff" ]]; then
      diag "Reference tree differs:"
      # diag() escapes its arguments and may thus mangle the patch, so emit it
      # directly to standard error instead.
      [[ -n "${QUIET}" ]] || echo "${tree_diff}" \
          | awk '{ print "    " $0 }' 1>&2
    fi
  fi


  # Print results.
  if [[ -z "${QUIET}" ]]; then
    if [[ "${ndefects}" -eq 0 ]]; then
      echo "${ncommits} commits meet basic Brillo kernel style, congrats!"
    else
      echo "${ndefects} defects detected in ${ncommits} commits."
    fi 1>&2
  fi

  exit "${ndefects}"
}

main "$@"
