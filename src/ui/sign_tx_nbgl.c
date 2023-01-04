#include <stdbool.h>  // bool
#include <string.h>   // memset

#include "os.h"
#include "ux.h"
#include "glyphs.h"

#include "constants.h"
#include "../globals.h"
#include "../io.h"
#include "../sw.h"
#include "action/validate.h"
#include "../transaction/types.h"
#include "../common/format.h"
#include "utils.h"
#include "menu.h"
#include "../shared_context.h"
#include "sign_tx_common.h"

#ifdef HAVE_NBGL

#include "nbgl_fonts.h"
#include "nbgl_front.h"
#include "nbgl_debug.h"
#include "nbgl_page.h"
#include "nbgl_use_case.h"

typedef enum item_kind_e {
    SIGNER,
    ACCOUNT,
    SCOPE,
    CONTRACT,
    GROUP,
} item_kind_t;

typedef struct item_signer_s {
    uint8_t signer_index;
} item_signer_t;

typedef struct item_account_s {
    uint8_t signer_index;
} item_account_t;

typedef struct item_scope_s {
    uint8_t signer_index;
} item_scope_t;

typedef struct item_contract_s {
    uint8_t signer_index;
    uint8_t contract_index;
} item_contract_t;

typedef struct item_group_s {
    uint8_t signer_index;
    uint8_t group_index;
} item_group_t;

typedef struct dynamic_item_s {
    item_kind_t kind;
    union {
        item_signer_t as_item_signer;
        item_account_t as_item_account;
        item_scope_t as_item_scope;
        item_contract_t as_item_contract;
        item_group_t as_item_group;
    } content;
} dynamic_item_t;

typedef struct dynamic_slot_s {
    char title[64];
    char text[64];
} dynamic_slot_t;

static void start_review(void);
static void rejectUseCaseChoice(void);
static nbgl_layoutTagValueList_t layout;
static nbgl_layoutTagValue_t current_pair;
static nbgl_layoutTagValue_t static_items[MAX_NUM_STEPS + 1];
static dynamic_slot_t dyn_slots[4];
static uint8_t static_items_nb;
static dynamic_item_t dyn_items[64];
static uint8_t dyn_items_nb;

static int create_transaction_flow(void) {
    static_items_nb = 0;
    dyn_items_nb = 0;

    if (!G_context.tx_info.transaction.is_system_asset_transfer && !G_context.tx_info.transaction.is_vote_script &&
        !N_storage.scriptsAllowed) {
        return -1;
    }

    if (G_context.tx_info.transaction.is_vote_script) {
        if (G_context.tx_info.transaction.is_remove_vote) {
            static_items[static_items_nb].item = "Object";
            static_items[static_items_nb].value = "Retracting vote";
            ++static_items_nb;
        } else {
            static_items[static_items_nb].item = "Object";
            static_items[static_items_nb].value = "Casting vote";
            ++static_items_nb;
        }
    } else if (G_context.tx_info.transaction.is_system_asset_transfer) {
        static_items[static_items_nb].item = "Object";
        static_items[static_items_nb].value = "System asset transfer";
        ++static_items_nb;
        static_items[static_items_nb].item = "Destination addr";
        static_items[static_items_nb].value = G_tx.dst_address;
        ++static_items_nb;
        static_items[static_items_nb].item = "Token amount";
        static_items[static_items_nb].value = G_tx.token_amount;
        ++static_items_nb;
    }

    static_items[static_items_nb].item = "Target network";
    static_items[static_items_nb].value = G_tx.network;
    ++static_items_nb;

    static_items[static_items_nb].item = "System fee";
    static_items[static_items_nb].value = G_tx.system_fee;
    ++static_items_nb;

    static_items[static_items_nb].item = "Network fee";
    static_items[static_items_nb].value = G_tx.network_fee;
    ++static_items_nb;

    static_items[static_items_nb].item = "Total fees";
    static_items[static_items_nb].value = G_tx.total_fees;
    ++static_items_nb;

    static_items[static_items_nb].item = "Valid until height";
    static_items[static_items_nb].value = G_tx.valid_until_block;
    ++static_items_nb;

    for (int i = 0; i < G_context.tx_info.transaction.signers_size; ++i) {
        dyn_items[dyn_items_nb].kind = SIGNER;
        dyn_items[dyn_items_nb].content.as_item_signer.signer_index = i;
        ++dyn_items_nb;
        dyn_items[dyn_items_nb].kind = ACCOUNT;
        dyn_items[dyn_items_nb].content.as_item_account.signer_index = i;
        ++dyn_items_nb;
        dyn_items[dyn_items_nb].kind = SCOPE;
        dyn_items[dyn_items_nb].content.as_item_scope.signer_index = i;
        ++dyn_items_nb;

        signer_t signer = G_context.tx_info.transaction.signers[i];
        for (int j = 0; j < signer.allowed_contracts_size; ++j) {
            dyn_items[dyn_items_nb].kind = CONTRACT;
            dyn_items[dyn_items_nb].content.as_item_contract.signer_index = i;
            dyn_items[dyn_items_nb].content.as_item_contract.contract_index = j;
            ++dyn_items_nb;
        }
        for (int j = 0; j < signer.allowed_groups_size; ++j) {
            dyn_items[dyn_items_nb].kind = GROUP;
            dyn_items[dyn_items_nb].content.as_item_group.signer_index = i;
            dyn_items[dyn_items_nb].content.as_item_group.group_index = j;
            ++dyn_items_nb;
        }
    }

    return 0;
}

