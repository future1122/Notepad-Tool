
#include "WisDefineResources.h"

std::string configPath = STORE_ROOT_FOLDER + "refreshtoken.json";  // Bob update
std::string inputPath = APP_ROOT_FOLDER + "inputs"; // King update
std::string awsAccountPath = APP_ROOT_FOLDER + "aws_account.cfg";//damon update
std::string volumeChangedEvent = "{"                              \
    "\"event\": {"                                                \
        "\"header\": {"                                           \
            "\"namespace\": \"Speaker\","                         \
            "\"name\": \"VolumeChanged\","                        \
            "\"messageId\": \"messageId123\""                     \
        "},"                                                      \
        "\"payload\": {"                                          \
            "\"volume\": current_volume,"                         \
            "\"muted\": isMuted"                                  \
        "}"                                                       \
    "}"                                                           \
"}";

std::string muteChangedEvent = "{"                                \
    "\"event\": {"                                                \
        "\"header\": {"                                           \
            "\"namespace\": \"Speaker\","                         \
            "\"name\": \"MuteChanged\","                          \
            "\"messageId\": \"messageId123\""                     \
        "},"                                                      \
        "\"payload\": {"                                          \
            "\"volume\": current_volume,"                         \
            "\"muted\": isMuted"                                  \
        "}"                                                       \
    "}"                                                           \
"}";


const char* CMD_FIND_DEVICES          ="find_devices";
const char* CMD_PLAY_MUSIC            ="play_music";
const char* CMD_PLAY_LOCAL_MUSIC      ="play_local_music";
const char* CMD_SVA_AVS               ="sva_avs";
const char* CMD_RESET_WIFI            ="reset_wifi";
const char* CMD_AVS_DATA              ="avs_data";
const char* CMD_AVS_AUTH_CODE         ="avs_auth_code";
const char* CMD_AVS_CLEAR             ="clear_avs_data";
const std::string CMD_WIFI_SETTING                      ="wifi_setting";;
const std::string CMD_WIFI_SETTING_ACTION_SCAN          ="scan";
const std::string CMD_WIFI_SETTING_ACTION_SCAN_RESULT   ="scan_result";
const std::string CMD_WIFI_SETTING_TAG_SSID             ="ssid";
const std::string CMD_WIFI_SETTING_TAG_ENCRYPTION       ="encryption";
const std::string CMD_WIFI_SETTING_TAG_PASSWORD         ="password";
const std::string CMD_WIFI_SETTING_TAG_LEVEL            ="level";
const std::string CMD_AVS_SETTING                       = "avs_setting";
const std::string CMD_AVS_SETTING_ACTION_GET_DATA       = "get_data";
const std::string CMD_AVS_SETTING_ACTION_LOGIN_INFO     = "login_info";
const std::string CMD_AVS_SETTING_ACTION_LOGOUT         = "logout";
const std::string CMD_AVS_SETTING_ACTION_LOGOUT_DONE    = "logout_done";
const std::string CMD_AVS_SETTING_TAG_CLIENT_ID         = "client_id";
const std::string CMD_AVS_SETTING_TAG_REDIRECT_URI      = "redirect_uri";
const std::string CMD_AVS_SETTING_TAG_AUTH_CODE         = "auth_code";
const std::string CMD_AVS_SETTING_TAG_USER_NAME         = "user_name";
const std::string CMD_AVS_SETTING_TAG_USER_EMAIL        = "user_email";
const std::string CMD_AVS_SETTING_TAG_AVS_DATA          = "avs_data";
const std::string CMD_AVS_SETTING_TAG_PRODUCT_ID        = "product_id";
const std::string CMD_AVS_SETTING_TAG_PRODUCT_DSN       = "product_dsn";
const std::string CMD_AVS_SETTING_TAG_CODE_VERIFIER     = "code_verifier";
const std::string CMD_AVS_SETTING_TAG_CODE_CHALLENGE    = "code_challenge";
const std::string CMD_AVS_SETTING_TAG_DEVICE_ONLINE     = "device_online";
const char* CMD_SET_LOCALE            ="locale";
const std::string CMD_OTA_UPDATE                        = "ota";
const std::string CMD_OTA_UPDATE_ACTION_CHECK_CUR_VER   = "check_cur_ver";
const std::string CMD_OTA_UPDATE_ACTION_CHECK_UPDATE    = "check_update";
const std::string CMD_OTA_UPDATE_ACTION_UPGRADE         = "upgrade";
const std::string CMD_OTA_UPDATE_ACTION_CANCEL          = "cancel";
const std::string CMD_OTA_UPDATE_TAG_CUR_VER            = "cur_ver";
const std::string CMD_OTA_UPDATE_TAG_NEW_VER            = "new_ver";
const std::string CMD_OTA_UPDATE_TAG_UPGRADE_PROGRESS   = "upgrade_progress";
const char* CMD_ADVANCED_SETTING      ="advanced_settings";
//Bob add for FM 20170821
const std::string CMD_FM_DATA                ="fm";
const std::string CMD_FM_DATA_ENABLE         ="enable";
const std::string CMD_FM_DATA_DISABLE        ="disable";
const std::string CMD_FM_DATA_FREQUENCY      ="frequency";
const std::string CMD_FM_DATA_GET_VOLUME     ="get_volume";
const std::string CMD_FM_DATA_SET_VOLUME     ="set_volume";
const std::string CMD_FM_DATA_SEARCH         ="search";
//Bob end 20170821
const std::string CMD_REBOOT_DEVICE    = "reboot_device";
const std::string CMD_FACTORY_RESET    = "factory_reset";
const char* CMD_BATTERY_INFO           ="battery_info";
