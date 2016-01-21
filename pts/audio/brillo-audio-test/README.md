Brillo audio test
-----------------

This folder contains tests for libmedia and stagefright on Brillo.

* brillo_audio_test.cpp: Parses input and calls the appropriate test.
* libmedia_playback.cpp: Plays audio (a sine wave) using libmedia.
* libmedia_record.cpp: Records audio using libmedia and saves the result to a
  file.
* stagefright_playback.cpp: Play audio (either sine wave or an MP3 file) using
  stagefright.
* stagefright_record.cpp: Records audio using stagefright and saves the result
  to a file.
