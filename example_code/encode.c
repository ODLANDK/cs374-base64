/* The purpose of this example is to demonstrate different methods of bitwise
 * manipulation used to perform different types of encoding.
 */

#define _POSIX_C_SOURCE 200809L
#include <limits.h>
#include <stdio.h>

#define ARRAY_LEN(x) (sizeof(x) / sizeof *(x))

#if CHAR_BIT != 8
#error "CHAR_BIT != 8"
#endif

/* Base2 encoding is straightforward; for every byte in the
 * data stream, simply extract each bit from most to
 * least significant and print its value as a '1' or '0'
 */
void
base2encode(char const *data, size_t n)
{
  char const base2a[] = "01"; /* Alphabet */

  for (size_t i = 0; i < n; ++i) {
    unsigned char byte = data[i];
    for (int bit = 0; bit < CHAR_BIT; bit += 1) {
      /* Extract and print most significant bit */
      unsigned char mask = 1u << (CHAR_BIT - 1); /* 0b10000000 */
      int idx = (byte & mask) >> (CHAR_BIT - 1);
      char c = base2a[idx];
      putchar(c);

      /* Left shift 1 bit */
      byte <<= 1;
    }
  }
  putchar('\n');
}

/* Base16 is as simple as Base2, except here we extract four bits
 * (one nibble) at a time. Instead of a loop, it makes more sense
 * to split this into two operations (hi and lo).
 */
#define NIBBLE_BIT (CHAR_BIT / 2) /* A nibble is 4 bits */
void
base16encode(char const *data, size_t n)
{
  static char const base16a[] = "0123456789abcdef"; /* Alphabet */

  for (size_t i = 0; i < n; ++i) {
    unsigned char byte = data[i];
    { /* Extract and print upper nibble */
      int idx = byte >> NIBBLE_BIT;
      char c = base16a[idx];
      putchar(c);
    }
    { /* Extract and print lower nibble */
      int idx = byte & ((1u << NIBBLE_BIT) - 1);
      char c = base16a[idx];
      putchar(c);
    }
  }
  putchar('\n');
}

/* Base8 is much more complex. Why?
 * Log2(2) = 1  -> LCM(1, 8) = 8
 * Log2(8) = 3  -> LCM(3, 8) = 24
 * Log2(16) = 4 -> LCM(4, 8) = 8
 *
 * Notice that the least common multiple of 3 (the number of bits in an octal
 * digit) and 8 (the number of bits in a byte) is 24. We cannot encode one byte
 * at a time, and must instead encode three bytes at a time.
 *
 * What if the input is not a multiple of 3 bytes? The solution is to pad the
 * input with null-bytes to the next largest multiple of 3. When processing the
 * last chunk of three bytes, we remove the octal digits that come from the
 * padding and replace them with a padding character, conventionally '='.
 *
 * For example, {0, 0, 0} encodes as "00000000" in base8,
 * while, {0, 0} encodes as "000000==" in base8,
 * and, {0} encodes as "000=====" in base8.
 */

#define TRIBBLE_BIT 3

/* Aye, sir. Before they went into warp, I transported the whole kit 'n'
 * caboodle into their engine room, where they'll be no tribble at all.
 * - Scotty */

void
base8encode(char const *data, size_t n)
{
  static char const base8a[] = "01234567"; /* Alphabet */
  static char const pad_char = '=';        /* Padding character */

  for (size_t i = 0; i < n; ++i) {
    /* Shift three bytes into a dword */
    int bytes = 1;
    unsigned long dword = data[i];

    /* Second byte */
    dword <<= CHAR_BIT;
    if (++i < n) {
      dword |= data[i];
      ++bytes;
    }

    /* Third byte */
    dword <<= CHAR_BIT;
    if (++i < n) {
      dword |= data[i];
      ++bytes;
    }

    /* Process input, three bits at a time */
    for (int j = 0; j < ((3 * CHAR_BIT) / TRIBBLE_BIT); ++j) {
      if ((j * TRIBBLE_BIT) > (bytes * CHAR_BIT)) {
        putchar(pad_char);
      } else {
        int idx = dword >> (3 * CHAR_BIT - TRIBBLE_BIT);
        char c = base8a[idx];
        putchar(c);
        dword <<= TRIBBLE_BIT; /* Left shift */
        dword &= 0xffffff;     /* Discard upper bits > 24th position */
      }
    }
  }
  putchar('\n');
}

#define SIX_BIT 6
void
base64encode(char const *data, size_t n)
{
  static char const b64a[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz"
                           "0123456789"
                           "+/";
  static char const pad_char = '=';        /* Padding character */

  for (size_t i = 0; i < n; ++i) {
    /* Shift three bytes into a dword */
    int bytes = 1;
    unsigned long dword = data[i];

    /* Second byte */
    dword <<= CHAR_BIT;
    if (++i < n) {
      dword |= data[i];
      ++bytes;
    }

    /* Third byte */
    dword <<= CHAR_BIT;
    if (++i < n) {
      dword |= data[i];
      ++bytes;
    }

    /* Process input, six bits at a time */
    for (int j = 0; j < ((3 * CHAR_BIT) / SIX_BIT); ++j) {
      if ((j * SIX_BIT) > (bytes * CHAR_BIT)) {
        putchar(pad_char);
      } else {
        int idx = dword >> (3 * CHAR_BIT - SIX_BIT);
        char c = b64a[idx];
        putchar(c);
        dword <<= SIX_BIT; /* Left shift */
        dword &= 0xffffff;     /* Discard upper bits > 24th position */
      }
    }
  }
  putchar('\n');
}



/* Let's consider base32 and base64 as well,
 * Log2(32) = 5 -> LCM(5, 8) = 40
 * Log2(64) = 6 -> LCM(6, 8) = 24
 *
 * So for a base32 encoding algorithm, we'd need to process chunks of
 * 5 bytes at a time, while the base64 encoding algorithm only needs
 * to process 3 bytes at a time, similar to the base8 encoding algorithm above.
 *
 * Base64 is relatively simple (similar to the base8 implementation above), and
 * it's efficient: there are only 96 printable ASCII characters, and 64 is the
 * largest power of two within that range, so it is the most space-efficient
 * fast encoding method!
 */

int
main(int argc, char *argv[])
{
  char msg[] = "hello24";
  puts("Base2 encoding:");
  base2encode(msg, ARRAY_LEN(msg));
  puts("Base8 encoding:");
  base8encode(msg, ARRAY_LEN(msg));
  puts("Base16 encoding:");
  base16encode(msg, ARRAY_LEN(msg));
  puts("Base64 encoding:");
  base64encode(msg, ARRAY_LEN(msg));
}
