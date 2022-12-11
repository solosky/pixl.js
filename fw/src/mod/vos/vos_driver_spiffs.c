#include "vos_driver_spiffs.h"

#include "hal_spi_flash.h"

#include "nrf_log.h"

#include "cwalk.h"
#include "spiffs.h"

#include "mui_mem.h"

#define LOG_PAGE_SIZE 256
#define LOG_BLOCK_SIZE 4096
#define LOG_FLASH_SIZE
#define IFS_MAX_FD 4

#define VOS_SPIFFS_FOLDER_NAME ".folder"

static spiffs fs;
static u8_t spiffs_work_buf[LOG_PAGE_SIZE * 2];
static u8_t spiffs_fds[32 * IFS_MAX_FD];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE + 32) * 4];

/// help functions ///

static const char *spiffs_map_bucket_name(vos_bucket_t bucket) {
    if (bucket == VOS_BUCKET_AMIIBO) {
        return "/amiibo";
    } else if (bucket == VOS_BUCKET_SYSTEM) {
        return "/system";
    } else {
        return NULL;
    }
}

static s32_t spiffs_map_error_code(s32_t err) {
    static const s32_t err_code[][2] = {{SPIFFS_OK, VOS_OK},
                                        {SPIFFS_ERR_FULL, VOS_ERR_NOSPC},
                                        {SPIFFS_ERR_NAME_TOO_LONG, VOS_ERR_MAXNM},
                                        {SPIFFS_ERR_NOT_FOUND, VOS_ERR_NOOBJ}};
    for (uint32_t i = 0; i < sizeof(err_code) / sizeof(err_code[0]); i++) {
        if (err_code[i][0] == err) {
            return err_code[i][1];
        }
    }
    return VOS_ERR_FAIL;
}

static s32_t spiffs_block_read(u32_t addr, u32_t size, u8_t *dst) { return hal_spi_flash_read(addr, dst, size); }

static s32_t spiffs_block_write(u32_t addr, u32_t size, u8_t *src) { return hal_spi_flash_prog(addr, src, size); }

static s32_t spiffs_block_erase(u32_t addr, u32_t size) { return hal_spi_flash_erase(addr); }

int32_t vos_spiffs_mount() {
    flash_info_t flash_info;
    ret_code_t err_code;

    err_code = hal_spi_flash_init();
    if (err_code) {
        return err_code;
    }

    // read flash info
    err_code = hal_spi_flash_info(&flash_info);
    if (err_code) {
        return VOS_ERR_NODEV;
    }

    spiffs_config cfg;
    cfg.phys_size = flash_info.block_count * flash_info.block_size;
    cfg.phys_addr = 0;                            // start spiffs at start of spi flash
    cfg.phys_erase_block = flash_info.block_size; // according to datasheet
    cfg.log_block_size = flash_info.block_size;   // let us not complicate things
    cfg.log_page_size = flash_info.page_size;     // as we said

    cfg.hal_read_f = spiffs_block_read;
    cfg.hal_write_f = spiffs_block_write;
    cfg.hal_erase_f = spiffs_block_erase;

    int res = SPIFFS_mount(&fs, &cfg, spiffs_work_buf, spiffs_fds, sizeof(spiffs_fds), spiffs_cache_buf,
                           sizeof(spiffs_cache_buf), 0);
    NRF_LOG_INFO("external fs mount: %d", res);

    int force_format = 0;

    if (res || force_format) {
        if (SPIFFS_mounted(&fs)) {
            SPIFFS_unmount(&fs);
        }

        res = SPIFFS_format(&fs);
        NRF_LOG_INFO("external fs format: %d", res);
        if (!res) {
            res = SPIFFS_mount(&fs, &cfg, spiffs_work_buf, spiffs_fds, sizeof(spiffs_fds), spiffs_cache_buf,
                               sizeof(spiffs_cache_buf), 0);
            NRF_LOG_INFO("external fs re-mount: %d", res);
        }
    }
    return res;
}

