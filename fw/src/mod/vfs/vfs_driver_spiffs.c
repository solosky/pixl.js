#include "vfs_driver_spiffs.h"

#include "hal_spi_flash.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "cwalk.h"
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

    //test only 
// int32_t vfs_spiffs_create_dir(const char*);
//      res = vfs_spiffs_create_dir("/amiibo");
//       res = vfs_spiffs_create_dir("/hello");
//      NRF_LOG_INFO("create amiibo dir: %d", res);
    SPIFFS_creat(&fs, "/amiibo/.folder", 0);
    SPIFFS_creat(&fs, "/hello/.folder", 0);
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

size_t cwalk_get_segment_size(const char *path) {
    struct cwk_segment segment;
    size_t segment_size = 0;
    if (!cwk_path_get_first_segment(path, &segment)) {
        return 0;
    }

    do {
        segment_size++;
    } while (cwk_path_get_next_segment(&segment));

    return segment_size;
}

bool cwalk_same_prefix_segment(const char *base, const char *target) {
    return strncmp(base, target, strlen(base)) == 0;
}

/**directory operations*/
int32_t vfs_spiffs_open_dir(const char *dir, vfs_dir_t *fd) {
    vfs_spiffs_dir_t *p_dir = mui_mem_malloc(sizeof(vfs_spiffs_dir_t));
    if (!p_dir) {
        return VFS_ERR_NOMEM;
    }
    fd->handle = p_dir;
    p_dir->pe = &p_dir->e;
    strncpy(p_dir->dir, dir, sizeof(p_dir->dir));
    p_dir->seg = cwalk_get_segment_size(dir);

    if (!SPIFFS_opendir(&fs, "/", &p_dir->d)) {
        mui_mem_free(p_dir);
        return VFS_ERR_NOOBJ;
    }

    return VFS_OK;
}

