#include "ApplicationSettings.h"
#include "ProgramVector.h"
#include "ServiceCall.h"


bool ApplicationSettings::reset(){
    void (*reset_callback)(void) = NULL;
    void* reset_args[] = {(void*)SYSTEM_FUNCTION_SETTINGS_RESET, &reset_callback};
    int ret = getProgramVector()->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, reset_args, 2);
    if (ret == OWL_SERVICE_OK) {
        reset_callback();
        return true;
    }
    else{
        return false;
    }
}

bool ApplicationSettings::storeToFlash(){
    if (!written || multipleWrites) {
        void (*store_callback)(void) = NULL;
        void* store_args[] = {(void*)SYSTEM_FUNCTION_SETTINGS_STORE, &store_callback};
        int ret = getProgramVector()->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, store_args, 2);
        if (ret == OWL_SERVICE_OK) {
            store_callback();
            written = true;
            return true;
        }
    }
    return false;
}

bool ApplicationSettings::loadFromFlash(){
    void (*load_callback)(void) = NULL;
    void* load_args[] = {(void*)SYSTEM_FUNCTION_SETTINGS_LOAD, &load_callback};
    int ret = getProgramVector()->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, load_args, 2);
    if (ret == OWL_SERVICE_OK) {
        load_callback();
        return true;
    }
    else {
        return false;
    }
}