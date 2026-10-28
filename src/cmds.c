#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/gpio.h>
#include <string.h> 

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);		// Из заводского оверлея забираем конфигурацию светодиода

// Регистрация новых команд

static int cmd_my_tasks(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
    ARG_UNUSED(argv);
	// Вот тут хитрость: мы отправляем в шелл на выполнение команду kernel
	// с двумя аргументами
    shell_execute_cmd(sh, "kernel thread list");
    
    return 0; 
}

static int cmd_my_leds(const struct shell *sh, size_t argc, char **argv)
{
	// В любой бизнес-логике нужно проверять, всё ли у нас 
	// инициализировалось
	if (!gpio_is_ready_dt(&led)) {
        shell_error(sh, "GPIO not ready!");
        return -ENODEV;
    }

	// Первый аргумент всегда название команды. Второй - субкоманда.
	// Если общее число аргументов ниже двух, то выводим ошибку.
	if (argc < 2) {
        shell_error(sh, "You lost arguments! Using: leds <on|off>");
        return -EINVAL;
    }

	// Первый аргумент - on (включение)
    if (strcmp(argv[1], "on") == 0) {
        gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
        shell_print(sh, "System led ON");
        return 0;
    } 
    
    // Второй аргумент - off (отключение)
    if (strcmp(argv[1], "off") == 0) {
        gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
        shell_print(sh, "System led OFF");
        return 0;
    }

    // Если пользователь ввел что-то третье, например "leds blink"
    shell_error(sh, "Unknown argument '%s'. Using 'on' or 'off'.", argv[1]);
    return -EINVAL;
}

SHELL_CMD_REGISTER(task, NULL, "Show tasks info.",   cmd_my_tasks);
SHELL_CMD_REGISTER(led,  NULL, "Control system led", cmd_my_leds);
