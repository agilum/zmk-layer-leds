#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h>

// --- ZMK EVENT HEADERS ---
#include <zmk/zmk.h> // <-- FIX: Added to define core types like zmk_layer_t
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

// Get the device pointer using the Devicetree Node Label
#if DT_NODE_HAS_STATUS(DT_NODELABEL(layer_leds), okay)
static const struct device *layer_leds_dev = DEVICE_DT_GET(DT_NODELABEL(layer_leds));

// Define the two LED indices based on the child nodes in DTS
#define LOWER_LED_INDEX 0
#define RAISE_LED_INDEX 1

#else
static const struct device *layer_leds_dev = NULL;
#define LOWER_LED_INDEX -1
#define RAISE_LED_INDEX -1
#warning "Layer LEDs device not found or not enabled in Devicetree."
#endif

/**
 * @brief Turns a specified LED on or off, setting brightness to 50% when on.
 * @param led_index The index of the LED to control (0 for LOWER, 1 for RAISE).
 * @param on True to set brightness to 50%, False to set brightness to 0%.
 */
static void set_led_state(int led_index, bool on) {
    if (!device_is_ready(layer_leds_dev)) {
        LOG_WRN("LED device not ready, cannot set state.");
        return;
    }
    
    int ret;
    // Set brightness to 50% if 'on' is true, or 0% if 'on' is false.
    uint8_t brightness = on ? 50 : 0; 
    
    ret = led_set_brightness(layer_leds_dev, led_index, brightness);
    
    if (ret != 0) {
        LOG_ERR("Failed to set LED state for index %d (err %d)", led_index, ret);
    } else {
        LOG_DBG("LED %d set to %d%% brightness", led_index, brightness);
    }
}

// --- ZMK LAYER EVENT HANDLER ---

int layer_event_handler(const zmk_event_t *eh) {
    const struct zmk_layer_state_changed *ev = as_zmk_layer_state_changed(eh);
    if (!ev) {
        return -EINVAL;
    }

    // Get the currently active layer ID
    // ZMK layers are 0 (BASE), 1 (LOWER), 2 (RAISE), 3, etc.
    const zmk_layer_t current_layer = zmk_keymap_get_highest_layer();

    LOG_DBG("Layer changed event. Highest active layer: %d", current_layer);

    // Control LOWER LED (Index 0)
    // Turn ON (50% brightness) if layer 1 (LOWER) is active.
    set_led_state(LOWER_LED_INDEX, current_layer == 1);

    // Control RAISE LED (Index 1)
    // Turn ON (50% brightness) if layer 2 (RAISE) is active.
    set_led_state(RAISE_LED_INDEX, current_layer == 2);

    return 0;
}

// Register our handler to listen for layer state changes
ZMK_LISTENER(layer_leds, layer_event_handler);
ZMK_SUBSCRIPTION(layer_leds, zmk_layer_state_changed);


// --- INITIALIZATION ---

static int layer_leds_init(void)
{
    if (!device_is_ready(layer_leds_dev)) {
        LOG_ERR("LED CONTROLLER FAILURE: Device '%s' not ready.", 
                layer_leds_dev ? layer_leds_dev->name : "NULL");
        return -ENODEV;
    }

    LOG_INF("LED CONTROLLER SUCCESS: Device '%s' found and ready.", layer_leds_dev->name);
    
    // Initial state: turn both LEDs OFF (0% brightness)
    set_led_state(LOWER_LED_INDEX, false);
    set_led_state(RAISE_LED_INDEX, false);

    LOG_INF("Layer LED module initialized and listening for layer changes.");

    return 0;
}
// Ensure the module runs after devices are initialized, which we confirmed work
SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
