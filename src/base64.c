#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gprintf.h"

static char const b64a[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz"
                           "0123456789"
                           "+/";

int
main(int argc, char *argv[])
{
  printf("The base64 alphabet is: %s\n", b64a);







  /* You can use the gprintf function provided along with this source code to
   * print debugging messages, which won't show up in your release build.
   */
  gprintf("I received %d arguments\n", argc);
  for (int i = 0; i < argc; ++i) {
    gprintf("Argument %d is %s\n", i, argv[i]);
  }

  assert(0); /* Notice that assertions only affect the debug build; this is
                because the NDEBUG macro is defined in the command-line of the
                release build (-DNDEBUG) */

  puts("Made it to the end! Goodbye :)");
  return EXIT_SUCCESS;
}
