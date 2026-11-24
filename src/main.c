#include "tasks.h"
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>
#include <string.h>

#define CONFIG_FILE "/lfs/config.json"

/* Определяем структуру монтирования LittleFS для нашего раздела ext_storage */
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(storage_cfg);

static struct fs_mount_t lfs_storage_mnt = {
    .type = FS_LITTLEFS,
    .fs_data = &storage_cfg,
    .storage_dev = (void *)PARTITION_ID(lfs_partition),
    .mnt_point = "/lfs",
};

int main(void)
{
    struct fs_file_t file;
    int rc;
    char buffer[128];

    system_init();

    k_sleep(K_MSEC(500));
    printk("--- Тест W25Qxx + LittleFS в Zephyr ---\n");

    fs_file_t_init(&file);

    /* Создаём и записываем файл (раздел /lfs уже примонтирован при старте, об этом позаботился код из оверлея) */
    rc = fs_open(&file, CONFIG_FILE, FS_O_CREATE | FS_O_WRITE | FS_O_TRUNC);
    if (rc < 0) {
        printk("Ошибка открытия на запись: %d\n", rc);
        return 1;
    }

    const char *json = "{\"os\":\"zephyr\",\"uart\":\"usart3\",\"ok\":true}\n";
    fs_write(&file, json, strlen(json));
    fs_close(&file);
    printk("Файл успешно записан!\n");

    /* Читаем файл обратно */
    fs_file_t_init(&file);
    rc = fs_open(&file, CONFIG_FILE, FS_O_READ);
    if (rc < 0) {
        printk("Ошибка открытия на чтение: %d\n", rc);
        return 1;
    }

    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes = fs_read(&file, buffer, sizeof(buffer) - 1);
    fs_close(&file);

    if (bytes >= 0) {
        printk("Прочитано (%zd байт): %s\n", bytes, buffer);
    }

    return 0;
}
