#include "cwalk2.h"
#include "cwalk.h"
#include "vfs.h"
#include <stdio.h>
#include <string.h>

void cwalk_append_segment(char *out_path, const char *path, const char *segment) {

    // if (path[strlen(path) - 1] == '/') {
    //     if (strlen(path) == 1) {
    //         sprintf(out_path, "/%s", segment);
    //     } else {
    //         sprintf(out_path, "%s/%s", path, segment);
    //     }
    // } else {
    //     sprintf(out_path, "%s/%s", path, segment);
    // }

    if (strlen(path) == 1 && path[0] == '/') {
        strcpy(out_path, "/");
        strncat(out_path, segment, VFS_MAX_PATH_LEN - strlen(out_path) - 1);
    } else if (path[strlen(path) - 1] == '/') {
        strcpy(out_path, path);
        strncat(out_path, segment, VFS_MAX_PATH_LEN - strlen(out_path) - 1);
    } else {
        strcpy(out_path, path);
        strcat(out_path, "/");
        strncat(out_path, segment, VFS_MAX_PATH_LEN - strlen(out_path) - 1);
    }
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


void cwalk_dir_prefix_match(char* out_path, const char* path){
    strcpy(out_path, path);
    if (path[strlen(path) - 1] != '/'){
        strcat(out_path, "/");
    }
}