static const nbgl_pageInfoLongPress_t review_final_long_press = {
    .text = "Sign message on\nNeo N3 network?",
    .icon = &C_icon_neo_n3_64x64,
    .longPressText = "Hold to sign",
    .longPressToken = 0,
    .tuneId = TUNE_TAP_CASUAL,
};

static void review_final_callback(bool confirmed) {
    if (confirmed) {
        ui_action_validate_transaction(true, false);
        nbgl_useCaseStatus("MESSAGE\nSIGNED", true, ui_menu_main);
    } else {
        rejectUseCaseChoice();
    }
}

static void rejectChoice(void) {
    ui_action_validate_transaction(false, false);
    nbgl_useCaseStatus("message\nrejected", false, ui_menu_main);
}

static void rejectUseCaseChoice(void) {
    nbgl_useCaseConfirm("Reject message?", NULL, "Yes, reject", "Go back to message", rejectChoice);
}

// function called by NBGL to get the current_pair indexed by "index"
static nbgl_layoutTagValue_t *get_single_action_review_pair(uint8_t index) {
    if (index < static_items_nb) {
        // No need to copy to dyn_slots as item and value are pointers to static values
        current_pair.item = static_items[index].item;
        current_pair.value = static_items[index].value;
    } else {
        dynamic_item_t *current_item = &dyn_items[index - static_items_nb];
        signer_t s = G_context.tx_info.transaction.signers[current_item->content.as_item_scope.signer_index];
        dynamic_slot_t *slot = &dyn_slots[index % 4];
        switch (current_item->kind) {
            case SIGNER:
                format_signer(current_item->content.as_item_signer.signer_index,
                              slot->title,
                              sizeof(slot->title),
                              slot->text,
                              sizeof(slot->text));
                break;

            case ACCOUNT:
                format_account(&s, slot->title, sizeof(slot->title), slot->text, sizeof(slot->text));
                break;

            case SCOPE:
                format_scope(&s, slot->title, sizeof(slot->title), slot->text, sizeof(slot->text));
                break;

            case CONTRACT:
                format_contract(&s,
                                current_item->content.as_item_contract.contract_index,
                                slot->title,
                                sizeof(slot->title),
                                slot->text,
                                sizeof(slot->text));
                break;

            case GROUP:
                format_group(&s,
                             current_item->content.as_item_group.group_index,
                             slot->title,
                             sizeof(slot->title),
                             slot->text,
                             sizeof(slot->text));
                break;
        }
        current_pair.item = slot->title;
        current_pair.value = slot->text;
    }
    return &current_pair;
}

static void start_review(void) {
    layout.nbMaxLinesForValue = 0;
    layout.smallCaseForValue = true;
    layout.wrapping = true;
    layout.pairs = NULL;  // to indicate that callback should be used
    layout.callback = get_single_action_review_pair;
    layout.startIndex = 0;
    layout.nbPairs = static_items_nb + dyn_items_nb;

    nbgl_useCaseStaticReview(&layout, &review_final_long_press, "Reject message", review_final_callback);
}

static void arbitrary_script_rejection_callback(bool confirmed) {
    ui_action_validate_transaction(false, false);
    if (confirmed) {
        ui_menu_settings();
    } else {
        ui_menu_main();
    }
}

void start_sign_tx_ui(void) {
    // Prepare steps
    if (create_transaction_flow() != 0) {
        // TODO: maybe add a mechanism to resume the transaction if the user allows the setting
        nbgl_useCaseChoice(
            &C_warning64px,
            "Arbitrary contract\nscripts are not allowed.",
            "Go to Settings menu to enable\nthe signing of such transactions.\n\nThis transaction\nwill be rejected.",
            "Go to Settings menu",
            "Reject transaction",
            arbitrary_script_rejection_callback);
    } else {
        // start display
        nbgl_useCaseReviewStart(&C_icon_neo_n3_64x64,
                                "Review message to\nsign on Neo N3\nnetwork",
                                "",
                                "Reject message",
                                start_review,
                                rejectUseCaseChoice);
    }
}

#endif
