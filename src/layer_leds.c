#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h> // Corrected Include path as suggested by user
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

#define BRIGHTNESS_LEVEL 10  // Adjust this value from 0 (off) to 100 (full brightness)

// Get the device pointer using the Devicetree Node Label
// This relies on the successful DTS structure you created: layer_leds: layer_leds { ... };
#if DT_NODE_HAS_STATUS(DT_NODELABEL(layer_leds), okay)
static const struct device *layer_leds_dev = DEVICE_DT_GET(DT_NODELABEL(layer_leds));

// Define the two LED indices based on the child nodes in DTS
#define LOWER_LED_INDEX 0
#define RAISE_LED_INDEX 1

#else
// If the DTS node is missing or disabled, define a dummy device pointer
static const struct device *layer_leds_dev = NULL;
#define LOWER_LED_INDEX -1
#define RAISE_LED_INDEX -1
#warning "Layer LEDs device not found or not enabled in Devicetree."
#endif

// Layer indices (0-based; adjust if your keymap differs)
#define LOWER_LAYER 1
#define RAISE_LAYER 2

static void set_lower_led(bool active) {
    int ret = led_set_brightness(layer_leds_dev, LOWER_LED_INDEX, active ? BRIGHTNESS_LEVEL : 0);
    if (ret != 0) {
        LOG_ERR("Failed to set LOWER LED brightness (err %d)", ret);
    } else {
        LOG_INF("LOWER LED is now at %d%% brightness.", active ? BRIGHTNESS_LEVEL : 0);
    }
}

static void set_raise_led(bool active) {
    int ret = led_set_brightness(layer_leds_dev, RAISE_LED_INDEX, active ? BRIGHTNESS_LEVEL : 0);
    if (ret != 0) {
        LOG_ERR("Failed to set RAISE LED brightness (err %d)", ret);
    } else {
        LOG_INF("RAISE LED is now at %d%% brightness.", active ? BRIGHTNESS_LEVEL : 0);
    }
}

static int handle_layer_state_changed(const zmk_event_t *eh) {
    if (is_zmk_layer_state_changed(eh)) {
        bool lower_active = zmk_keymap_layer_active(LOWER_LAYER);
        bool raise_active = zmk_keymap_layer_active(RAISE_LAYER);
        set_lower_led(lower_active);
        set_raise_led(raise_active);
    }
    return 0;
}

static int layer_leds_init(void)
{
    if (!device_is_ready(layer_leds_dev)) {
        LOG_ERR("LED CONTROLLER FAILURE: Device '%s' not ready.", 
                layer_leds_dev ? layer_leds_dev->name : "NULL");
        return -ENODEV;
    }

    LOG_INF("LED CONTROLLER SUCCESS: Device '%s' found and ready.", layer_leds_dev->name);
    LOG_INF("Ready to use LOWER LED (Index %d) and RAISE LED (Index %d) at brightness %d%%.", 
            LOWER_LED_INDEX, RAISE_LED_INDEX, BRIGHTNESS_LEVEL);

    // Initial boot state: off
    set_lower_led(false);
    set_raise_led(false);

    return 0;
}

// Ensure the module runs after devices are initialized
SYS_INIT(layer_leds_init, POST_KERNEL, 90);

// Listener for layer changes
ZMK_LISTENER(layer_leds_listener, handle_layer_state_changed);
ZMK_SUBSCRIPTION(layer_leds_listener, zmk_layer_state_changed);
