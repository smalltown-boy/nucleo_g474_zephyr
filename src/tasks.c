#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include "tasks.h"

// Для создания новой задачи
#define SIMPLY_TASK_STACK_SIZE 	512							// Размер стека указывается в байтах
#define SIMPLY_TASK_PRIORITY	7							// Номер приоритета

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);				// Регистрируем модуль логирования
K_SEM_DEFINE(button_sem, 0, 1);								// Объявляем бинарный семафор. Начальное значение - 0, максимальное - 1

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);			// Кнопка уже описана в оверлее изначально
static struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)		// Обработчик прерывания
{
    // Отдаем семафор. Это разблокирует задачу просто и безопасно прямо из ISR
    k_sem_give(&button_sem);
}

void simply_task(void *p1, void *p2, void *p3)
{
	// Если аргументы не используются, используем макрос ARG_UNUSED
    ARG_UNUSED(p1); 
	ARG_UNUSED(p2); 
	ARG_UNUSED(p3);

	LOG_INF("[simply_task] has been started.");

    while (1) {
		int err = k_sem_take(&button_sem, K_FOREVER);

		if (err == 0) {
            // Семафор успешно взят (кнопка зажата)
            LOG_INF("[simply_task] Received signal from ISR! Button was pressed.");
        }
		// Тут была задержка, но теперь она не нужна, т.к. процесс ожидания
		// семафора ничего не потребляет, и задача вновь перейдёт в режим
		// ожидания после вывода лога
    }
}

int system_init()
{
	int ret;

	// По заветам бизнес-логики надо чекнуть драйвер
    if (!gpio_is_ready_dt(&button)) {
        LOG_ERR("Error: GPIO device %s is not ready", button.port->name);
        return -ENODEV;
    }

	// Кноака настраивается на вход
	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
        LOG_ERR("Error %d: failed to configure %s pin %d", ret, button.port->name, button.pin);
        return ret;
    }

	// Настраиваем генерацию прерывания при нажатии кнопки
	ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        LOG_ERR("Error %d: failed to configure interrupt on pin %d", ret, button.pin);
        return ret;
    }

	// Привязывем функцию к прерыванию (то есть делем функцию button_pressed)
	// функцией обратного вызова
	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    LOG_INF("GPIO Button initialized successfully.");
    return 0;
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
