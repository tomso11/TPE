#ifndef __KEYBOARD__DRIVER__H__
#define __KEYBOARD__DRIVER__H__

int get_char();
void store_scancode();
int has_key();
int key_pressed();
int isShiftKey(char k);
int processKeyScanCode(char k);
int chooseKeyboard(int k);
int isAlpha(char k);

#endif
