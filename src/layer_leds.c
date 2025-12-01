#include <zephyr/devicetree.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

// Get the device pointer for the parent controller node.
// This is the actual device instance created by the "pwm-leds" driver.
static const struct device *layer_leds_dev = DEVICE_DT_GET(DT_NODELABEL(layer_leds));

// Define the indices for the child LEDs (lower_led is index 0, raise_led is index 1)
#define LOWER_LED_INDEX 0
#define RAISE_LED_INDEX 1

static int layer_leds_init(void) {
    // ------------------------------------
    // --- Verification ---
    // ------------------------------------
    // Check 1: Verify the device pointer itself (checks if the device ordinal exists).
    if (layer_leds_dev == NULL) {
        LOG_ERR("LED CONTROLLER FAILED CREATION: Device pointer is NULL. Check DTS/Kconfig linkage.");
        return -EINVAL;
    }
    
    // Check 2: Verify the device is ready and initialized.
    if (!device_is_ready(layer_leds_dev)) {
        LOG_ERR("LED CONTROLLER '%s' NOT READY.", layer_leds_dev->name);
        return -ENODEV;
    }

    LOG_INF("LED CONTROLLER SUCCESS: Device '%s' found and ready.", layer_leds_dev->name);
    LOG_INF("Ready to use LOWER LED (Index %d) and RAISE LED (Index %d).", 
            LOWER_LED_INDEX, RAISE_LED_INDEX);

    return 0;
}

SYS_INIT(layer_leds_init, POST_KERNEL, 90);
