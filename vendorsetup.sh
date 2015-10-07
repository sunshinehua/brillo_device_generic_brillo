#
# Copyright 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This file is here to temporarily monkey patch envsetup.sh while the
# Brillo device experience is worked out.  All code in here should
# migrate to build/envsetup.sh over time.

# Execute the contents of any productsetup.sh files we can find.

# TODO(leecam): Remove this as soon as BSP refactor is done.
scan_for_brillo_products()
{
	test -n "$NO_BRILLO" && return
	local f
	for f in `test -d device/generic/brillo && find -L device/generic/brillo -maxdepth 4 -name 'productsetup.sh' 2> /dev/null | sort`;
	do
		echo "including $f"
		. $f
	done
}

scan_for_brillo_products

# TODO(leecam): Move this to /build
scan_for_brillo_devices()
{
  test -n "$NO_BRILLO" && return
  local f
  for f in `test -d device/ && find -L device/ -maxdepth 6 -name 'devicesetup.sh' 2> /dev/null | sort`;
  do
    echo "including $f"
    . $f
  done
}

scan_for_brillo_devices

# build/envsetup.sh defines a make() function that wraps the make command and
# does some extra stats collection to report to users. This function is called
# by all flavors of make; mm, mma, etc.
# We want to hook into these to take our own metrics, but not interfere with the
# functionality, so we re-declare it under a new name, and evoke this original
# behavior in our override (like one might make a call to "super").
eval "overridden_$(declare -f make)"
# TODO(arihc): move all this functionality into brunch build.
make() {
  local start_time=$(date +"%s")
  overridden_make "$@"
  local ret=$?
  local end_time=$(date +"%s")
  local tdiff=$(($end_time-$start_time))

  # Determine which make function this was called from.
  local make_type="${FUNCNAME[1]}"
  if [[ -z "${make_type}" ]]; then
      make_type="make"
  fi

  # Send metrics only if:
  #   * The user has opted in.
  #   * The build was successful.
  #   * The build was non-trivial.
  local T="$(gettop)"
  local BRUNCH_LIB_DIR="${T}/tools/bdk/brunch/lib"
  # TODO(arihc) (b/24410633): Check the configuration file in the brunch dir
  # for opt-in instead of using an environment variable.
  if [[ "${BRILLO_ANALYTICS_OPT_IN}" -ne 0 && "$ret" -eq 0 && \
        "$tdiff" -gt 0 ]]; then
    if [ -z "$PYTHONPATH" ]; then
      export PYTHONPATH=${BRUNCH_LIB_DIR}
    else
      export PYTHONPATH=${BRUNCH_LIB_DIR}:${PYTHONPATH}
    fi
    local data_script="${BRUNCH_LIB_DIR}/metrics/send_build.py"
    (python "${data_script}" "${make_type}" "$tdiff" & )
  fi
}
