#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h>

// --- ZMK EVENT HEADERS ---
// Removed the problematic '#include <zmk.h>' line.
// We rely on the event and keymap headers to correctly define zmk_layer_t.
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>
#include <stdint.h> // Include standard integer types

// --- CRITICAL FIX FOR COMPILATION ---
// Since the ZMK headers are failing to define the type zmk_layer_t 
// due to environment specific include path issues, we manually define it.
// zmk_layer_t is typically an alias for a simple unsigned integer.
typedef uint8_t zmk_layer_t;

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

// --- INITIALIZATION ---

static int layer_leds_init(void)
{
    if (!device_is_ready(layer_leds_dev)) {
        LOG_ERR("LED CONTROLLER FAILURE: Device '%s' not ready.", 
                layer_leds_dev ? layer_leds_dev->name : "NULL");
        return -ENODEV;
    }

    LOG_INF("LED CONTROLLER SUCCESS: Device '%s' found and ready.", layer_leds_dev->name);
    
    // TEST: Turn both LEDs ON at 50% brightness immediately on boot
    int ret;

    // Turn on LOWER LED (Index 0)
    ret = led_set_brightness(layer_leds_dev, LOWER_LED_INDEX, 50);
    if (ret != 0) {
        LOG_ERR("Failed to turn on LOWER LED (err %d)", ret);
    } else {
        LOG_INF("LOWER LED turned ON (50%% brightness)");
    }

    // Turn on RAISE LED (Index 1)
    ret = led_set_brightness(layer_leds_dev, RAISE_LED_INDEX, 50);
    if (ret != 0) {
        LOG_ERR("Failed to turn on RAISE LED (err %d)", ret);
    } else {
        LOG_INF("RAISE LED turned ON (50%% brightness)");
    }

    LOG_INF("Layer LED module initialized (Test Mode: Always On).");

    return 0;
}

// Ensure the module runs after devices are initialized
SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
