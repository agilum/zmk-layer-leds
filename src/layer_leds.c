#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h>

// --- ZMK HEADERS ---
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

// --- CONFIGURATION ---
// Define the indices of your layers (0-based)
#define LOWER_LAYER_ID 1
#define RAISE_LAYER_ID 2

// Define the desired brightness (0-100) when Caps Lock is ON
#define LED_BRIGHTNESS 5

// Define the LED channel indices (0 and 1) on the controller
#define LOWER_LED_INDEX 0
#define RAISE_LED_INDEX 1

// Get the PARENT device (the controller)
// Assuming your DTS has: layer_leds: layer_leds { ... };
static const struct device *layer_leds_dev = DEVICE_DT_GET(DT_NODELABEL(layer_leds));

// --- HELPER FUNCTION ---
static void update_leds(void) {
    if (!device_is_ready(layer_leds_dev)) {
        return;
    }

    // Check layer state
    bool lower_active = zmk_keymap_layer_active(LOWER_LAYER_ID);
    bool raise_active = zmk_keymap_layer_active(RAISE_LAYER_ID);

    // Set brightness (0 = OFF, 50 = ON at 50% brightness)
    // Note: The second argument is the CHILD INDEX (0 or 1)
    led_set_brightness(layer_leds_dev, LOWER_LED_INDEX, lower_active ? LED_BRIGHTNESS : 0);
    led_set_brightness(layer_leds_dev, RAISE_LED_INDEX, raise_active ? LED_BRIGHTNESS : 0);
    
    LOG_DBG("Lower=%d, Raise=%d", lower_active, raise_active);
}

// --- ZMK EVENT LISTENER ---
static int layer_event_handler(const zmk_event_t *eh) {
    // We only care about layer state changes
    if (as_zmk_layer_state_changed(eh)) {
        update_leds();
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(layer_leds, layer_event_handler);
ZMK_SUBSCRIPTION(layer_leds, zmk_layer_state_changed);

// --- INITIALIZATION ---
static int layer_leds_init(void) {
    if (!device_is_ready(layer_leds_dev)) {
        LOG_ERR("Layer LEDs device '%s' not ready", layer_leds_dev->name);
        return -ENODEV;
    }

    // --- Added Debug Message on Device Ready ---
    LOG_INF("Layer LEDs device ready and initialized.");
    
    // --- Removed Boot Blink Code ---
    // The previous k_sleep(K_MSEC(1000)) block is now removed.
    
    // Initial sync: This sets the LEDs to the current active layer state (likely both off)
    update_leds();

    return 0;
}

// Use APPLICATION priority to ensure ZMK keymap services are ready
SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
