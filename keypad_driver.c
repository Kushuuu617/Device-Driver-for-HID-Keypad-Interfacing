#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/mutex.h>

#define DRIVER_NAME "keypad_driver"
#define ROWS 4
#define COLS 4

// BCM GPIO numbers (adjust if your wiring is different)
static const unsigned int row_gpios[ROWS] = {17, 5, 6, 13};
static const unsigned int col_gpios[COLS] = {23, 24, 12, 16};

// Key mapping: Linux input keycodes
static const unsigned short keymap[ROWS][COLS] = {
    {KEY_1, KEY_2, KEY_3, KEY_A}, // KEY_A = 'A' on PC keyboard
    {KEY_4, KEY_5, KEY_6, KEY_B},
    {KEY_7, KEY_8, KEY_9, KEY_C},
    {KEY_BACKSPACE, KEY_0, KEY_KPENTER, KEY_D} // * 0 # D
};

static struct input_dev *keypad_input;
static struct timer_list scan_timer;
static struct work_struct scan_work;
static DEFINE_MUTEX(scan_mutex);

// Bookkeeping for key state
static bool key_state[ROWS][COLS] = {{false}};

// Forward declaration
static void keypad_scan(struct work_struct *work);

// GPIO setup/cleanup
static int keypad_gpio_init(void)
{
    int i, ret;
    // Request and configure row pins as output, default LOW
    for (i = 0; i < ROWS; i++)
    {
        ret = gpio_request(row_gpios[i], DRIVER_NAME "_row");
        if (ret)
            goto err_row;
        gpio_direction_output(row_gpios[i], 0);
    }
    // Request and configure column pins as input
    for (i = 0; i < COLS; i++)
    {
        ret = gpio_request(col_gpios[i], DRIVER_NAME "_col");
        if (ret)
            goto err_col;
        gpio_direction_input(col_gpios[i]);
        // Pull-downs should be set in device tree or with external resistors
    }
    return 0;
err_col:
    while (--i >= 0)
        gpio_free(col_gpios[i]);
    for (i = 0; i < ROWS; i++)
        gpio_free(row_gpios[i]);
    return ret;
err_row:
    while (--i >= 0)
        gpio_free(row_gpios[i]);
    return ret;
}

static void keypad_gpio_cleanup(void)
{
    int i;
    for (i = 0; i < ROWS; i++)
        gpio_free(row_gpios[i]);
    for (i = 0; i < COLS; i++)
        gpio_free(col_gpios[i]);
}

// Keypad scanning work
static void keypad_scan(struct work_struct *work)
{
    int row, col, val;
    mutex_lock(&scan_mutex);
    for (row = 0; row < ROWS; row++)
    {
        // Set all rows LOW
        int i;
        for (i = 0; i < ROWS; i++)
            gpio_set_value(row_gpios[i], 0);
        // Set current row HIGH
        gpio_set_value(row_gpios[row], 1);
        udelay(10); // Allow signal to settle
        // Scan columns
        for (col = 0; col < COLS; col++)
        {
            val = gpio_get_value(col_gpios[col]);
            if (val && !key_state[row][col])
            {
                // Key pressed
                input_report_key(keypad_input, keymap[row][col], 1);
                input_sync(keypad_input);
                key_state[row][col] = true;
            }
            else if (!val && key_state[row][col])
            {
                // Key released
                input_report_key(keypad_input, keymap[row][col], 0);
                input_sync(keypad_input);
                key_state[row][col] = false;
            }
        }
    }
    mutex_unlock(&scan_mutex);
    // Re-arm timer for next scan
    mod_timer(&scan_timer, jiffies + msecs_to_jiffies(30));
}

// Timer callback schedules the scan work
static void scan_timer_callback(struct timer_list *t)
{
    schedule_work(&scan_work);
}

// Input device registration
static int keypad_input_init(void)
{
    int i, j, err;
    keypad_input = input_allocate_device();
    if (!keypad_input)
        return -ENOMEM;
    keypad_input->name = "Raspi keypad";
    keypad_input->phys = "keypad/input0";
    keypad_input->id.bustype = BUS_HOST;
    keypad_input->evbit[0] = BIT_MASK(EV_KEY);
    // Register all keys in keymap
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++)
            __set_bit(keymap[i][j], keypad_input->keybit);
    err = input_register_device(keypad_input);
    if (err)
    {
        input_free_device(keypad_input);
        return err;
    }
    return 0;
}

static void keypad_input_cleanup(void)
{
    if (keypad_input)
        input_unregister_device(keypad_input);
}

static int __init keypad_driver_init(void)
{
    int err;
    mutex_init(&scan_mutex);
    err = keypad_gpio_init();
    if (err)
        return err;
    err = keypad_input_init();
    if (err)
        goto err_input;
    INIT_WORK(&scan_work, keypad_scan);
    timer_setup(&scan_timer, scan_timer_callback, 0);
    mod_timer(&scan_timer, jiffies + msecs_to_jiffies(100));
    pr_info(DRIVER_NAME ": loaded\n");
    return 0;
err_input:
    keypad_gpio_cleanup();
    return err;
}

static void __exit keypad_driver_exit(void)
{
    del_timer_sync(&scan_timer);
    cancel_work_sync(&scan_work);
    keypad_input_cleanup();
    keypad_gpio_cleanup();
    pr_info(DRIVER_NAME ": unloaded\n");
}

module_init(keypad_driver_init);
module_exit(keypad_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Atul M");
MODULE_DESCRIPTION("Keypad driver for RasPi");