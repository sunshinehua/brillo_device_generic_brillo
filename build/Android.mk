#
# Copyright (C) 2015 The Android Open Source Project
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

# -----------------------------------------------------------------

# Include the peripheral HALs
$(foreach f,$(HAL_MAKEFILES), \
    $(if $(wildcard $(f)),$(eval include $(f))))

ifneq "" "$(filter eng.%,$(BUILD_NUMBER))"
  # BUILD_NUMBER has a timestamp in it, which means that
  # it will change every time.  Pick a stable value.
  FILE_NAME_TAG := eng.$(USER)
else
  FILE_NAME_TAG := $(BUILD_NUMBER)
endif

name := $(TARGET_PRODUCT)
ifeq ($(TARGET_BUILD_TYPE),debug)
  name := $(name)_debug
endif

# The vendor partition package for brillo device.
ifdef BRILLO_VENDOR_PARTITIONS

# The staging directory to store vendor partitions.
intermediates := $(call intermediates-dir-for, PACKAGING, vendor-partitions)

vendor_partition_zip := $(intermediates)/$(name)-vendor_partitions-$(FILE_NAME_TAG).zip

# Set up rules to copy all the files to staging directory, and gather the paths of the dest files.
# BRILLO_VENDOR_PARTITIONS contains a list of strings in the format of parent_directory:file_path.
# Each vendor partition's full path is parent_directory/file_path. The directory structure in
# file_path will be preserved.
vendor_partition_copied_files := \
	$(foreach f, $(BRILLO_VENDOR_PARTITIONS) $(PRODUCT_OUT):provision-device, \
		$(eval pair := $(subst :,$(space),$(f))) \
		$(eval src := $(word 1, $(pair))/$(word 2, $(pair))) \
		$(eval dest := $(intermediates)/$(word 2, $(pair))) \
		$(eval $(call copy-one-file, $(src), $(dest))) \
		$(dest))

$(vendor_partition_zip) : $(vendor_partition_copied_files)
	@echo "Package vendor partitions: $@"
	$(hide) cd $(dir $@) && zip -qr $(notdir $@) *

$(call dist-for-goals, dist_files, $(vendor_partition_zip))

endif

# Build autotest server side package.
AUTOTEST_DIR := external/autotest
autotest_server_pkg_intermediates := $(call intermediates-dir-for, PACKAGING, autotest_server_package)
autotest_server_package := $(autotest_server_pkg_intermediates)/$(name)-autotest_server_package-$(FILE_NAME_TAG).tar.bz2

# Exclude files not needed for autotest server-side packaging. The list of files
# is different from what's used in building autotest server package in ChromeOS,
# refer to https://chromium.googlesource.com/chromiumos/chromite/+/master/cbuildbot/commands.py#1997
# The reason is that ChromeOS builder processes the autotest build output, which
# does not include files not included in autotest ebuilds. Brillo builder needs
# to filter the files from autotest repo, thus more excludes are needed here.
exclude_dirs := .git \
    apache \
    cli \
    client/build_deps \
    client/deps \
    client/profilers/*/*.tar.bz2 \
    client/profilers/*/*.tar.gz \
    client/site_tests \
    client/tests \
    contrib \
    database \
    docs \
    logs \
    packages \
    puppylab \
    results \
    scheduler \
    server/site_tests/native_Benchmarks \
    site-packages \
    tko

excludes := $(addprefix --exclude , $(foreach d, $(exclude_dirs), $(d)))

$(autotest_server_package) :
	@echo "Package autotest server package: $@"
	$(hide) rm -rf $(autotest_server_pkg_intermediates)/autotest
	$(hide) rsync -r $(excludes) $(AUTOTEST_DIR) $(autotest_server_pkg_intermediates)
	$(hide) tar -jcf $@ -C $(dir $@) autotest

$(call dist-for-goals, dist_files, $(autotest_server_package))
# End building autotest server side package.

# Building autotest test_suites package.
ifndef AUTOTEST_TEST_SUITES
# If AUTOTEST_TEST_SUITES is not set in board config, include all suite control
# files in autotest repo.
TEST_SUITES_DIR := $(AUTOTEST_DIR)/test_suites
AUTOTEST_TEST_SUITES := $(addprefix $(TEST_SUITES_DIR)/, $(call find-files-in-subdirs,$(TEST_SUITES_DIR),control.*,.))
else  # AUTOTEST_TEST_SUITES is defined
@echo "AUTOTEST_TEST_SUITES is already assigned:"
@echo ${AUTOTEST_TEST_SUITES}
endif  # ifndef AUTOTEST_TEST_SUITES

# The staging directory to store test suites.
test_suites_intermediates := $(call intermediates-dir-for, PACKAGING, test_suites)
target_test_suites_dir := $(test_suites_intermediates)/autotest/test_suites

test_suites_package := $(test_suites_intermediates)/$(name)-test_suites-$(FILE_NAME_TAG).tar.bz2
dependency_info := $(target_test_suites_dir)/dependency_info
suite_to_control_file_map := $(target_test_suites_dir)/suite_to_control_file_map

# Trigger rebuilding the test_suites package when a control file is add or
# removed from the directory.
$(test_suites_package) : $(TEST_SUITES_DIR)

# Note that the test suites tar ball won't be regenerated if the depended
# AUTOTEST_TEST_SUITES is not changed. For example, change in autotest module
# control_file_preprocessor.py will not lead to the rebuild of the test suites
# tar ball.
# The recipe does following in order:
# 1. Copy the test suites in AUTOTEST_TEST_SUITES to the intermediate folder.
# 2. Run suite_preprocessor.py to create a dependency_info file, which includes
#    the dependencies for the test suites.
# 3. Run control_file_preprocessor.py to create a file mapping suites to the
#    test control files for each suite.
# 4. tar the autotest intermediate folder as the desired test_suites package.
$(test_suites_package) : $(AUTOTEST_TEST_SUITES)
	@echo "Package test suites: $@"
	$(hide) rm -rf $(target_test_suites_dir)
	$(hide) mkdir -p $(target_test_suites_dir)
	$(foreach f, $(AUTOTEST_TEST_SUITES), \
		$(shell cp $(f) $(target_test_suites_dir)/))
	$(hide) python -B $(AUTOTEST_DIR)/site_utils/suite_preprocessor.py \
		-a $(AUTOTEST_DIR) -o $(dependency_info)
	$(hide) python -B $(AUTOTEST_DIR)/site_utils/control_file_preprocessor.py \
		-a $(AUTOTEST_DIR) -o $(suite_to_control_file_map)
	$(hide) tar -jcf $@ -C $(dir $@) autotest

$(call dist-for-goals, dist_files, $(test_suites_package))
# End building autotest test_suites package.
