#pragma once

#include <stdbool.h>  // bool

/**
 * Callback to reuse action with approve/reject in step FLOW.
 */
typedef void (*action_validate_cb)(bool);

/**
 * Display transaction information on the device and ask confirmation to sign.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_transaction(void);

/**
 * State of the dynamic display flow.
 * Use to keep track of whether we are displaying screens that are inside the
 * UX_FLOW array (dynamic), or outside the array (static).
 */
enum e_state {
    STATIC_SCREEN,
    DYNAMIC_SCREEN,
};

enum e_direction { DIRECTION_FORWARD, DIRECTION_BACKWARD };

extern struct display_ctx_t display_ctx;

bool get_next_data(enum e_direction direction);
void next_prop();
void prev_prop();
