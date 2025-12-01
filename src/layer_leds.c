#include <zephyr/devicetree.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led.h> // Still required to define struct led_dt_spec
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

// The LED_DT_SPEC_GET macro resolves the parent device pointer and the child index.
static const struct led_dt_spec lower_led = LED_DT_SPEC_GET(DT_NODELABEL(lower_led));
static const struct led_dt_spec raise_led = LED_DT_SPEC_GET(DT_NODELABEL(raise_led));

static int layer_leds_init(void) {

    // ------------------------------------
    // --- Verification: lower_led ---
    // ------------------------------------
    // Check 1: Verify the device pointer itself (checks if the device ordinal exists).
    if (lower_led.dev == NULL) {
        LOG_ERR("LOWER LED FAILED CREATION: Device pointer is NULL. Check DTS/Kconfig linkage.");
        return -EINVAL;
    }
    
    // Check 2: Verify the device is ready and initialized (checks driver initialization).
    if (!device_is_ready(lower_led.dev)) {
        LOG_ERR("LOWER LED PARENT DEVICE NOT READY: %s", lower_led.dev->name);
        return -ENODEV;
    }
    
    LOG_INF("LOWER LED SUCCESS: Parent device '%s' found. Index: %d", 
            lower_led.dev->name, lower_led.index);

    // ------------------------------------
    // --- Verification: raise_led ---
    // ------------------------------------
    if (raise_led.dev == NULL) {
        LOG_ERR("RAISE LED FAILED CREATION: Device pointer is NULL. Check DTS/Kconfig linkage.");
        return -EINVAL;
    }
    if (!device_is_ready(raise_led.dev)) {
        LOG_ERR("RAISE LED PARENT DEVICE NOT READY: %s", raise_led.dev->name);
        return -ENODEV;
    }

    LOG_INF("RAISE LED SUCCESS: Parent device '%s' found. Index: %d", 
            raise_led.dev->name, raise_led.index);

    return 0;
}

SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
