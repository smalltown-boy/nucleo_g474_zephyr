/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include "tasks.h"

// Main - функция, вызываемая из своего отдельного потока
int main(void)
{	
	k_thread_name_set(my_task_id, "simply_task");

	while(1)
	{
		k_sleep(K_MSEC(1000)); 
	}
}
