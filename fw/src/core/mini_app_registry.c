#include "mini_app_defines.h"
#include "mini_app_registry.h"
#include <stddef.h>

extern const mini_app_t* mini_app_registry[];
extern const uint32_t mini_app_num;


const mini_app_t *mini_app_registry_find_by_id(uint32_t id){
    for(uint32_t i=0; i< mini_app_registry_get_app_num(); i++){
        if(mini_app_registry[i]->id == id){
            return mini_app_registry[i];
        }
    }
    return NULL;
}
uint32_t mini_app_registry_get_app_num(){
    return mini_app_num;
}
const mini_app_t *mini_app_registry_get_app_array(){
    return (mini_app_t *) mini_app_registry;
}