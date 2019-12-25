#include "power_save.h"
#include "app_timer.h"
#include "keyboard_evt.h"
#include "nrf.h"
#include <stdint.h>

#if LED_AUTOOFF_TIME > 0

static bool counting;
static bool power_save_mode = true;
APP_TIMER_DEF(led_off_timer);

/**
 * @brief 设置省电模式状态
 * 
 * @param on 
 */
void power_save_set_mode(bool on)
{
    if (counting)
        app_timer_stop(led_off_timer);

    power_save_mode = on;
    if (on)
        power_save_reset();
    else
        trig_event_param(USER_EVT_POWERSAVE, PWR_SAVE_EXIT);

    trig_event_param(USER_EVT_POWERSAVE, on ? PWR_SAVE_ON : PWR_SAVE_OFF);
}

/**
 * @brief LED自动关闭的handler
 * 
 * @param context 
 */
static void power_save_timer_handler(void* context)
{
    trig_event_param(USER_EVT_POWERSAVE, PWR_SAVE_ENTER);
    counting = false;
}

/**
 * @brief 启动自动关闭计时器
 * 
 */
void power_save_reset()
{
    if (power_save_mode) {
        if (counting)
            app_timer_stop(led_off_timer);
        app_timer_start(led_off_timer, APP_TIMER_TICKS(LED_AUTOOFF_TIME), NULL);
        counting = true;
    }
    // 无论是否进入这个mode都重置
    trig_event_param(USER_EVT_POWERSAVE, PWR_SAVE_EXIT);
}

/**
 * @brief 初始化计时器
 * 
 */
void power_save_timer_init(void)
{
    app_timer_create(&led_off_timer, APP_TIMER_MODE_SINGLE_SHOT, power_save_timer_handler);
}

static void ps_event_handler(enum user_event event, void* arg)
{
    if (event == USER_EVT_STAGE && (*(uint8_t*)arg) == KBD_STATE_POST_INIT)
        power_save_timer_init();
}

EVENT_HANDLER(ps_event_handler);
#endif