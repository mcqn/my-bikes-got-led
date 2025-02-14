/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>


//See : https://github.com/fduignan/zephyr_bbc_microbit_v2/tree/main
#include "matrix.h"
#include "buttons.h"


/*

Manages the game running with button presses

    A Set the game: Broadcast to all bollards to enter waiting mode
    B Check: Broadcast to all the bollards to check status and display correct/incorrect indicator
    A+B Reset Broadcast to all the Parcels to generate a random destination number 1-4 and broadcast to bollards to enter waiting mode


*/


LOG_MODULE_REGISTER(Last_Mile_Controller, LOG_LEVEL_INF);


#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)


/*  Declare the advertising packet */
static const struct bt_data ad[] = {
	/*  Set the advertising flags */
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
	/*  Set the advertising packet data  */
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),

};

/* Declare the URL data to include in the scan response */
static unsigned char payload_data[] = { 0x0C, 'C', 'H', 'E', 'C', 'K' };

/* Declare the scan response packet */
static const struct bt_data sd[] = {
	/* Include the payload data in the scan response packet */
	BT_DATA(BT_DATA_URI, payload_data, sizeof(payload_data)),
};


void button_a_callback(void)
{

	if(get_buttonB()){
		matrix_put_pattern(0x1f,0);
	} else {
		matrix_put_pattern(0,0);
	}

}

void button_b_callback(void)
{
	if(get_buttonA()){
		matrix_put_pattern(0xaa,0xaa);
	} else {
		matrix_put_pattern(0,0);
	}
}


int main(void)
{

	int err;
	/* Enable the Bluetooth LE stack */
	err = bt_enable(NULL);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)\n", err);
		return -1;
	}
        settings_subsys_init();
	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	LOG_INF("Bluetooth initialized\n");
	


	/*  Start advertising */
	err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)\n", err);
		return -1;
	}

	// Initialise microbit io
	int ret;
  	ret = buttons_begin();
  	if (ret < 0)
  	{
		printk("Error initializing the buttons\n");
		return -1;
	}
	ret = matrix_begin();
	if (ret < 0)
  	{
		printk("Error initializing the matrix\n");
		return -2;
	}
	attach_callback_to_button_a(button_a_callback);
	attach_callback_to_button_b(button_b_callback);

	LOG_INF("Advertising successfully started\n");	
	while(1) {
		LOG_INF("Advertising\n");
	
	}
	return 0;
}
