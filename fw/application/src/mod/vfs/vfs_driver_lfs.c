
#include "boards_defines.h"

#ifdef VFS_LFS_ENABLE
#include "vfs_driver_fs.h"

#include "nrf_log.h"

#include "lfs.h"

#include "hal_spi_flash.h"
#include "mui_mem.h"

#include "m-string.h"

#include "cwalk2.h"

#define LFS_SECTOR_SIZE 256
#define LFS_BLOCK_SIZE 4096
#define LFS_ATTR_META 0xAA

int block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
    return hal_spi_flash_read((block * c->block_size + off), buffer, size);
}

int block_device_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer,
                      lfs_size_t size) {
    return hal_spi_flash_prog((block * c->block_size + off), buffer, size);
}

int block_device_erase(const struct lfs_config *c, lfs_block_t block) {
    return hal_spi_flash_erase(block * c->block_size);
}

int block_device_sync(const struct lfs_config *c) { return 0; }

static int32_t vfs_lfs_map_error_code(int32_t err) {
    static const int32_t err_code[][2] = {{LFS_ERR_OK, VFS_OK},
                                          {LFS_ERR_NOMEM, VFS_ERR_NOSPC},
                                          {LFS_ERR_NAMETOOLONG, VFS_ERR_MAXNM},
                                          {LFS_ERR_NOENT, VFS_ERR_NOOBJ}};
    for (uint32_t i = 0; i < sizeof(err_code) / sizeof(err_code[0]); i++) {
        if (err_code[i][0] == err) {
            return err_code[i][1];
        }
    }
    return VFS_ERR_FAIL;
}

static int32_t vfs_lfs_map_flags(int32_t vfs_flags) {
    int32_t lfs_flags = 0;
    if (vfs_flags & VFS_MODE_CREATE) {
        lfs_flags |= LFS_O_CREAT;
    }
    if (vfs_flags & VFS_MODE_TRUNC) {
        lfs_flags |= LFS_O_TRUNC;
    }
    if (vfs_flags & VFS_MODE_APPEND) {
        lfs_flags |= LFS_O_APPEND;
    }

    if (vfs_flags & VFS_MODE_READONLY) {
        lfs_flags |= LFS_O_RDONLY;
    }

    if (vfs_flags & VFS_MODE_WRITEONLY) {
        lfs_flags |= LFS_O_WRONLY;
    }
    return lfs_flags;
}

#define LFS_CACHE_SIZE 256

lfs_t lfs;
struct lfs_config lfs_config;
bool mounted;

uint8_t lfs_read_buf[LFS_CACHE_SIZE];
uint8_t lfs_prog_buf[LFS_CACHE_SIZE];
uint8_t lfs_lookahead_buf[16]; // 128/8=16

int32_t vfs_lfs_mount(void) {
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

    // block device operations
    lfs_config.read = block_device_read;
    lfs_config.prog = block_device_prog;
    lfs_config.erase = block_device_erase;
    lfs_config.sync = block_device_sync;

    // block device configuration
    lfs_config.read_size = flash_info.page_size;
    lfs_config.prog_size = flash_info.page_size;
    lfs_config.block_size = flash_info.block_size;
    lfs_config.block_count = flash_info.block_count;
    lfs_config.cache_size = LFS_CACHE_SIZE;
    lfs_config.lookahead_size = 16;
    lfs_config.block_cycles = 500;

    lfs_config.read_buffer = lfs_read_buf;
    lfs_config.prog_buffer = lfs_prog_buf;
    lfs_config.lookahead_buffer = lfs_lookahead_buf;

    mounted = false;

    int err = lfs_mount(&lfs, &lfs_config);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        NRF_LOG_INFO("mount error, format as new device: %d", err);
        err = lfs_format(&lfs, &lfs_config);
        if (err) {
            NRF_LOG_INFO("lfs format error: %d", err);
            return err;
        }
        err = lfs_mount(&lfs, &lfs_config);
        if (err) {
            NRF_LOG_INFO("re mount error: %d", err);
            return err;
        }
        mounted = true;
    } else {
        mounted = true;
    }

    return err;
}

static int _traverse_df_cb(void *p, lfs_block_t block) {
    uint32_t *nb = p;
    *nb += 1;
    return 0;
}

ret_code_t lfs_storage_stat(uint32_t *p_total_space, uint32_t *p_free_space) {
    ret_code_t err;

    uint32_t _df_nballocatedblock = 0;
    err = lfs_fs_traverse(&lfs, _traverse_df_cb, &_df_nballocatedblock);
    if (err < 0) {
        return err;
    }

    uint32_t available = lfs_config.block_count * lfs_config.block_size - _df_nballocatedblock * lfs_config.block_size;
    *p_free_space = available;
    *p_total_space = lfs_config.block_count * lfs_config.block_size;

    return err;
}

int32_t vfs_lfs_umount() {
    if (lfs_unmount(&lfs) == LFS_ERR_OK) {
        return VFS_OK;
    }
    return VFS_ERR_FAIL;
}

