#ifndef DEVICE_KEYBOARD_H
#define DEVICE_KEYBOARD_H

/*!
 * 8042 keyboard controller
 */

const int KEYBOARD_BUFFER_SIZE = 256; /* keyboard buffer size (software buffer) */

class Keyboard {
public:
  Keyboard();
  ~Keyboard();

protected:
  // circular buffer
  char buffer[KEYBOARD_BUFFER_SIZE];
  int ptr1, ptr2;
};

extern Keyboard *gKeyboard;

#endif
