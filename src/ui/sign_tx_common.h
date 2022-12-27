#pragma once

#include "types.h"

#define MAX_NUM_STEPS 13

typedef struct global_item_storage_s {
    // 34 + \0
    char dst_address[35];
    char system_fee[30];
    char network_fee[30];
    char total_fees[30];
    char token_amount[30];
    // 33 bytes public key + \0
    char vote_to[67];
    // Target network the tx in tended for ("MainNet", "TestNet" or uint32 network number for private nets)
    char network[11];
    // uint32 (=max 10 chars) + \0
    char valid_until_block[11];
} global_item_storage_t;

extern global_item_storage_t G_tx;

void format_signer(uint8_t signer_idx, char *dest_title, size_t dest_title_size, char *dest_text, size_t dest_text_size);

void format_account(const signer_t *s, char *dest_title, size_t dest_title_size, char *dest_text, size_t dest_text_size);

void format_scope(const signer_t *s, char *dest_title, size_t dest_title_size, char *dest_text, size_t dest_text_size);

void format_contract(const signer_t *s, uint8_t contract_index, char *dest_title, size_t dest_title_size, char *dest_text, size_t dest_text_size);

void format_group(const signer_t *s, uint8_t group_index, char *dest_title, size_t dest_title_size, char *dest_text, size_t dest_text_size);

int start_sign_tx(void);

void start_sign_tx_ui(void);

