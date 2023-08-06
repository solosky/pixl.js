#include "mini_app_defines.h"
#include "mini_app_registry.h"
#include <stddef.h>
#include "i18n/language.h"

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
const mini_app_t *mini_app_registry_find_by_index(uint32_t idx){
    switch (mini_app_registry[idx]->id)
    {
    case MINI_APP_ID_AMIIBO:
            strncpy(mini_app_registry[idx]->name, getLangString(_L_APP_AMIIBO), sizeof(mini_app_registry[idx]->name));
        break;
    case MINI_APP_ID_AMIIBOLINK:
            strncpy(mini_app_registry[idx]->name, getLangString(_L_APP_AMIIBOLINK), sizeof(mini_app_registry[idx]->name));
        break;
    case MINI_APP_ID_BLE:
            strncpy(mini_app_registry[idx]->name, getLangString(_L_APP_BLE), sizeof(mini_app_registry[idx]->name));
        break;
    case MINI_APP_ID_PLAYER:
            strncpy(mini_app_registry[idx]->name, getLangString(_L_APP_PLAYER), sizeof(mini_app_registry[idx]->name));
        break;
    case MINI_APP_ID_SETTINGS:
            strncpy(mini_app_registry[idx]->name, getLangString(_L_APP_SET), sizeof(mini_app_registry[idx]->name));
        break;
    
    default:
        break;
    }

    return mini_app_registry[idx];
}