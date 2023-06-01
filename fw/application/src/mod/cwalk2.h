#include <stddef.h>
#include <stdbool.h>

void cwalk_append_segment(char *out_path, const char *path, const char *segment);
size_t cwalk_get_segment_size(const char *path);
bool cwalk_same_prefix_segment(const char *base, const char *target);
void cwalk_dir_prefix_match(char* out_path, const char* path);