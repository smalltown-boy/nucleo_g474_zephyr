
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>
#include <zephyr/logging/log.h>
#include "init_fs.h"

/* Регистрируем отдельный модуль логов для файловой системы */
LOG_MODULE_REGISTER(fs_module, LOG_LEVEL_INF);

#define NVS_PARTITION        storage_partition
#define NVS_PARTITION_DEVICE FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET FIXED_PARTITION_OFFSET(NVS_PARTITION)

#define PARAM_ID 1

static struct nvs_fs fs;

int fs_init(void)
{
    int rc = 0;
    struct flash_pages_info info;

    fs.flash_device = NVS_PARTITION_DEVICE;
    if (!device_is_ready(fs.flash_device)) {
        LOG_ERR("Flash device %s is not ready", fs.flash_device->name);
        return -ENODEV;
    }

    fs.offset = NVS_PARTITION_OFFSET;
    rc = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
    if (rc) {
        LOG_ERR("Unable to get page info, rc=%d", rc);
        return rc;
    }

    fs.sector_size = info.size;
    fs.sector_count = FIXED_PARTITION_SIZE(NVS_PARTITION) / info.size;

    rc = nvs_mount(&fs);
    if (rc) {
        LOG_ERR("Flash Init failed, rc=%d", rc);
        return rc;
    }

    LOG_INF("NVS mounted successfully.");

    return 0;
}

int storage_get_param(float *val)
{
    int rc = nvs_read(&fs, PARAM_ID, val, sizeof(float));
    if (rc > 0) {
        return 0; /* Успешно прочитано */
    }
    return -ENOENT; /* Запись еще не создана */
}

int storage_set_param(float val)
{
    int rc = nvs_write(&fs, PARAM_ID, &val, sizeof(float));
    if (rc < 0) {
        LOG_ERR("Failed to write param, rc=%d", rc);
        return rc;
    }
    return 0;
}