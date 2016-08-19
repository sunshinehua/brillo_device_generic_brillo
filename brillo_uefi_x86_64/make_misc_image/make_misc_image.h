/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifndef BUB_MISC_IMAGE_H_
#define BUB_MISC_IMAGE_H_

#include <fcntl.h>
#include <gtest/gtest.h>
#include <base/files/file_util.h>

#include <iostream>
#include <string>
#include "../boot_loader/bub_image_util.h"

#define MAX_FILE_NAME_LENGTH 255

using namespace std;

int parse_command_line_args(int argc, const char *argv[],
                            BubSlotData (&metadata)[2], base::FilePath* fname);

int make_misc_image(const BubSlotData (&metadata)[2], const base::FilePath* fname);

#endif /* BUB_MISC_IMAGE_H_ */