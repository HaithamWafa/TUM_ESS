
#include "VirtualSerial.h"
#include "packetizer.h"
//#include "crypto.h"
#include "randombytes_salsa20XMC_random.h"
#include "sodium.h"

//BOARD ID: 00A7 -> NONCE = OBTAINED FROM Marble Run Webpage by TUM
int main(void) {
  uint8_t ID[16]= {0xc1, 0x80, 0x9, 0x9, 0xb, 0xa0, 0x1b, 0x92, 0x82, 0x6, 0x0, 0x10, 0xa, 0x0, 0x0, 0x0};
  uint8_t key[32] = {0xc1, 0x80, 0x9, 0x9, 0xb, 0xa0, 0x1b, 0x92, 0x82, 0x6, 0x0, 0x10, 0xa, 0x0, 0x0, 0x0,0xc1, 0x80, 0x9, 0x9, 0xb, 0xa0, 0x1b, 0x92, 0x82, 0x6, 0x0, 0x10, 0xa, 0x0, 0x0, 0x0};
  pt_s plaintext = { 0 };
  ct_s ciphertext = { 0 };

  USB_Init();

  randombytes_set_implementation(&randombytes_salsa20XMC_implementation);
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
      ciphertext.text = (uint8_t*)malloc((crypto_secretbox_MACBYTES+plaintext.textLen)*sizeof(uint8_t));
      ciphertext.textLen = (crypto_secretbox_MACBYTES+plaintext.textLen);
      if(crypto_secretbox_easy(ciphertext.text, plaintext.text, plaintext.textLen, plaintext.nonce,
                 key) == 0) {
        //ciphertext.textLen = getCiphertextLength(plaintext.textLen);
        /* Finally, if encryption was successful, send ciphertext back in a packet */
        packetizerSend(&ciphertext);
      }
      /* Free all buffers that were allocated in the meantime */
      free(plaintext.text);
      plaintext.text = NULL;
      free(ciphertext.text);
      ciphertext.text = NULL;
    }
  }
}
