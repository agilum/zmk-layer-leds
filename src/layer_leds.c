#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>
#include <zephyr/devicetree.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

// Layer indices (0-based; adjust if your keymap differs)
#define LOWER_LAYER 1
#define RAISE_LAYER 2

static const struct device *lower_led_dev = DEVICE_DT_GET(DT_PATH(layer_leds, lower_led));
static const struct device *raise_led_dev = DEVICE_DT_GET(DT_PATH(layer_leds, raise_led));

static void set_lower_led(bool active) {
    if (active) {
        led_set_brightness(lower_led_dev, 0, 100);  // Full on; adjust for dimming
    } else {
        led_set_brightness(lower_led_dev, 0, 0);  // Off
    }
}

static void set_raise_led(bool active) {
    if (active) {
        led_set_brightness(raise_led_dev, 0, 100);  // Full on; adjust for dimming
    } else {
        led_set_brightness(raise_led_dev, 0, 0);  // Off
    }
}

static int handle_layer_state_changed(const zmk_event_t *eh) {
    if ((eh)->type == &zmk_event_zmk_layer_state_changed) {  // Expanded to avoid macro issue
        bool lower_active = zmk_keymap_layer_active(LOWER_LAYER);
        bool raise_active = zmk_keymap_layer_active(RAISE_LAYER);
        LOG_DBG("Layer event: lower_active=%d, raise_active=%d", lower_active, raise_active);
        set_lower_led(lower_active);
        set_raise_led(raise_active);
    }
    return 0;
}

static int layer_leds_init(void) {
    if (!device_is_ready(lower_led_dev)) {
        LOG_ERR("Lower LED device not ready");
        return -ENODEV;
    }
    if (!device_is_ready(raise_led_dev)) {
        LOG_ERR("Raise LED device not ready");
        return -ENODEV;
    }

    // Optional boot test: light for 5s to verify
    set_lower_led(true);
    set_raise_led(true);
    k_sleep(K_MSEC(5000));
    set_lower_led(false);
    set_raise_led(false);

    return 0;
}

ZMK_LISTENER(layer_leds_listener, handle_layer_state_changed);
ZMK_SUBSCRIPTION(layer_leds_listener, zmk_layer_state_changed);

SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
