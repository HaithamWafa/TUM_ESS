#include <stdio.h>
int main(void) {
  
/*asm("mov sp, pc");
asm("movw r2, #0x8101");
asm("movt r2, #0x4802");
asm("movs r1, #0x80");
asm("strb r1, [r2, #0xf]");
asm("lsrs r1, r1, #7");
asm("strb r1, [r2, #3]");
asm("b #0x12");
*/
int r1 = 0;
int r2 = 0;

// Initialize stack pointer
asm("mov sp, pc" : : : );

// Initialize R2 register
asm("movw r2, #0x8101" : : : "r2");
asm("movt r2, #0x4802" : : : "r2");

// Initialize R1 register and store value in memory
asm("movs r1, #0x80" : : : "r1");
asm("strb r1, [r2, #0xf]" : : : "r1", "r2");

// Shift value in R1 register and store in memory
asm("lsrs r1, r1, #7" : : : "r1");
asm("strb r1, [r2, #3]" : : : "r1", "r2");

// Unconditional branch to 0x12
asm("b #0x12" : : : );

printf("r1 = 0x%x, r2 = 0x%x\n", r1, r2);
return 0;
  
}
