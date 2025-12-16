#ifndef KEY_H
#define KEY_H

#include "stm32f10x.h"
#include <stdint.h>

typedef enum
{
    KEY_S1 = 0, /* PA0 */
    KEY_S2,     /* PA8 */
    KEY_S3,     /* PC9 */
    KEY_S4,     /* PC13 */
    KEY_COUNT
} key_id_t;

void Key_Init(void);
uint8_t Key_IsPressed(key_id_t key);
uint8_t Key_ReadMask(void);

#endif
