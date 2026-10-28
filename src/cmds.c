#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

// Регистрация новой комманды
static int cmd_my_tasks(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
    	ARG_UNUSED(argv);
	// Вот тут хитрость: мы отправляем в шелл на выполнение команду kernel
	// с двумя аргументами
    	shell_execute_cmd(sh, "kernel thread list");
    
    	return 0; 
}

SHELL_CMD_REGISTER(task, NULL, "Show tasks info.", cmd_my_tasks);
