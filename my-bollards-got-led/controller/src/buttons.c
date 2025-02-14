#include <stdint.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include "buttons.h"
// Both buttons are on GPIO0
#define BUTTON_A_PORT_BIT 14
#define BUTTON_B_PORT_BIT 23
static const struct device *gpio_0;


int get_buttonA()
{
	return gpio_pin_get(gpio_0,BUTTON_A_PORT_BIT);
}
int get_buttonB()
{
	return gpio_pin_get(gpio_0,BUTTON_B_PORT_BIT);
}
int buttons_begin()
{
	int ret;
	// Configure the GPIO's 	
	gpio_0=DEVICE_DT_GET(DT_NODELABEL(gpio0));
	if (gpio_0 == NULL)
	{
		printf("Error acquiring GPIO 0 interface\n");
		return -1;
	}
	ret = gpio_pin_configure(gpio_0,BUTTON_A_PORT_BIT,GPIO_INPUT);
	ret = gpio_pin_configure(gpio_0,BUTTON_B_PORT_BIT,GPIO_INPUT);
	return 0;

}

static fptr button_a_user_handler = NULL;
static struct gpio_callback button_a_cb;
static void button_a_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("interrupt\n");
	if (button_a_user_handler)
	{
		button_a_user_handler();
	}
}
int attach_callback_to_button_a(fptr callback_function)
{
	if (gpio_pin_interrupt_configure(gpio_0,BUTTON_A_PORT_BIT,GPIO_INT_EDGE_FALLING) < 0)
	{
		printk("Error configuring interrupt for button A\n");
	}
	gpio_init_callback(&button_a_cb, button_a_handler, (1 << BUTTON_A_PORT_BIT) );	
    if (gpio_add_callback(gpio_0, &button_a_cb) < 0)
	{
		printk("Error adding callback for button A\n");
	}
	else
	{
		// success so far so use the user supplied callback function
		button_a_user_handler = callback_function;
	}

	return 0;
}


//TIDYME

static fptr button_b_user_handler = NULL;
static struct gpio_callback button_b_cb;
static void button_b_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("interrupt\n");
	if (button_b_user_handler)
	{
		button_b_user_handler();
	}
}
int attach_callback_to_button_b(fptr callback_function)
{
	if (gpio_pin_interrupt_configure(gpio_0,BUTTON_B_PORT_BIT,GPIO_INT_EDGE_FALLING) < 0)
	{
		printk("Error configuring interrupt for button B\n");
	}
	gpio_init_callback(&button_b_cb, button_b_handler, (1 << BUTTON_B_PORT_BIT) );	
    if (gpio_add_callback(gpio_0, &button_b_cb) < 0)
	{
		printk("Error adding callback for button B\n");
	}
	else
	{
		// success so far so use the user supplied callback function
		button_b_user_handler = callback_function;
	}

	return 0;
}



