/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#include <stdbool.h>  // bool

#include "validate.h"
#include "../menu.h"
#include "../../sw.h"
#include "../../io.h"
#include "../../crypto.h"
#include "../../globals.h"
#include "../../helper/send_response.h"

void ui_action_validate_pubkey(bool approved) {
    PRINTF("ui_action_validate_pubkey\n");
    if (approved) {
        helper_send_response_pubkey();
    } else {
        io_send_sw(SW_DENY);
    }

    ui_menu_main();
}

void ui_action_validate_transaction(bool approved) {
    PRINTF("ui_action_validate_transaction\n");
    if (approved) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_tx() < 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGN_FAIL);
        } else {
            io_send_response(&(const buffer_t){.ptr = G_context.tx_info.signature,
                                               .size = G_context.tx_info.signature_len,
                                               .offset = 0},
                             SW_OK);
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }

    ui_menu_main();
}







// static nbgl_layoutTagValue_t pair;
// static nbgl_layoutTagValueList_t pairList = {0};
// static nbgl_pageInfoLongPress_t infoLongPress;

// #define MAX_TAG_VALUE_PAIRS_DISPLAYED 4
// static actionArgument_t bkp_args[MAX_TAG_VALUE_PAIRS_DISPLAYED];

// static char review_title[20];

// static void transaction_rejected(void) {
//     user_action_tx_cancel();
// }

// static void reject_confirmation(void) {
//     nbgl_useCaseConfirm("Reject transaction?", NULL, "Yes, Reject", "Go back to transaction", transaction_rejected);
// }

// // called when long press button on 3rd page is long-touched or when reject footer is touched
// static void review_choice(bool confirm) {
//     if (confirm) {
//         user_action_single_action_sign_flow_ok();
//     } else {
//         reject_confirmation();
//     }
// }


// // function called by NBGL to get the pair indexed by "index"
// static nbgl_layoutTagValue_t* get_single_action_review_pair(uint8_t index) {
//     if (index == 0) {
//         pair.item = "Contract";
//         pair.value = txContent.contract;
//     } else if (index == 1) {
//         pair.item = "Action";
//         pair.value = txContent.action;
//     } else {
//         // Retrieve action argument, with an index to action args offset
//         printArgument(index - 2, &txProcessingCtx);

//         // Backup action argument as MAX_TAG_VALUE_PAIRS_DISPLAYED can be displayed
//         // simultaneously and their content must be store on app side buffer as
//         // only the buffer pointer is copied by the SDK and not the buffer content.
//         uint8_t bkp_index = index % MAX_TAG_VALUE_PAIRS_DISPLAYED;
//         memcpy(bkp_args[bkp_index].label, txContent.arg.label, sizeof(txContent.arg.label));
//         memcpy(bkp_args[bkp_index].data, txContent.arg.data, sizeof(txContent.arg.data));
//         pair.item = bkp_args[bkp_index].label;
//         pair.value = bkp_args[bkp_index].data;
//     }
//     return &pair;
// }

// static void single_action_review_continue(void) {
//     infoLongPress.icon = &C_stax_app_eos_64px;

//     if (txProcessingCtx.currentActionIndex == txProcessingCtx.currentActionNumer) {
//         infoLongPress.text = "Sign transaction";
//         infoLongPress.longPressText = "Hold to sign";
//     } else {
//         infoLongPress.text = "Accept & review next";
//         infoLongPress.longPressText = "Hold to continue";
//     }

//     pairList.nbMaxLinesForValue = 0;
//     pairList.nbPairs = txContent.argumentCount + 2;
//     pairList.pairs = NULL; // to indicate that callback should be used
//     pairList.callback = get_single_action_review_pair;
//     pairList.startIndex = 0;

//     nbgl_useCaseStaticReview(&pairList, &infoLongPress, "Reject transaction", review_choice);
// }