int32_t vos_spiffs_umount() {
    if (SPIFFS_mounted(&fs)) {
        SPIFFS_unmount(&fs);
    }
    return VOS_OK;
}

int32_t vos_spiffs_format() {
    if (SPIFFS_mounted(&fs)) {
        SPIFFS_unmount(&fs);
    }
    int res = SPIFFS_format(&fs);
    if (res == 0) {
        return VOS_OK;
    } else {
        return VOS_ERR_FAIL;
    }
}

bool vos_spiffs_mounted() { return SPIFFS_mounted(&fs); }

int32_t vos_spiffs_stat(vos_stat_t *p_stat) {
    p_stat->avaliable = SPIFFS_mounted(&fs);
    SPIFFS_info(&fs, &p_stat->total_bytes, &p_stat->free_bytes);
    p_stat->free_bytes = p_stat->total_bytes - p_stat->free_bytes;
    return VOS_OK;
}

/* folder operations */
int32_t vos_spiffs_list_folder(vos_bucket_t bucket, vos_obj_t *folders, size_t folder_size) {
    spiffs_DIR d;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe = &e;

    char name[SPIFFS_OBJ_NAME_LEN];
    uint32_t i = 0;

    memset(folders, 0, sizeof(vos_obj_t) * folder_size);

    if (!SPIFFS_opendir(&fs, spiffs_map_bucket_name(bucket), &d)) {
        return VOS_ERR_NOOBJ;
    }
    while ((pe = SPIFFS_readdir(&d, pe))) {
        struct cwk_segment segment;

        cwk_path_get_last_segment(pe->name, &segment);
        memset(name, 0, sizeof(name));
        strncpy(name, segment.begin, segment.size);

        NRF_LOG_INFO("%s [%04x] %d size:%i\n", nrf_log_push(pe->name), pe->obj_id, pe->type, pe->size);

        if (strcmp(name, VOS_SPIFFS_FOLDER_NAME) == 0 && i < folder_size) {
            cwk_path_get_first_segment(pe->name, &segment); // bucket name
            cwk_path_get_next_segment(&segment);            // folder name
            strncpy(folders[i].name, segment.begin, segment.size);
            folders[i].size = 0;
            folders[i].type = 0; // TODO ???
            i++;
        }
    }
    SPIFFS_closedir(&d);

    return i;
}
int32_t vos_spiffs_create_folder(vos_bucket_t bucket, const char *name) {
    char path[SPIFFS_OBJ_NAME_LEN];
    snprintf(path, sizeof(path), "%s/%s/%s", spiffs_map_bucket_name(bucket), name, VOS_SPIFFS_FOLDER_NAME);

    int res = SPIFFS_creat(&fs, path, 0);
    return spiffs_map_error_code(res);
}
int32_t vos_spiffs_remove_folder(vos_bucket_t bucket, const char *name) {
    spiffs_DIR d;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe = &e;

    char path[SPIFFS_OBJ_NAME_LEN];
    sprintf(path, "%s/%s/", spiffs_map_bucket_name(bucket), name);

    if (!SPIFFS_opendir(&fs, path, &d)) {
        return VOS_ERR_NOOBJ;
    }

    while ((pe = SPIFFS_readdir(&d, pe))) {
        int res = SPIFFS_remove(&fs, pe->name);
        if (res != SPIFFS_OK) {
            SPIFFS_closedir(&d);
            return VOS_ERR_FAIL;
        }
    }
    SPIFFS_closedir(&d);

    return VOS_OK;
}

