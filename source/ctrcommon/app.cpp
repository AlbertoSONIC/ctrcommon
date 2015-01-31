#include "ctrcommon/common.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <3ds.h>

bool amInitialized = false;
bool nsInitialized = false;

bool am_prepare() {
    if(!amInitialized && amInit() == 0) {
        amInitialized = true;
    }

    return amInitialized;
}

bool ns_prepare() {
    if(!nsInitialized && nsInit() == 0) {
        nsInitialized = true;
    }

    return nsInitialized;
}

u8 app_mediatype_to_byte(MediaType mediaType) {
    return mediaType == NAND ? mediatype_NAND : mediatype_SDMC;
}

AppPlatform app_platform_from_id(u16 id) {
    switch(id) {
        case 1:
            return WII;
        case 3:
            return DSI;
        case 4:
            return THREEDS;
        case 5:
            return WIIU;
        default:
            return UNKNOWN_PLATFORM;
    }
}

AppCategory app_category_from_id(u16 id) {
    if((id & 0x2) == 0x2) {
        return DLC;
    } else if((id & 0x6) == 0x6) {
        return PATCH;
    } else if((id & 0x10) == 0x10) {
        return SYSTEM;
    } else if((id & 0x8000) == 0x8000) {
        return TWL;
    }

    return APP;
}

const std::string app_get_platform_name(AppPlatform platform) {
    switch(platform) {
        case WII:
            return "Wii";
        case DSI:
            return "DSi";
        case THREEDS:
            return "3DS";
        case WIIU:
            return "Wii U";
        default:
            return "Unknown";
    }
}

const std::string app_get_category_name(AppCategory category) {
    switch(category) {
        case APP:
            return "App";
        case DLC:
            return "DLC";
        case PATCH:
            return "Patch";
        case SYSTEM:
            return "System";
        case TWL:
            return "TWL";
        default:
            return "Unknown";
    }
}

std::vector<App> app_list(MediaType mediaType) {
    std::vector<App> titles;
    if(!am_prepare()) {
        return titles;
    }

    u32 titleCount;
    if(AM_GetTitleCount(app_mediatype_to_byte(mediaType), &titleCount) != 0) {
        return titles;
    }

    u64 titleIds[titleCount];
    if(AM_GetTitleList(app_mediatype_to_byte(mediaType), titleCount, titleIds) != 0) {
        return titles;
    }

    for(u32 i = 0; i < titleCount; i++) {
        u64 titleId = titleIds[i];
        App app;
        app.titleId = titleId;
        app.uniqueId = ((u32*) &titleId)[0];
        AM_GetTitleProductCode(app_mediatype_to_byte(mediaType), titleId, app.productCode);
        if(strcmp(app.productCode, "") == 0) {
            strcpy(app.productCode, "<N/A>");
        }

        app.mediaType = mediaType;
        app.platform = app_platform_from_id(((u16*) &titleId)[3]);
        app.category = app_category_from_id(((u16*) &titleId)[2]);

        titles.push_back(app);
    }

    return titles;
}

bool app_install(MediaType mediaType, const std::string path, std::function<bool(int)> onProgress) {
    if(!am_prepare()) {
        return false;
    }

    FILE* fd = fopen(path.c_str(), "r");
    if(!fd) {
        return false;
    }

    fseek(fd, 0, SEEK_END);
    u64 size = (u64) ftell(fd);
    fseek(fd, 0, SEEK_SET);

    if(onProgress != NULL) {
        onProgress(0);
    }

    Handle ciaHandle;
    if(AM_StartCiaInstall(app_mediatype_to_byte(mediaType), &ciaHandle) != 0) {
        return false;
    }

    FSFILE_SetSize(ciaHandle, size);

    u32 bufSize = 1024 * 256; // 256KB
    void* buf = malloc(bufSize);
    bool cancelled = false;
    for(u64 pos = 0; pos < size; pos += bufSize) {
        if(onProgress != NULL && !onProgress((int) ((pos / (float) size) * 100))) {
            AM_CancelCIAInstall(&ciaHandle);
            cancelled = true;
            break;
        }

        u32 bytesRead = fread(buf, 1, bufSize, fd);
        FSFILE_Write(ciaHandle, NULL, pos, buf, bytesRead, FS_WRITE_NOFLUSH);
    }

    free(buf);
    fclose(fd);

    if(cancelled) {
        return false;
    }

    if(onProgress != NULL) {
        onProgress(100);
    }

    Result res = AM_FinishCiaInstall(app_mediatype_to_byte(mediaType), &ciaHandle);
    if(res != 0 && (u32) res != 0xC8A044DC) { // Happens when already installed, but seems to have succeeded anyway...
        return false;
    }

    return true;
}

bool app_delete(App app) {
    if(!am_prepare()) {
        return false;
    }

    return AM_DeleteAppTitle(app_mediatype_to_byte(app.mediaType), app.titleId) == 0;
}

bool app_launch(App app) {
    if(!ns_prepare()) {
        return false;
    }

    return NS_RebootToTitle(app.mediaType, app.titleId) == 0;
}