#include "boards_defines.h"

#ifdef VFS_SPIFFS_ENABLE

#include "vfs_driver_fs.h"

#include "hal_spi_flash.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "cwalk.h"
#include "cwalk2.h"
#include "spiffs.h"

#include "mui_mem.h"

#define LOG_PAGE_SIZE 256
#define LOG_BLOCK_SIZE 4096
#define LOG_FLASH_SIZE
#define IFS_MAX_FD 4

#define VFS_SPIFFS_FOLDER_NAME ".folder"

static spiffs fs;
static u8_t spiffs_work_buf[LOG_PAGE_SIZE * 2];
static u8_t spiffs_fds[32 * IFS_MAX_FD];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE + 32) * 4];

typedef struct {
    spiffs_DIR d;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe;
    char dir[SPIFFS_OBJ_NAME_LEN];
    uint8_t seg;
} vfs_spiffs_dir_t;

/// help functions ///
static s32_t vfs_spiffs_map_error_code(s32_t err) {
    static const s32_t err_code[][2] = {{SPIFFS_OK, VFS_OK},
                                        {SPIFFS_ERR_FULL, VFS_ERR_NOSPC},
                                        {SPIFFS_ERR_NAME_TOO_LONG, VFS_ERR_MAXNM},
                                        {SPIFFS_ERR_NOT_FOUND, VFS_ERR_NOOBJ}};
    for (uint32_t i = 0; i < sizeof(err_code) / sizeof(err_code[0]); i++) {
        if (err_code[i][0] == err) {
            return err_code[i][1];
        }
    }
    return VFS_ERR_FAIL;
}

static s32_t spiffs_block_read(u32_t addr, u32_t size, u8_t *dst) { return hal_spi_flash_read(addr, dst, size); }

static s32_t spiffs_block_write(u32_t addr, u32_t size, u8_t *src) { return hal_spi_flash_prog(addr, src, size); }

static s32_t spiffs_block_erase(u32_t addr, u32_t size) { return hal_spi_flash_erase(addr); }

