#pragma once

#include <stdint.h>  // uint*_t
#include <stdbool.h>

#include "os.h"
#include "cx.h"

#ifdef HAVE_NBGL
#include "nbgl_page.h"
#endif

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

bool address_from_pubkey(const uint8_t public_key[static 64], char* out, size_t out_len);

void script_hash_to_address(char* out, size_t out_len, const unsigned char* script_hash);

#ifdef HAVE_NBGL
extern nbgl_page_t* pageContext;
void releaseContext(void);
#endif