int32_t vfs_spiffs_read_dir(vfs_dir_t *fd, vfs_obj_t *obj) {
    vfs_spiffs_dir_t *p_dir = fd->handle;
    memset(obj, 0, sizeof(vfs_obj_t));
    while ((p_dir->pe = SPIFFS_readdir(&p_dir->d, p_dir->pe))) {

        // strcpy(obj->name, p_dir->pe->name);
        //         obj->size = p_dir->pe->size;
        //         obj->type = VFS_TYPE_REG;
        //         return VFS_OK;
         
        //prefix compare
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
    sprintf("%s/%s", dir, VFS_SPIFFS_FOLDER_NAME);
    int res = SPIFFS_creat(&fs, path, 0);
    return vfs_spiffs_map_error_code(res);
}

int32_t vfs_spiffs_remove_dir(const char *dir_name) {

    vfs_spiffs_dir_t dir;
    vfs_spiffs_dir_t *p_dir = &dir;
    p_dir->pe = &p_dir->e;
    strncpy(p_dir->dir, dir_name, sizeof(p_dir->dir));

    if (!SPIFFS_opendir(&fs, "/", &p_dir->d)) {
        return VFS_ERR_NOOBJ;
    }

    while ((p_dir->pe = SPIFFS_readdir(&p_dir->d, p_dir->pe))) {
        // prefix compare
        if (cwalk_same_prefix_segment(p_dir->dir, p_dir->pe->name)) {
            int res = SPIFFS_remove(&fs, p_dir->pe->name);
            if (!res) {
                SPIFFS_closedir(&dir.d);
                return vfs_spiffs_map_error_code(res);
            }
        }
    }

    SPIFFS_closedir(&dir.d);

    return VFS_OK;
}

int32_t vfs_spiffs_rename_dir(const char *dir_name, const char *new_dir_name) {
    vfs_spiffs_dir_t dir;
    vfs_spiffs_dir_t *p_dir = &dir;
    p_dir->pe = &p_dir->e;
    strncpy(p_dir->dir, dir_name, sizeof(p_dir->dir));

    if (!SPIFFS_opendir(&fs, "/", &p_dir->d)) {
        return VFS_ERR_NOOBJ;
    }

    while ((p_dir->pe = SPIFFS_readdir(&p_dir->d, p_dir->pe))) {
        // prefix compare
        if (cwalk_same_prefix_segment(p_dir->dir, p_dir->pe->name)) {

            char new_path[SPIFFS_OBJ_NAME_LEN];
            const char *file_suffix = p_dir->pe->name + strlen(p_dir->dir);

            if (strlen(new_dir_name) + strlen(file_suffix) >= SPIFFS_OBJ_NAME_LEN) {
                return VFS_ERR_MAXNM;
            }

            strcpy(new_path, new_dir_name);
            strcat(new_path, file_suffix);

            int res = SPIFFS_rename(&fs, p_dir->pe->name, new_path);
            if (!res) {
                SPIFFS_closedir(&dir.d);
                return vfs_spiffs_map_error_code(res);
            }
        }
    }

    SPIFFS_closedir(&dir.d);
    return VFS_OK;
}

/**file operations*/
int32_t vfs_spiffs_open_file(const char *file, vfs_file_t *fd, uint32_t flags) { return VFS_ERR_UNSUPT; }

int32_t vfs_spiffs_close_file(vfs_file_t *fd) { return VFS_ERR_UNSUPT; }

int32_t vfs_spiffs_read_file(vfs_file_t *fd, void *buff, size_t buff_size) { return VFS_ERR_UNSUPT; }

int32_t vfs_spiffs_write_file(vfs_file_t *fd, void *buff, size_t buff_size) { return VFS_ERR_UNSUPT; }

/**short opearation*/
int32_t vfs_spiffs_write_file_data(const char *file, void *buff, size_t buff_size) {
    spiffs_file fd = SPIFFS_open(&fs, file, SPIFFS_WRONLY | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
    if (fd < 0) {
        return vfs_spiffs_map_error_code(fd);
    }
    int res = SPIFFS_write(&fs, fd, buff, buff_size);
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
    int res = SPIFFS_rename(&fs, file, new_file);
    return vfs_spiffs_map_error_code(res);
}

int32_t vfs_spiffs_remove_file(const char *file) {
    int res = SPIFFS_remove(&fs, file);
    return vfs_spiffs_map_error_code(res);
}

/**directory operations*/
int32_t (*open_dir)(const char *dir, vfs_dir_t *fd);
int32_t (*read_dir)(vfs_dir_t *fd, vfs_obj_t *obj);
int32_t (*close_dir)(vfs_dir_t *fd);

int32_t (*create_dir)(const char *dir);
int32_t (*remove_dir)(const char *dir);
int32_t (*rename_dir)(const char *dir, const char *new_dir);

/**file operations*/
int32_t (*open_file)(const char *file, vfs_file_t *fd, uint32_t flags);
int32_t (*close_file)(vfs_file_t *fd);
int32_t (*read_file)(vfs_file_t *fd, void *buff, size_t buff_size);
int32_t (*write_file)(vfs_file_t *fd, void *buff, size_t buff_size);

/**short opearation*/
int32_t (*write_file_data)(const char *file, void *buff, size_t buff_size);
int32_t (*read_file_data)(const char *file, void *buff, size_t buff_size);

int32_t (*rename_file)(const char *file, const char *new_file);
int32_t (*remove_file)(const char *file);

// TODO
vfs_driver_t vfs_driver_spiffs = {.mount = vfs_spiffs_mount,
                                  .umount = vfs_spiffs_umount,
                                  .format = vfs_spiffs_format,
                                  .mounted = vfs_spiffs_mounted,
                                  .stat = vfs_spiffs_stat,

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

                                  .write_file_data = vfs_spiffs_write_file_data,
                                  .read_file_data = vfs_spiffs_read_file_data,

                                  .rename_file = vfs_spiffs_rename_file,
                                  .remove_file = vfs_spiffs_remove_file};