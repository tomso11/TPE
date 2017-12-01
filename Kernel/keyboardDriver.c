#include "keyboardDriver.h"
#include "keyboardmap.h"
#include "process.h"

#define BUFFSIZE 128
#define TRUE 1
#define FALSE 0
#define ESCAPE 1


char get_key();


static char buffer[BUFFSIZE];
static unsigned int ret_index = 0;
static unsigned int store_index = 0;
static int buff_size = 0;
static int shiftPressed = FALSE;
static int capsLockPressed = FALSE;
static int capsLockPressedCount = 0;
static int capsLockActivated = FALSE;

/* Guarda un scancode en el buffer */
void store_scancode() {
  if (buff_size == BUFFSIZE)
    return;

  int k = get_key(); // Funcion de Assembler que devuelve el scancode

  if (k == ESCAPE) {
    set_foreground_force_process(get_process_by_pid(ppid_process(get_foreground_process())));
    return;
  }

  if (k > 0 && k < TOPCHARCODE) { /* Se apretó una tecla */
    if(!processKeyScanCode(k)) { //caso de un caracter a imprimir en pantalla
      buff_size++;
      buffer[store_index++] = k;  // guarda el scancode de la tecla
      unblock_read_process(get_foreground_process());
    }
  } else if (k < 0) { /*Se solto una tecla */
        processKeyScanCode(k + TOPCHARCODE);
  }

  if (store_index == BUFFSIZE)
    store_index = 0;


}

/* Obtiene la tecla correspondiente segun los flags activados */
int get_char() {
  if (buff_size == 0)
    return -1;
  char k = buffer[ret_index++];
  buff_size--;
  if (ret_index == BUFFSIZE)
    ret_index = 0;
  return chooseKeyboard(k);
}

/* Retorna 1 si el scancode recibido es de Shift */
int isShiftKey(char k) {
 return ((k == LSHIFTCODE) || (k == RSHIFTCODE));
}

/* Retorna TRUE si se toco algun caracter especial como SHIFT, CAPSLOCK y modifica los flags correspondientes*/
int processKeyScanCode(char k){
  switch(k){
    case RSHIFTCODE:
          shiftPressed = !shiftPressed;
          return TRUE;
          break;
    case LSHIFTCODE:
          shiftPressed = !shiftPressed;
          return TRUE;
          break;
    case CAPSLOCK:
          capsLockPressed = !capsLockPressed;
          capsLockPressedCount += 1;
          if(capsLockPressedCount % 2 == 0) { //fue apretado y soltado
            capsLockActivated = !capsLockActivated;
            capsLockPressedCount = 0;
          }
          return TRUE;
          break;
    default:
          return FALSE;
          break;
  }
  return FALSE;

}

/* Retorna el caracter pedido eligiendo el teclado segun los flags que se encuentran activados.
** en caso de no encontrarse el caracter retorna -1 */
int chooseKeyboard(int k) {
  if (kbdus[k] == 0) // no es imprimible
    return -1;
  if(capsLockActivated && !shiftPressed)
    return capskbdus[k];
  if(!capsLockActivated && shiftPressed)
    return shiftkbdus[k];
  if(capsLockActivated && shiftPressed)
    return isAlpha(k) ? kbdus[k] : shiftkbdus[k];
  return kbdus[k];
}

/* Retorna 1 si el scancode recibido equivale a una letra */
int isAlpha(char k) {
  if(( k >= QPOSITION && k <= PPOSITION) || (k >= APOSITION && k <= LPOSITION) || (k >= ZPOSITION && k <= MPOSITION))
    return TRUE;
  return FALSE;
}
