/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/random/random.h>

/* 

	Broadcast destination number to bollards	
	Listen for reset command from Controller and generate a random number (1-4)

	? Can this just be encoded as the last bit of the BT_UUID_CUSTOM_SERVICE_VAL 
	  Or does it need to be represented as a characteristic of a generic Last Mile service
	?

*/


LOG_MODULE_REGISTER(Last_Mile_Parcel, LOG_LEVEL_INF);


volatile bool last_button = true;

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define RANDOM sys_rand16_get() % 4

/*  Declare the advertising packet */
static const struct bt_data ad[] = {
	/*  Set the advertising flags */
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
	/*  Set the advertising packet data  */
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),

};



/* Declare the URL data to include in the scan response */
unsigned char payload_data[] = { 0x0C, 'B', 'O', 'L', '\0'};

snprintf(payload_data, sizeof(payload_data)+1, "%s%d", payload_data, RANDOM);

/* Declare the scan response packet */
struct bt_data sd[] = {
	/* Include the payload data in the scan response packet */
	BT_DATA(BT_DATA_URI, payload_data, sizeof(payload_data)),
};


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

	LOG_INF("Advertising successfully started\n");	

	
	while(1) {
		k_msleep(100);	
				
		
		
	}
	return 0;
}
