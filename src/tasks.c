#include <zephyr/logging/log.h>
#include "tasks.h"

// Для создания новой задачи
#define SIMPLY_TASK_STACK_SIZE 	512							// Размер стека указывается в байтах
#define SIMPLY_TASK_PRIORITY	7							// Номер приоритета

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);

void simply_task(void *p1, void *p2, void *p3)
{
	// Если аргументы не используются, используем макрос ARG_UNUSED
    ARG_UNUSED(p1); 
	ARG_UNUSED(p2); 
	ARG_UNUSED(p3);

	LOG_INF("[simply_task] has been started.");

    while (1) {
		LOG_INF("[simply_task] is running!");
        k_sleep(K_MSEC(4000)); // Как и во FreeRTOS - обязательно даём другим задачам поработать
    }
}

// Объявляем задачу с обозначенным ранее стеком и приоритетом
// Обратите внимание, что это СТАТИЧЕСКИЙ способ инициалиции, т.е.
// ДО компиляции
K_THREAD_DEFINE(my_task_id, 
				SIMPLY_TASK_STACK_SIZE, 
				simply_task, 
				NULL, 
				NULL, 
				NULL, 
				SIMPLY_TASK_PRIORITY, 
				0, 
				0);