int32_t vfs_lfs_format() {
    int res = lfs_format(&lfs, &lfs_config);
    if (res == LFS_ERR_OK) {
        return VFS_OK;
    } else {
        return VFS_ERR_FAIL;
    }
}

bool vfs_lfs_mounted() { return mounted; }

int32_t vfs_lfs_stat(vfs_stat_t *p_stat) {
    p_stat->avaliable = mounted;
    return lfs_storage_stat(&p_stat->total_bytes, &p_stat->free_bytes);
}

int32_t vfs_lfs_stat_file(const char *file, vfs_obj_t *obj) {

    struct lfs_info info;
    memset(obj, 0, sizeof(vfs_obj_t));
    int res = lfs_stat(&lfs, file, &info);
    if (res == LFS_ERR_OK) {
        obj->type = info.type == LFS_TYPE_DIR ? VFS_TYPE_DIR : VFS_TYPE_REG;
        obj->size = info.size;
        strncpy(obj->name, info.name, sizeof(obj->name));
        lfs_getattr(&lfs, file, LFS_ATTR_META, obj->meta, sizeof(obj->meta));
        return VFS_OK;
    }

    return VFS_ERR_NOOBJ;
}

int32_t vfs_lfs_open_dir(const char *dir, vfs_dir_t *fd) {
    lfs_dir_t *p_dir = mui_mem_malloc(sizeof(lfs_dir_t));
    if (!p_dir) {
        return VFS_ERR_NOMEM;
    }
    fd->handle = p_dir;
    strcpy(fd->path, dir);

    int32_t err = lfs_dir_open(&lfs, p_dir, dir);

    if (err != LFS_ERR_OK) {
        mui_mem_free(p_dir);
        return VFS_ERR_NOOBJ;
    }

    NRF_LOG_INFO("open dir %s\n", nrf_log_push(dir));

    return VFS_OK;
}

int32_t vfs_lfs_read_dir_internal(vfs_dir_t *fd, vfs_obj_t *obj) {
    char path[VFS_MAX_FULL_PATH_LEN];
    lfs_dir_t *p_dir = fd->handle;
    memset(obj, 0, sizeof(vfs_obj_t));
    struct lfs_info info;

    int32_t err = lfs_dir_read(&lfs, p_dir, &info);
    if (err > 0) {
        obj->type = info.type == LFS_TYPE_DIR ? VFS_TYPE_DIR : VFS_TYPE_REG;
        obj->size = info.size;
        strncpy(obj->name, info.name, sizeof(obj->name));

        cwalk_append_segment(path, fd->path, obj->name);
        lfs_getattr(&lfs, path, LFS_ATTR_META, obj->meta, sizeof(obj->meta));

        return VFS_OK;
    } else if (err == 0) {
        return VFS_ERR_EOF;
    } else {
        return vfs_lfs_map_error_code(err);
    }
}

int32_t vfs_lfs_read_dir(vfs_dir_t *fd, vfs_obj_t *obj) {
    int32_t err;
    //skip . and .. directory
    while ((err = vfs_lfs_read_dir_internal(fd, obj)) == VFS_OK) {
        if (strcmp(obj->name, ".") != 0 && strcmp(obj->name, "..") != 0) {
            return err;
        }
    }
    return err;
}

int32_t vfs_lfs_close_dir(vfs_dir_t *fd) {
    lfs_dir_t *p_dir = fd->handle;
    lfs_dir_close(&lfs, p_dir);
    mui_mem_free(fd->handle);
    return VFS_OK;
}

int32_t vfs_lfs_create_dir(const char *dir) {
    NRF_LOG_INFO("create dir %s\n", nrf_log_push(dir));
    int err = lfs_mkdir(&lfs, dir);
    return vfs_lfs_map_error_code(err);
}

int32_t vfs_lfs_remove_dir(const char *dir) {
    NRF_LOG_INFO("remove dir %s\n", nrf_log_push(dir));
    int err = lfs_remove(&lfs, dir);
    return vfs_lfs_map_error_code(err);
}

int32_t vfs_lfs_rename_dir(const char *old_dir, const char *new_dir) {
    int32_t err = lfs_rename(&lfs, old_dir, old_dir);
    return vfs_lfs_map_error_code(err);
}

/**file operations*/
int32_t vfs_lfs_open_file(const char *file, vfs_file_t *fd, uint32_t flags) {
    lfs_file_t *p_file = mui_mem_malloc(sizeof(lfs_file_t));
    int32_t err = lfs_file_open(&lfs, p_file, file, vfs_lfs_map_flags(flags));
    if (err != LFS_ERR_OK) {
        mui_mem_free(p_file);
        return vfs_lfs_map_error_code(err);
    }
    fd->handle = p_file;
    return VFS_OK;
}

int32_t vfs_lfs_close_file(vfs_file_t *fd) {
    if (fd->handle) {
        lfs_file_close(&lfs, fd->handle);
        mui_mem_free(fd->handle);
        fd->handle = NULL;
    }
    return VFS_OK;
}

