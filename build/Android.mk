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
LOCAL_PATH := $(call my-dir)

# A simple package to include the whitelist file.
include $(CLEAR_VARS)
LOCAL_MODULE := brillo_test_whitelist
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/nativetest
LOCAL_MODULE_STEM := tests.txt
LOCAL_SRC_FILES := ../tests.txt
include $(BUILD_PREBUILT)

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
    site-packages

excludes := $(addprefix --exclude , $(foreach d, $(exclude_dirs), $(d)))
# autotest_server_package should be rebuilt if there is any change on control or python files.
server_package_deps := \
    $(call find-files-in-subdirs,.,'*.py' -and -type f,$(AUTOTEST_DIR) $(EXTRA_AUTOTEST_DIRS)) \
    $(call find-files-in-subdirs,.,'control*' -and -type f,$(AUTOTEST_DIR) $(EXTRA_AUTOTEST_DIRS))

$(autotest_server_package): PRIVATE_AUTOTEST_SERVER_PKG_INTERMEDIATES = $(autotest_server_pkg_intermediates)
$(autotest_server_package): PRIVATE_EXCLUDES := $(excludes)
$(autotest_server_package) : $(server_package_deps)
	@echo "Package autotest server package: $@"
	$(hide) rm -rf $(PRIVATE_AUTOTEST_SERVER_PKG_INTERMEDIATES)/autotest
	$(hide) rsync -r $(PRIVATE_EXCLUDES) $(AUTOTEST_DIR) $(PRIVATE_AUTOTEST_SERVER_PKG_INTERMEDIATES)
	# Copy private autotest code from each project folder to autotest_server_pkg_intermediates
	$(hide) rsync -r $(EXTRA_AUTOTEST_DIRS) $(PRIVATE_AUTOTEST_SERVER_PKG_INTERMEDIATES)
	# Copy autotest server package folder to OUT_DIR. This allows user to run
	# test_droid directly in OUT_DIR/autotest.
	# Before copy autotest files, delete existing files except site-packages
	# folder. This saves user a minute before running test_droid which requires
	# site-packages to be set up first.
	$(hide) find $(OUT_DIR)/autotest -maxdepth 1 ! -name site-packages ! -name autotest -exec rm -rf {} + || true
	$(hide) rsync -r $(PRIVATE_AUTOTEST_SERVER_PKG_INTERMEDIATES)/autotest $(OUT_DIR)/
	$(hide) tar -jcf $@ -C $(dir $@) autotest

$(call dist-for-goals, dist_files, $(autotest_server_package))

# Create a phony target for brillo_autotest_server_package. This allows user to
# run `m dist brillo_autotest_server_package` to only build the autotest server
# package.
.PHONY : brillo_autotest_server_package
brillo_autotest_server_package : $(autotest_server_package)
$(call dist-for-goals,brillo_autotest_server_package,$(autotest_server_package))
# End building autotest server side package.

# Building autotest test_suites package.
# Include all suite control files in autotest repo and EXTRA_AUTOTEST_DIRS.
AUTOTEST_TEST_SUITES := \
    $(call find-files-in-subdirs,.,'control.*', \
        $(AUTOTEST_DIR)/test_suites $(addsuffix /test_suites,$(EXTRA_AUTOTEST_DIRS)))

# The staging directory to store test suites.
test_suites_intermediates := $(call intermediates-dir-for, PACKAGING, test_suites)
target_test_suites_dir := $(test_suites_intermediates)/autotest/test_suites

test_suites_package := $(test_suites_intermediates)/$(name)-test_suites-$(FILE_NAME_TAG).tar.bz2
dependency_info := $(target_test_suites_dir)/dependency_info
suite_to_control_file_map := $(target_test_suites_dir)/suite_to_control_file_map

# Tell control_file_preprocessor.py when we have extra autotest directories.
preprocessor_arg :=
ifneq ($(EXTRA_AUTOTEST_DIRS),)
    preprocessor_arg := -e $(subst $(space),$(comma),$(EXTRA_AUTOTEST_DIRS))
