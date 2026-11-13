/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include "tasks.h"
#include "init_fs.h"

// Main - функция, вызываемая из своего отдельного потока
// Её можно ликвидировать, и функциональность остальных потоков не пострадает
// А можно использовать как родительскую задачу для всех
int main(void)
{	
	system_init();											// Настройка прерывания
	fs_init();												// Инициализация зарезервированного под настройки участка flash-памяти
	k_thread_name_set(my_task_id, "simply_task");

	return 0; 												// Выход из потока
}
