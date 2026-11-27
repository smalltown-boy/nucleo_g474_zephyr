#include "tasks.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/fs/fs.h>
#include <string.h>
#include <zephyr/dfu/mcuboot.h>

#define CONFIG_FILE "/lfs/config.json"

void check_physical_flash(void)
{
    /* Берём устройство по метке w25q_flash из app.overlay */
    const struct device *flash_dev = DEVICE_DT_GET(DT_NODELABEL(w25q_flash));
    uint64_t size_bytes = 0;
    uint8_t jedec_id[3] = {0};

    if (!device_is_ready(flash_dev)) {
        printk("Ошибка: драйвер w25q_flash не готов!\n");
        return;
    }

    /* Считываем аппаратный JEDEC ID напрямую из микросхемы */
    if (flash_read_jedec_id(flash_dev, jedec_id) == 0) {
        printk("JEDEC ID: [0x%02X 0x%02X 0x%02X]\n",
               jedec_id[0], jedec_id[1], jedec_id[2]);

        /* Расшифровка последнего байта объёма для Winbond W25Qxx */
        switch (jedec_id[2]) {
            case 0x14: printk("Модель: W25Q80 (8 Мбит / 1 МБ)\n");              break;
            case 0x15: printk("Модель: W25Q16 (16 Мбит / 2 МБ)\n");             break;
            case 0x16: printk("Модель: W25Q32 (32 Мбит / 4 МБ)\n");             break;
            case 0x17: printk("Модель: W25Q64 (64 Мбит / 8 МБ)\n");             break;
            case 0x18: printk("Модель: W25Q128 (128 Мбит / 16 МБ)\n");          break;
            default:   printk("Неизвестный код объёма: 0x%02X\n", jedec_id[2]); break;
        }
    } else {
        printk("Не удалось прочитать JEDEC ID\n");
    }

    /* Получаем физический размер чипа от драйвера */
    if (flash_get_size(flash_dev, &size_bytes) == 0) {
        printk("Физический объём чипа: %llu байт (%llu МБ)\n",
               size_bytes,
               size_bytes / (1024 * 1024));
    }

    /* Вычисляем общий объём и свободное место в файловой системе LittleFS (/lfs) */
    struct fs_statvfs stats;
    if (fs_statvfs("/lfs", &stats) == 0) {
        uint64_t total_lfs = (uint64_t)stats.f_blocks * stats.f_frsize;
        uint64_t free_lfs  = (uint64_t)stats.f_bfree  * stats.f_frsize;

        printk("LittleFS (/lfs): всего %llu КБ, свободно %llu КБ\n",
               total_lfs / 1024,
               free_lfs / 1024);
    }
    
}

int main(void)
{
    system_init();

    /* Небольшая пауза, чтобы терминал успел принять вывод при старте */
    k_sleep(K_MSEC(500));

    printk("\n--- Проверка памяти W25Qxx ---\n");
    check_physical_flash();
    printk("------------------------------\n");

    if (!boot_is_img_confirmed()) {
        int rc = boot_write_img_confirmed();
        if (rc == 0) {
            printk("Firmware image confirmed!");
        } else {
            printk("Failed to confirm image: %d", rc);
        }
    }

    return 0;
}
