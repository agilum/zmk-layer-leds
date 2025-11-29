#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>
#include <zephyr/devicetree.h>  // For DT_PATH
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

// Layer indices (0-based; matches your logs: default=0/logged1, lower=1/logged2, raise=2/logged3)
#define LOWER_LAYER 1
#define RAISE_LAYER 2

static const struct device *lower_led_dev;
static const struct device *raise_led_dev;

static void set_lower_led(bool active) {
    LOG_DBG("Setting Lower LED to %s", active ? "ON" : "OFF");
    if (active) {
        led_set_brightness(lower_led_dev, 0, 100);  // Index 0, full on; adjust for dimming
    } else {
        led_set_brightness(lower_led_dev, 0, 0);    // Index 0, off
    }
}

static void set_raise_led(bool active) {
    LOG_DBG("Setting Raise LED to %s", active ? "ON" : "OFF");
    if (active) {
        led_set_brightness(raise_led_dev, 0, 100);  // Index 0, full on; adjust for dimming
    } else {
        led_set_brightness(raise_led_dev, 0, 0);    // Index 0, off
    }
}

static int handle_layer_state_changed(const zmk_event_t *eh) {
    LOG_DBG("Layer handler triggered");
    if ((eh)->type == &zmk_event_zmk_layer_state_changed) {  // Expanded macro to avoid implicit declaration issue
        bool lower_active = zmk_keymap_layer_active(LOWER_LAYER);
        bool raise_active = zmk_keymap_layer_active(RAISE_LAYER);
        int highest = zmk_keymap_highest_layer_active();
        LOG_DBG("Layer event: highest_active=%d, lower_active=%d, raise_active=%d", highest, lower_active, raise_active);
        set_lower_led(lower_active);
        set_raise_led(raise_active);
    }
    return 0;
}

static int layer_leds_init(void) {
    LOG_DBG("Layer LEDs module init started");

    lower_led_dev = DEVICE_DT_GET(DT_PATH(layer_leds, lower_led));
    if (!device_is_ready(lower_led_dev)) {
        LOG_ERR("Lower LED device not ready");
        return -ENODEV;
    }
    LOG_DBG("Lower LED ready: %p", lower_led_dev);

    raise_led_dev = DEVICE_DT_GET(DT_PATH(layer_leds, raise_led));
    if (!device_is_ready(raise_led_dev)) {
        LOG_ERR("Raise LED device not ready");
        return -ENODEV;
    }
    LOG_DBG("Raise LED ready: %p", raise_led_dev);

    // Test LEDs on boot (lights them for 5s; comment out after confirming hardware works)
    set_lower_led(true);
    set_raise_led(true);
    k_sleep(K_MSEC(5000));
    set_lower_led(false);
    set_raise_led(false);

    LOG_DBG("Layer LEDs init complete");

    return 0;
}

ZMK_LISTENER(layer_leds_listener, handle_layer_state_changed);
ZMK_SUBSCRIPTION(layer_leds_listener, zmk_layer_state_changed);

SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
