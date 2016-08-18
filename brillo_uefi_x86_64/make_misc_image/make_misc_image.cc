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

#include "make_misc_image.h"
#include <base/command_line.h>
#include "base/strings/string_tokenizer.h"
#include "base/strings/string_number_conversions.h"

using namespace base;

static int set_slots_from_tokens(StringTokenizer* st,
                                 BubSlotData (&metadata)[2]) {
  int attribute_value;

  for (int i = 0; i < 2; ++i) {
    if (!st->GetNext()) {
      cerr << "ERROR: Need exactly 6 ab_metadata attribute values.\n";
      return 0;
    } else if (!StringToInt(st->token(), &attribute_value) ||
        attribute_value < 0 || attribute_value > 15) {
      cerr << "ERROR: 0 <= [a,b]_priority <= 15\n";
      return 0;
    } else metadata[i].priority = (uint8_t)attribute_value;

    if (!st->GetNext()) {
      cerr << "ERROR: Need exactly 6 ab_metadata attribute values.\n";
      return 0;
    } else if (!StringToInt(st->token(), &attribute_value) ||
        attribute_value < 0 || attribute_value > 7) {
      cerr << "ERROR: 0 <= [a,b]_tries_remaining <= 7\n";
      return 0;
    } else metadata[i].tries_remaining = (uint8_t)attribute_value;

    if (!st->GetNext()) {
      cerr << "ERROR: Need exactly 6 ab_metadata attribute values.\n";
      return 0;
    } else if (!StringToInt(st->token(), &attribute_value) ||
        attribute_value < 0 || attribute_value > 1) {
      cerr << "ERROR: 0 <= [a,b]_successful_boot <= 1\n";
      return 0;
    } else metadata[i].successful_boot = (uint8_t)attribute_value;
  }

  return 1;
}

int parse_command_line_args(int argc, const char *argv[],
                            BubSlotData (&metadata)[2], FilePath* fname) {
  CommandLine::Reset();
  CommandLine::Init(argc, argv);
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  CommandLine::StringVector args = command_line->GetArgs();

  StringTokenizer
    data_tokens(command_line->GetSwitchValueASCII("ab_metadata"), ",");

  if (!set_slots_from_tokens(&data_tokens, metadata))
    return 0;

  *fname = command_line->GetSwitchValuePath("output");
  if (fname->empty()) {
    cerr << "ERROR: Specify output as --output=/PATH/TO/MISC_IMAGE\n";
    return 0;
  }

  return 1;
}

int make_misc_image(const BubSlotData (&metadata)[2], const FilePath* fname) {
  MyOps ops;
  BubAbData misc_data;

  ops.write_ab_metadata(&misc_data,
                        (uint8_t[4])BUB_BOOT_CTRL_MAGIC,
                        metadata[0].priority,
                        metadata[0].tries_remaining,
                        metadata[0].successful_boot,
                        metadata[1].priority,
                        metadata[1].tries_remaining,
                        metadata[1].successful_boot);
  ops.make_metadata_image(&misc_data, fname->value().c_str());

  return 1;
}
