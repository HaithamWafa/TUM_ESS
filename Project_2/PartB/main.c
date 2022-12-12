/* Main source file for assignment 2 part A, which implements shitty homebrewn crypto together with
 * a nasty little stack based buffer overflow
 *
 * Copyright 2018 TUM
 * Created: 2018-11-15 Florian Wilde <florian.wilde@tum.de>
 **************************************************************************************************/

#include "VirtualSerial.h"
#include "packetizer.h"
#include "crypto.h"
#include "blink.h"
#include "sodium.h"

int main(void) {
  //uint8_t key[8] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42};
  uint8_t chip_ID[16]= {0xc1, 0x80, 0x8d, 0xb, 0x6, 0x20, 0x4, 0x59, 0x82, 0x6, 0x0, 0x10, 0xa, 0x0, 0x0, 0x0};
  uint16_t key[32] = {0xc1, 0x80, 0x8d, 0xb, 0x6, 0x20, 0x4, 0x59, 0x82, 0x6, 0x0, 0x10, 0xa, 0x0, 0x0, 0x0,0xc1, 0x80, 0x8d, 0xb, 0x6, 0x20, 0x4, 0x59, 0x82, 0x6, 0x0, 0x10, 0xa, 0x0, 0x0, 0x0}
  pt_s plaintext = { 0 };
  ct_s ciphertext = { 0 };
  int blinkCode = 0;

  USB_Init();
  sodium_init();

  while(1) {
    /* Consumes incoming data and parses it until we received a valid packet.
       If so, it will fill the plaintextJob struct with the appropriate information. */
    if(packetizerReceive(&plaintext) == 0) {
      /* If the sender did not sent a nonce, generate one */
      if(plaintext.nonce == NULL) {
        plaintext.nonce = malloc(CRYPTO_NONCE_BYTES);
        for(size_t i = 0; i < CRYPTO_NONCE_BYTES; i++) {
          plaintext.nonce[i] = rand() & UINT8_MAX;
        }
      }
      /* Now encrypt the plaintext */
      if(crypto_secretbox_easy(&ciphertext.text, plaintext.text, plaintext.textLen, plaintext.nonce,
                 key) == 0) {
        ciphertext.textLen = getCiphertextLength(plaintext.textLen);
        /* Finally, if encryption was successful, send ciphertext back in a packet */
        packetizerSend(&ciphertext);
      }
      /* Free all buffers that were allocated in the meantime */
      free(plaintext.text);
      plaintext.text = NULL;
      free(ciphertext.text);
      ciphertext.text = NULL;
    }
    /* To detect traffic, enable the blink function if a button is pressed */
    if(getButtonEvent() != noButton) {
      blinkCode ^= BLINKENLIGHTS;
      blink(blinkCode);
    }
  }
}
