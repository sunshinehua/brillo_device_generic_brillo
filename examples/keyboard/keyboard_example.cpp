/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

static const int kMaxDeviceName = 80;

// Reads the list of files in |directory_path| into |filenames|.
void ListDirectory(const string& directory_path, vector<string>* filenames) {
  if (!filenames) {
    printf("Error: filenames is null.\n");
    exit(1);
  }
  DIR* directory = opendir(directory_path.c_str());
  if (!directory) {
    printf("Failed to open %s.\n", directory_path.c_str());
    return;
  }
  struct dirent *entry = NULL;
  while ((entry = readdir(directory))) {
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
      filenames->push_back(directory_path + "/" + entry->d_name);
    }
  }
}

// Returns true iff the device reports EV_KEY events.
bool HasKeyEvents(int device_fd) {
  unsigned long evbit = 0;
  // Get the bit field of available event types.
  ioctl(device_fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);
  return evbit & (1 << EV_KEY);
}

// Returns true iff the given device has |key|.
bool HasSpecificKey(int device_fd, unsigned int key) {
  size_t nchar = KEY_MAX/8 + 1;
  unsigned char bits[nchar];
  // Get the bit fields of available keys.
  ioctl(device_fd, EVIOCGBIT(EV_KEY, sizeof(bits)), &bits);
  return bits[key/8] & (1 << (key % 8));
}

int main(int argc, char** argv) {
  string input_directory = "/dev/input";
  if (argc < 2) {
    printf("No input directory specified, using the default one: %s.\n",
           input_directory.c_str());
  } else {
    input_directory = argv[1];
  }

  vector<string> filenames;
  ListDirectory(input_directory, &filenames);
  printf("%i devices found in %s.\n", filenames.size(),
         input_directory.c_str());

  // Initializing the pollfd structures.
  vector<pollfd> poll_fds;
  char device_name[kMaxDeviceName];
  for (size_t i = 0; i < filenames.size(); i++) {
    int fd = open(filenames[i].c_str(), O_RDONLY);
    if (fd < 0) {
      printf("Failed to open %s for reading: %s\n", filenames[i].c_str(),
             strerror(errno));
      return 1;
    }

    // Reads the device name.
    if(ioctl(fd, EVIOCGNAME(sizeof(device_name) - 1),
             &device_name) < 1) {
      device_name[0] = '\0';
    }

    if (!HasKeyEvents(fd)) {
      printf("Discarding %s (%s): does not report any EV_KEY events.\n",
             filenames[i].c_str(), device_name);
      continue;
    }
    if (!HasSpecificKey(fd, KEY_B)) {
      printf("Discarding %s (%s): does not have a B key.\n",
             filenames[i].c_str(), device_name);
      continue;
    }
    poll_fds.push_back(pollfd{fd, POLLIN, 0});

    printf("Adding device %s: %s\n", filenames[i].c_str(), device_name);
  }

  if (poll_fds.empty()) {
    printf("No keyboard detected, exiting.\n");
    return 1;
  }

  // Number of repeated events + 1.
  int count = 1;

  const char instructions[] = "Press B to start the game and release B to go "
      "back to the main menu.";
  printf("%s\n", instructions);
  while (true) {
    // Wait for data to be available on one of the file descriptors without
    // timeout (-1).
    poll(poll_fds.data(), poll_fds.size(), -1);

    for (size_t i = 0; i < poll_fds.size(); i++) {
      if (poll_fds[i].revents & POLLIN) {
        struct input_event event;
        if (read(poll_fds[i].fd, &event, sizeof(event)) != sizeof(event)) {
          printf("Failed to read an event.\n");
          return 1;
        }

        if (event.type != EV_KEY || event.code != KEY_B) {
          continue;
        }

        switch (event.value) {
          case 0: {
            // A value of 0 indicates a "key released" event.
            double percent = 1. - 1./count;
            printf("%.02f%% loaded.\n", percent * 100);
            printf("B released, returning to the main menu.\n\n");
            printf("%s\n", instructions);
            break;
          }
          case 1: {
            // A value of 1 indicates a "key pressed" event.
            printf("B pressed, starting the game...\n");
            count = 1;
            break;
          }
          case 2: {
            // A value of 2 indicates a "key repeat" event.
            count++;
            break;
          }
          default: {}
        }
      }
    }
  }
  return 0;
}

