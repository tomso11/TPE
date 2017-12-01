#ifndef IDT_LOADER_H_
#define IDT_LOADER_H_

#include <stdint.h>

#pragma pack(push)
#pragma pack (1) 		/* Alinear las siguiente estructuras a 1 byte */

/* Descriptor de interrupcion */
typedef struct {
  uint16_t      offset_l,
            selector;
  uint8_t      cero,
            access;
  uint16_t   offset_m;
  uint32_t	    offset_h,
              other_cero;
} DESCR_INT;

/* IDTR  */
typedef struct {
  uint16_t  limit;
  uint64_t base;
} IDTR;

#pragma pack(pop)

#endif
