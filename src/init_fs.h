#ifndef INIT_FS_H_
#define INIT_FS_H_

#include <zephyr/kernel.h>

int fs_init();

int storage_get_param(float *val);
int storage_set_param(float val);

#endif