endif

# The recipe does following in order:
# 1. Copy the test suites in AUTOTEST_TEST_SUITES to the intermediate folder.
# 2. Run suite_preprocessor.py to create a dependency_info file, which includes
#    the dependencies for the test suites.
# 3. Run control_file_preprocessor.py to create a file mapping suites to the
#    test control files for each suite.
# 4. tar the autotest intermediate folder as the desired test_suites package.
$(test_suites_package) : PRIVATE_TARGET_TEST_SUITES_DIR := $(target_test_suites_dir)
$(test_suites_package) : PRIVATE_DEPENDENCY_INFO := $(dependency_info)
$(test_suites_package) : PRIVATE_SUITE_TO_CONTROL_FILE_MAP := $(suite_to_control_file_map)
$(test_suites_package) : PRIVATE_CMD_ARG := $(preprocessor_arg)
$(test_suites_package) : $(AUTOTEST_DIR)/site_utils/suite_preprocessor.py
$(test_suites_package) : $(AUTOTEST_DIR)/site_utils/control_file_preprocessor.py
$(test_suites_package) : $(AUTOTEST_TEST_SUITES)
	@echo "Package test suites: $@"
	$(hide) rm -rf $(PRIVATE_TARGET_TEST_SUITES_DIR)
	$(hide) mkdir -p $(PRIVATE_TARGET_TEST_SUITES_DIR)
	$(hide) cp $(AUTOTEST_TEST_SUITES) $(PRIVATE_TARGET_TEST_SUITES_DIR)/
	$(hide) python -B $(AUTOTEST_DIR)/site_utils/suite_preprocessor.py \
		-a $(AUTOTEST_DIR) -o $(PRIVATE_DEPENDENCY_INFO)
	$(hide) python -B $(AUTOTEST_DIR)/site_utils/control_file_preprocessor.py \
		-a $(AUTOTEST_DIR) -o $(PRIVATE_SUITE_TO_CONTROL_FILE_MAP) $(PRIVATE_CMD_ARG)
	$(hide) tar -jcf $@ -C $(dir $@) autotest

$(call dist-for-goals, dist_files, $(test_suites_package))
# End building autotest test_suites package.

# Building autotest_control_files.tar package.
control_files_intermediates := $(call intermediates-dir-for, PACKAGING, control_files)
target_control_files_dir := $(control_files_intermediates)/autotest
control_files_package := $(control_files_intermediates)/$(name)-autotest_control_files-$(FILE_NAME_TAG).tar

control_files := \
    $(foreach f, $(addprefix $(AUTOTEST_DIR)/, \
            $(filter-out test_suites/%,$(call find-files-in-subdirs,$(AUTOTEST_DIR),control* -and -type f,.))), \
        $(eval dest := $(subst $(AUTOTEST_DIR),$(target_control_files_dir),$(f))) \
        $(eval $(call copy-one-file, $(f), $(dest))) \
        $(dest))

# Include control files in EXTRA_AUTOTEST_DIRS's server/tests and server/site_tests folders.
# Only server side tests are supported for now.
control_files += \
    $(foreach d, $(EXTRA_AUTOTEST_DIRS), \
        $(foreach tests_dir, $(d)/server/tests $(d)/server/site_tests, \
            $(foreach f, $(addprefix $(tests_dir)/, \
                    $(filter-out test_suites/%,$(call find-files-in-subdirs,$(tests_dir),control* -and -type f,.))), \
                $(eval dest := $(subst $(d),$(target_control_files_dir),$(f))) \
                $(eval $(call copy-one-file, $(f), $(dest))) \
                $(dest))))

$(control_files_package) : $(control_files)
	@echo "Package control files: $@"
	$(hide) tar -cf $@ -C $(dir $@) autotest

$(call dist-for-goals, dist_files, $(control_files_package))
# End building autotest_control_files.tar package.