int32_t vfs_lfs_read_file(vfs_file_t *fd, void *buff, size_t buff_size) {
    if (fd->handle < 0) {
        return VFS_ERR_FAIL;
    }
    NRF_LOG_INFO("read file %d with %d bytes", fd->handle, buff_size);
    int32_t read_size = lfs_file_read(&lfs, fd->handle, buff, buff_size);
    if (read_size < 0) {
        return vfs_lfs_map_error_code(read_size);
    }
    return read_size;
}

int32_t vfs_lfs_write_file(vfs_file_t *fd, void *buff, size_t buff_size) {
    if (fd->handle < 0) {
        return VFS_ERR_FAIL;
    }
    NRF_LOG_INFO("write file %d with %d bytes", fd->handle, buff_size);
    int32_t written_size = lfs_file_write(&lfs, fd->handle, buff, buff_size);
    if (written_size < 0) {
        return vfs_lfs_map_error_code(written_size);
    }
    return written_size;
}

int32_t vfs_lfs_update_file_meta(const char *file, void *meta, size_t meta_size) {
    int32_t err = lfs_setattr(&lfs, file, LFS_ATTR_META, meta, meta_size);
    NRF_LOG_INFO("update file meta: %s, size:%d res: %d", nrf_log_push(file), meta_size, err);
    return vfs_lfs_map_error_code(err);
}

/**short opearation*/
int32_t vfs_lfs_write_file_data(const char *file, void *buff, size_t buff_size) {
    struct lfs_file fd;
    int32_t file_written_size = 0;

    NRF_LOG_INFO("write file data %s\n", nrf_log_push(file));
    int32_t err = lfs_file_open(&lfs, &fd, file, LFS_O_CREAT | LFS_O_TRUNC | LFS_O_WRONLY);
    if (err != LFS_ERR_OK) {
        return vfs_lfs_map_error_code(err);
    }

    err = lfs_file_write(&lfs, &fd, buff, buff_size);
    if (err < 0) {
        lfs_file_close(&lfs, &fd);
        return vfs_lfs_map_error_code(err);
    }

    file_written_size = err;

    err = lfs_file_close(&lfs, &fd);
    if (err < 0) {
        return vfs_lfs_map_error_code(err);
    }

    return file_written_size;
}

int32_t vfs_lfs_read_file_data(const char *file, void *buff, size_t buff_size) {
    struct lfs_file fd;
    int32_t file_read_size = 0;

    NRF_LOG_INFO("read file data %s\n", nrf_log_push(file));
    int32_t err = lfs_file_open(&lfs, &fd, file, LFS_O_RDONLY);
    if (err != LFS_ERR_OK) {
        return vfs_lfs_map_error_code(err);
    }

    err = lfs_file_read(&lfs, &fd, buff, buff_size);
    if (err < 0) {
        lfs_file_close(&lfs, &fd);
        return vfs_lfs_map_error_code(err);
    }

    file_read_size = err;

    err = lfs_file_close(&lfs, &fd);
    if (err < 0) {
        return vfs_lfs_map_error_code(err);
    }

    return file_read_size;
}

int32_t vfs_lfs_rename_file(const char *file, const char *new_file) {
    NRF_LOG_INFO("rename file %s => %s", nrf_log_push(file), nrf_log_push(new_file));
    int32_t err = lfs_rename(&lfs, file, new_file);
    return vfs_lfs_map_error_code(err);
}

int32_t vfs_lfs_remove_file(const char *file) {
    NRF_LOG_INFO("remove file %s => %s", file);
    int32_t err = lfs_remove(&lfs, file);
    return vfs_lfs_map_error_code(err);
}

// TODO
const vfs_driver_t vfs_driver_fs = {.mount = vfs_lfs_mount,
                                     .umount = vfs_lfs_umount,
                                     .format = vfs_lfs_format,
                                     .mounted = vfs_lfs_mounted,
                                     .stat = vfs_lfs_stat,

                                     .stat_file = vfs_lfs_stat_file,

                                     .open_dir = vfs_lfs_open_dir,
                                     .read_dir = vfs_lfs_read_dir,
                                     .close_dir = vfs_lfs_close_dir,
                                     .create_dir = vfs_lfs_create_dir,
                                     .remove_dir = vfs_lfs_remove_dir,
                                     .rename_dir = vfs_lfs_rename_dir,

                                     .open_file = vfs_lfs_open_file,
                                     .close_file = vfs_lfs_close_file,
                                     .read_file = vfs_lfs_read_file,
                                     .write_file = vfs_lfs_write_file,
                                     .update_file_meta = vfs_lfs_update_file_meta,

                                     .write_file_data = vfs_lfs_write_file_data,
                                     .read_file_data = vfs_lfs_read_file_data,

                                     .rename_file = vfs_lfs_rename_file,
                                     .remove_file = vfs_lfs_remove_file};
#endif