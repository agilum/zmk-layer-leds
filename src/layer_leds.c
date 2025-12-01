#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h> // Include the LED driver header

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

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


static int layer_leds_init(void)
{
    if (!device_is_ready(layer_leds_dev)) {
        LOG_ERR("LED CONTROLLER FAILURE: Device '%s' not ready.", 
                layer_leds_dev ? layer_leds_dev->name : "NULL");
        return -ENODEV;
    }

    LOG_INF("LED CONTROLLER SUCCESS: Device '%s' found and ready.", layer_leds_dev->name);
    LOG_INF("Ready to use LOWER LED (Index %d) and RAISE LED (Index %d).", 
            LOWER_LED_INDEX, RAISE_LED_INDEX);

    // --- NEW CODE STARTS HERE ---
    
    // 1. Turn on the LOWER LED (Index 0)
    int ret_lower = led_on(layer_leds_dev, LOWER_LED_INDEX);
    if (ret_lower != 0) {
        LOG_ERR("Failed to turn on LOWER LED (err %d)", ret_lower);
    } else {
        LOG_INF("LOWER LED is now ON.");
    }

    // 2. Turn on the RAISE LED (Index 1)
    int ret_raise = led_on(layer_leds_dev, RAISE_LED_INDEX);
    if (ret_raise != 0) {
        LOG_ERR("Failed to turn on RAISE LED (err %d)", ret_raise);
    } else {
        LOG_INF("RAISE LED is now ON.");
    }

    // --- NEW CODE ENDS HERE ---

    return 0;
}

// Ensure the module runs after devices are initialized, which we confirmed work
SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