int32_t vfs_spiffs_mount() {
    flash_info_t flash_info;
    ret_code_t err_code;

    err_code = hal_spi_flash_init();
    if (err_code) {
        return err_code;
    }

    // read flash info
    err_code = hal_spi_flash_info(&flash_info);
    if (err_code) {
        return VFS_ERR_NODEV;
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

    // test only
    // int32_t vfs_spiffs_create_dir(const char*);
    //      res = vfs_spiffs_create_dir("/amiibo");
    //       res = vfs_spiffs_create_dir("/hello");
    //      NRF_LOG_INFO("create amiibo dir: %d", res);
    // SPIFFS_creat(&fs, "/amiibo/.folder", 0);
    // SPIFFS_creat(&fs, "/hello/.folder", 0);
    // SPIFFS_creat(&fs, "/hello/塞尔达/.folder", 0);
    return res;
}

int32_t vfs_spiffs_umount() {
    if (SPIFFS_mounted(&fs)) {
        SPIFFS_unmount(&fs);
    }
    return VFS_OK;
}

int32_t vfs_spiffs_format() {
    if (SPIFFS_mounted(&fs)) {
        SPIFFS_unmount(&fs);
    }
    int res = SPIFFS_format(&fs);
    if (res == 0) {
        return VFS_OK;
    } else {
        return VFS_ERR_FAIL;
    }
}

bool vfs_spiffs_mounted() { return SPIFFS_mounted(&fs); }

int32_t vfs_spiffs_stat(vfs_stat_t *p_stat) {
    p_stat->avaliable = SPIFFS_mounted(&fs);
    SPIFFS_info(&fs, &p_stat->total_bytes, &p_stat->free_bytes);
    p_stat->free_bytes = p_stat->total_bytes - p_stat->free_bytes;
    return VFS_OK;
}

int32_t vfs_spiffs_stat_file(const char *file, vfs_obj_t *obj) {
    spiffs_stat s;
    const char *basename;
    size_t length;
    char path[SPIFFS_OBJ_NAME_LEN];
    memset(obj, 0, sizeof(vfs_obj_t));

    if (SPIFFS_stat(&fs, file, &s) == SPIFFS_OK) {
        cwk_path_get_basename(s.name, &basename, &length);

        obj->type = VFS_TYPE_REG;
        obj->size = s.size;
        strncpy(obj->name, basename, length);
        memcpy(obj->meta, s.meta, sizeof(obj->meta));

        return VFS_OK;
    } else {
        cwalk_append_segment(path, file, VFS_SPIFFS_FOLDER_NAME);
        NRF_LOG_INFO("stat file: %s", nrf_log_push(path));
        if (SPIFFS_stat(&fs, path, &s) == SPIFFS_OK) {
            cwk_path_get_basename(s.name, &basename, &length);
            obj->type = VFS_TYPE_DIR;
            obj->size = 0;
            strncpy(obj->name, basename, length);
            memcpy(obj->meta, s.meta, sizeof(obj->meta));
            return VFS_OK;
        }
    }

    return VFS_ERR_NOOBJ;
}

/**directory operations*/
int32_t vfs_spiffs_open_dir(const char *dir, vfs_dir_t *fd) {
    vfs_spiffs_dir_t *p_dir = mui_mem_malloc(sizeof(vfs_spiffs_dir_t));
    if (!p_dir) {
        return VFS_ERR_NOMEM;
    }
    fd->handle = p_dir;
    p_dir->pe = &p_dir->e;
    cwalk_dir_prefix_match(p_dir->dir, dir);
    p_dir->seg = cwalk_get_segment_size(dir);

    if (!SPIFFS_opendir(&fs, "/", &p_dir->d)) {
        mui_mem_free(p_dir);
        return VFS_ERR_NOOBJ;
    }

    NRF_LOG_INFO("open dir %s\n", nrf_log_push(dir));

    return VFS_OK;
}

int32_t vfs_spiffs_read_dir(vfs_dir_t *fd, vfs_obj_t *obj) {
    vfs_spiffs_dir_t *p_dir = fd->handle;
    memset(obj, 0, sizeof(vfs_obj_t));
    while ((p_dir->pe = SPIFFS_readdir(&p_dir->d, p_dir->pe))) {

        // NRF_LOG_INFO("list folder %s [%04x] %d size:%i\n", nrf_log_push(p_dir->pe->name), p_dir->pe->obj_id,
        //              p_dir->pe->type, p_dir->pe->size);
        // NRF_LOG_FLUSH();

        // strcpy(obj->name, p_dir->pe->name);
        //         obj->size = p_dir->pe->size;
        //         obj->type = VFS_TYPE_REG;
        //         return VFS_OK;

        // prefix compare
        if (cwalk_same_prefix_segment(p_dir->dir, p_dir->pe->name)) {

            uint8_t seg;
            const char *basename;
            size_t length;
            bool is_folder;

            // /amiibo/.folder
            // /amiibo/zelda/.folder
            // /amiibo/link.bin
            // /amiibo/zelda/mifa.bin

            cwk_path_get_basename(p_dir->pe->name, &basename, &length);

            is_folder = strncmp(basename, VFS_SPIFFS_FOLDER_NAME, length) == 0;

            seg = cwalk_get_segment_size(p_dir->pe->name);
            // next dir level
            if (seg == p_dir->seg + 1 && !is_folder) {
                // /amiibo/.folder
                // /amiibo/link.bin

                // not a .folder, it's a file!!
                struct cwk_segment segment;
                cwk_path_get_last_segment(p_dir->pe->name, &segment); //.folder
                strncpy(obj->name, segment.begin, segment.size);
                obj->name[sizeof(obj->name) - 1] = '\0';

                memcpy(obj->meta, p_dir->pe->meta, sizeof(obj->meta));

                obj->size = p_dir->pe->size;
                obj->type = VFS_TYPE_REG;

                return VFS_OK;

            } else if (seg == p_dir->seg + 2 && is_folder) {
                // /amiibo/zelda/.folder
                // /amiibo/zelda/mifa.bin

                struct cwk_segment segment;
                cwk_path_get_last_segment(p_dir->pe->name, &segment); //.folder
                cwk_path_get_previous_segment(&segment);              // zelda
                strncpy(obj->name, segment.begin, segment.size);
                obj->name[sizeof(obj->name) - 1] = '\0';

                memcpy(obj->meta, p_dir->pe->meta, sizeof(obj->meta));

                obj->size = p_dir->pe->size;
                obj->type = VFS_TYPE_DIR;

                return VFS_OK;
            }
        }
    }

    return VFS_ERR_EOF;
}

int32_t vfs_spiffs_close_dir(vfs_dir_t *fd) {
    vfs_spiffs_dir_t *p_dir = fd->handle;
    SPIFFS_closedir(&p_dir->d);
    mui_mem_free(fd->handle);
    return VFS_OK;
}

int32_t vfs_spiffs_create_dir(const char *dir) {
    char path[VFS_MAX_PATH_LEN];

    NRF_LOG_INFO("create dir %s\n", nrf_log_push(dir));
    snprintf(path, sizeof(path), "%s/%s", dir, VFS_SPIFFS_FOLDER_NAME);
    int res = SPIFFS_creat(&fs, path, 0);
    return vfs_spiffs_map_error_code(res);
}

int32_t vfs_spiffs_remove_dir(const char *dir_name) {

    vfs_spiffs_dir_t dir;
    vfs_spiffs_dir_t *p_dir = &dir;
    p_dir->pe = &p_dir->e;
    strncpy(p_dir->dir, dir_name, sizeof(p_dir->dir));
    strcat(p_dir->dir, "/");

    NRF_LOG_INFO("remove dir %s\n", nrf_log_push(dir_name));

    if (!SPIFFS_opendir(&fs, "/", &p_dir->d)) {
        return VFS_ERR_NOOBJ;
    }

    while ((p_dir->pe = SPIFFS_readdir(&p_dir->d, p_dir->pe))) {
        // prefix compare
        if (cwalk_same_prefix_segment(p_dir->dir, p_dir->pe->name)) {
            int res = SPIFFS_remove(&fs, p_dir->pe->name);
            NRF_LOG_INFO("remove file %d: %s\n", res, nrf_log_push(p_dir->pe->name));
            if (res != SPIFFS_OK) {
                SPIFFS_closedir(&dir.d);
                return vfs_spiffs_map_error_code(res);
            }
        }
    }

    SPIFFS_closedir(&dir.d);

    return VFS_OK;
}

int32_t vfs_spiffs_rename_dir_internal(const char *dir_name, const char *new_dir_name, bool check_only) {
    vfs_spiffs_dir_t dir;
    vfs_spiffs_dir_t *p_dir = &dir;
    int32_t err_code = VFS_OK;

    p_dir->pe = &p_dir->e;
    cwalk_dir_prefix_match(p_dir->dir, dir_name);

    if (!SPIFFS_opendir(&fs, "/", &p_dir->d)) {
        return VFS_ERR_NOOBJ;
    }

    while ((p_dir->pe = SPIFFS_readdir(&p_dir->d, p_dir->pe))) {
        // prefix compare
        if (cwalk_same_prefix_segment(p_dir->dir, p_dir->pe->name)) {

            char new_path[SPIFFS_OBJ_NAME_LEN];
            const char *file_suffix = p_dir->pe->name + strlen(p_dir->dir);

            uint32_t new_path_len = strlen(new_dir_name) + strlen(file_suffix) + 1;
            NRF_LOG_INFO("path len check: %d", new_path_len);
            if (new_path_len >= sizeof(new_path)) {
                err_code = VFS_ERR_MAXNM;
                NRF_LOG_INFO("rename folder name too long");
                break;
            }

            strcpy(new_path, new_dir_name);
            strcat(new_path, "/");
            strcat(new_path, file_suffix);

            if (!check_only) {
                int res = SPIFFS_rename(&fs, p_dir->pe->name, new_path);
                if (res != SPIFFS_OK) {
                    err_code = vfs_spiffs_map_error_code(res);
                    break;
                }
            }
        }
    }

    SPIFFS_closedir(&dir.d);
    return err_code;
}

int32_t vfs_spiffs_rename_dir(const char *dir_name, const char *new_dir_name) {
    int32_t err_code = vfs_spiffs_rename_dir_internal(dir_name, new_dir_name, true);
    if (err_code == VFS_OK) {
        err_code = vfs_spiffs_rename_dir_internal(dir_name, new_dir_name, false);
    }
    return err_code;
}

/**file operations*/
int32_t vfs_spiffs_open_file(const char *file, vfs_file_t *fd, uint32_t flags) {
    fd->handle = SPIFFS_open(&fs, file, flags, 0);
    if (fd->handle < 0) {
        return vfs_spiffs_map_error_code(fd->handle);
    }

    return VFS_OK;
}

int32_t vfs_spiffs_close_file(vfs_file_t *fd) {
    if (fd->handle >= 0) {
        SPIFFS_close(&fs, fd->handle);
    }
    return VFS_OK;
}

int32_t vfs_spiffs_read_file(vfs_file_t *fd, void *buff, size_t buff_size) {
    if (fd->handle < 0) {
        return VFS_ERR_FAIL;
    }
    NRF_LOG_INFO("read file %d with %d bytes", fd->handle, buff_size);
    int32_t read_size = SPIFFS_read(&fs, fd->handle, buff, buff_size);
    if (read_size < 0) {
        return vfs_spiffs_map_error_code(read_size);
    }
    return read_size;
}

int32_t vfs_spiffs_write_file(vfs_file_t *fd, void *buff, size_t buff_size) {
    if (fd->handle < 0) {
        return VFS_ERR_FAIL;
    }
    NRF_LOG_INFO("write file %d with %d bytes", fd->handle, buff_size);
    int32_t written_size = SPIFFS_write(&fs, fd->handle, buff, buff_size);
    if (written_size < 0) {
        return vfs_spiffs_map_error_code(written_size);
    }
    return written_size;
}

int32_t vfs_spiffs_update_file_meta(const char *file, void *meta, size_t meta_size) {
    vfs_obj_t obj;
    int32_t err = vfs_spiffs_stat_file(file, &obj);
    if (err != VFS_OK) {
        return err;
    }
    if (obj.type == VFS_TYPE_DIR) {
        char path[SPIFFS_OBJ_NAME_LEN];
        strcpy(path, file);
        strcat(path, "/");
        strcat(path, VFS_SPIFFS_FOLDER_NAME);
        err = SPIFFS_update_meta(&fs, path, meta);
    } else {
        err = SPIFFS_update_meta(&fs, file, meta);
    }

    NRF_LOG_INFO("update file meta: %s, size:%d res: %d", nrf_log_push(file), meta_size, err);
    return vfs_spiffs_map_error_code(err);
}

/**short opearation*/
int32_t vfs_spiffs_write_file_data(const char *file, void *buff, size_t buff_size) {

    NRF_LOG_INFO("write file data %s\n", nrf_log_push(file));
    spiffs_file fd = SPIFFS_open(&fs, file, SPIFFS_WRONLY | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
    if (fd < 0) {
        return vfs_spiffs_map_error_code(fd);
    }
    int res = VFS_OK;
    if (buff != NULL && buff_size > 0) {
        res = SPIFFS_write(&fs, fd, buff, buff_size);
    }
    SPIFFS_close(&fs, fd);
    if (res > 0) {
        return res;
    } else {
        return vfs_spiffs_map_error_code(res);
    }
}

int32_t vfs_spiffs_read_file_data(const char *file, void *buff, size_t buff_size) {
    spiffs_file fd = SPIFFS_open(&fs, file, SPIFFS_RDONLY, 0);
    if (fd < 0) {
        return vfs_spiffs_map_error_code(fd);
    }
    int res = SPIFFS_read(&fs, fd, buff, buff_size);
    SPIFFS_close(&fs, fd);
    if (res > 0) {
        return res;
    } else {
        return vfs_spiffs_map_error_code(res);
    }
}

int32_t vfs_spiffs_rename_file(const char *file, const char *new_file) {
    if (strlen(new_file) >= SPIFFS_OBJ_NAME_LEN) {
        NRF_LOG_INFO("rename file error, file %s new file %s is too long");
        return VFS_ERR_MAXNM;
    }
    NRF_LOG_INFO("rename file %s => %s\n", nrf_log_push(file), nrf_log_push(new_file));
    int res = SPIFFS_rename(&fs, file, new_file);
    return vfs_spiffs_map_error_code(res);
}

int32_t vfs_spiffs_remove_file(const char *file) {
    int res = SPIFFS_remove(&fs, file);
    return vfs_spiffs_map_error_code(res);
}

// TODO
const vfs_driver_t vfs_driver_fs = {.mount = vfs_spiffs_mount,
                                    .umount = vfs_spiffs_umount,
                                    .format = vfs_spiffs_format,
                                    .mounted = vfs_spiffs_mounted,
                                    .stat = vfs_spiffs_stat,

                                    .stat_file = vfs_spiffs_stat_file,

                                    .open_dir = vfs_spiffs_open_dir,
                                    .read_dir = vfs_spiffs_read_dir,
                                    .close_dir = vfs_spiffs_close_dir,
                                    .create_dir = vfs_spiffs_create_dir,
                                    .remove_dir = vfs_spiffs_remove_dir,
                                    .rename_dir = vfs_spiffs_rename_dir,

                                    .open_file = vfs_spiffs_open_file,
                                    .close_file = vfs_spiffs_close_file,
                                    .read_file = vfs_spiffs_read_file,
                                    .write_file = vfs_spiffs_write_file,
                                    .update_file_meta = vfs_spiffs_update_file_meta,

                                    .write_file_data = vfs_spiffs_write_file_data,
                                    .read_file_data = vfs_spiffs_read_file_data,

                                    .rename_file = vfs_spiffs_rename_file,
                                    .remove_file = vfs_spiffs_remove_file};
#endif