/* obj operations */
int32_t vos_spiffs_list_object(vos_bucket_t bucket, const char *folder_name, vos_obj_t *objects, size_t object_size) {
    spiffs_DIR d;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe = &e;
    uint32_t i = 0;

    char path[SPIFFS_OBJ_NAME_LEN];
    char name[SPIFFS_OBJ_NAME_LEN];
    snprintf(path, sizeof(path), "%s/%s/", spiffs_map_bucket_name(bucket), folder_name);

    if (!SPIFFS_opendir(&fs, path, &d)) {
        return VOS_ERR_NOOBJ;
    }

    while ((pe = SPIFFS_readdir(&d, pe))) {
        struct cwk_segment segment;
        cwk_path_get_last_segment(pe->name, &segment);
        memset(name, 0, sizeof(name));
        strncpy(name, segment.begin, segment.size);
        if (strcmp(name, VOS_SPIFFS_FOLDER_NAME) != 0 && i < object_size) {
            strcpy(objects[i].name, name);
            objects[i].size = pe->size;
            objects[i].type = 0; // TODO ??
            i++;
        }
    }
    SPIFFS_closedir(&d);

    return i;
}
int32_t vos_spiffs_write_object(vos_bucket_t bucket, const char *folder_name, const char *object_name, void *buffer,
                                size_t buffer_size) {
    char path[SPIFFS_OBJ_NAME_LEN];
    snprintf(path, sizeof(path), "%s/%s/%s", spiffs_map_bucket_name(bucket), folder_name, object_name);
    spiffs_file fd = SPIFFS_open(&fs, path, SPIFFS_WRONLY | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
    if (fd < 0) {
        return spiffs_map_error_code(fd);
    }
    int res = SPIFFS_write(&fs, fd, buffer, buffer_size);
    SPIFFS_close(&fs, fd);
    if (res > 0) {
        return res;
    } else {
        return spiffs_map_error_code(res);
    }
}
int32_t vos_spiffs_read_object(vos_bucket_t bucket, const char *folder_name, const char *object_name, void *buffer,
                               size_t buffer_size) {
    char path[SPIFFS_OBJ_NAME_LEN];
    snprintf(path, sizeof(path), "%s/%s/%s", spiffs_map_bucket_name(bucket), folder_name, object_name);
    spiffs_file fd = SPIFFS_open(&fs, path, SPIFFS_RDONLY, 0);
    if (fd < 0) {
        return spiffs_map_error_code(fd);
    }
    int res = SPIFFS_read(&fs, fd, buffer, buffer_size);
    SPIFFS_close(&fs, fd);
    if (res > 0) {
        return res;
    } else {
        return spiffs_map_error_code(res);
    }
}
int32_t vos_spiffs_remove_object(vos_bucket_t bucket, const char *folder_name, const char *object_name) {
    char path[SPIFFS_OBJ_NAME_LEN];
    snprintf(path, sizeof(path), "%s/%s/%s", spiffs_map_bucket_name(bucket), folder_name, object_name);
    int res = SPIFFS_remove(&fs, path);
    return spiffs_map_error_code(res);
}

int32_t vos_spiffs_rename_object(vos_bucket_t bucket, const char *folder_name, const char *object_name,
                                 const char *new_object_name) {
    char path[SPIFFS_OBJ_NAME_LEN];
    char path2[SPIFFS_OBJ_NAME_LEN];
    snprintf(path, sizeof(path), "%s/%s/%s", spiffs_map_bucket_name(bucket), folder_name, object_name);
    snprintf(path2, sizeof(path2), "%s/%s/%s", spiffs_map_bucket_name(bucket), folder_name, new_object_name);
    int res = SPIFFS_rename(&fs, path, path2);
    return spiffs_map_error_code(res);
}

// TODO
vos_driver_t vos_driver_spiffs = {.mount = vos_spiffs_mount,
                                  .umount = vos_spiffs_umount,
                                  .format = vos_spiffs_format,
                                  .mounted = vos_spiffs_mounted,
                                  .stat = vos_spiffs_stat,
                                  .list_folder = vos_spiffs_list_folder,
                                  .create_folder = vos_spiffs_create_folder,
                                  .remove_folder = vos_spiffs_remove_folder,
                                  .list_object = vos_spiffs_list_object,
                                  .write_object = vos_spiffs_write_object,
                                  .read_object = vos_spiffs_read_object,
                                  .remove_object = vos_spiffs_remove_object,
                                  .rename_object = vos_spiffs_rename_object};