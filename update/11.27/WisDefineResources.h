#pragma once

#include <string>
#include <chrono>

/**
 * This string specifies a Recognize event using the specified profile.
 *
 * CLOSE_TALK performs end-of-speech detection on the client, so no directive is sent from AVS to stop recording.
 * NEAR_FIELD performs end-of-speech detection in AVS, so a directive is sent from AVS to stop recording.
 */
#define RECOGNIZE_EVENT_JSON(PROFILE, DIALOG_REQUEST_ID )         \
    "{"                                                           \
        "\"event\":{"                                             \
            "\"payload\":{"                                       \
                "\"format\":\"AUDIO_L16_RATE_16000_CHANNELS_1\"," \
                "\"profile\":\"" #PROFILE "\""                    \
            "},"                                                  \
            "\"header\":{"                                        \
                "\"dialogRequestId\":\"" DIALOG_REQUEST_ID "\","  \
                "\"messageId\":\"messageId123\","                 \
                "\"name\":\"Recognize\","                         \
                "\"namespace\":\"SpeechRecognizer\""              \
            "}"                                                   \
        "},"                                                      \
        "\"context\":[{"                                          \
            "\"payload\":{"                                       \
                "\"activeAlerts\":[],"                            \
                "\"allAlerts\":[]"                                \
            "},"                                                  \
            "\"header\":{"                                        \
                "\"name\":\"AlertsState\","                       \
                "\"namespace\":\"Alerts\""                        \
            "}"                                                   \
        "},"                                                      \
        "{"                                                       \
            "\"payload\":{"                                       \
                "\"playerActivity\":\"IDLE\","                    \
                "\"offsetInMilliseconds\":0,"                     \
                "\"token\":\"\""                                  \
            "},"                                                  \
            "\"header\":{"                                        \
                "\"name\":\"PlaybackState\","                     \
                "\"namespace\":\"AudioPlayer\""                   \
            "}"                                                   \
        "},"                                                      \
        "{"                                                       \
            "\"payload\":{"                                       \
                "\"muted\":false,"                                \
                "\"volume\":0"                                    \
            "},"                                                  \
            "\"header\":{"                                        \
                "\"name\":\"VolumeState\","                       \
                "\"namespace\":\"Speaker\""                       \
            "}"                                                   \
        "},"                                                      \
        "{"                                                       \
            "\"payload\":{"                                       \
                "\"playerActivity\":\"FINISHED\","                \
                "\"offsetInMilliseconds\":0,"                     \
                "\"token\":\"\""                                  \
            "},"                                                  \
            "\"header\":{"                                        \
                "\"name\":\"SpeechState\","                       \
                "\"namespace\":\"SpeechSynthesizer\""             \
            "}"                                                   \
        "}]"                                                      \
    "}"


    extern std::string volumeChangedEvent;
    extern std::string muteChangedEvent;

    /// Payload for SpeechSynthesizer state when it is playing back audio.
/*    static const std::string SPEECH_SYNTHESIZER_PAYLOAD_PLAYING =
            "{"
                "\"playerActivity\":\"PLAYING\","
                "\"offsetInMilliseconds\":5,"
                "\"token\":\"\""
            "}";
    /// Payload for SpeechSynthesizer state when playback has finished.
    static const std::string SPEECH_SYNTHESIZER_PAYLOAD_FINISHED =
            "{"
                "\"playerActivity\":\"FINISHED\","
                "\"offsetInMilliseconds\":0,"
                "\"token\":\"\""
            "}";

    /// Payload for AudioPlayer.
    static const std::string AUDIO_PLAYER_PAYLOAD =
            "{"
                "\"playerActivity\":\"FINISHED\","
                "\"offsetInMilliseconds\":0,"
                "\"token\":\"\""
            "}";

    /// Payload for Alerts.
    static const std::string ALERTS_PAYLOAD =
            "{"
                "\"allAlerts\": ["
                    "{"
                        "\"token\": \"\","
                        "\"type\": \"TIMER\","
                        "\"scheduledTime\": \"\""
                    "}"
                "],"
                "\"activeAlerts\": ["
                        "{"
                        "\"token\": \"\","
                        "\"type\": \"TIMER\","
                        "\"scheduledTime\": \"\""
                    "}"
                "]"
            "}";*/

// This string to be used for Speak Directives which use the NAMESPACE_SPEECH_SYNTHESIZER namespace.
static const std::string NAME_VOLUME_STATE = "VolumeState";  // Bob add
// This string to be used for Speak Directives which use the NAMESPACE_SPEECH_SYNTHESIZER namespace.
static const std::string NAME_SPEAK = "Speak";
// This string to be used for Speak Directives which use the NAMESPACE_SPEECH_SYNTHESIZER namespace.
static const std::string NAME_RECOGNIZE = "Recognize";
// This string to be used for AdjustVolume Directives which use the NAMESPACE_SPEAKER namespace.
static const std::string NAME_ADJUST_VOLUME = "AdjustVolume";
// This string to be used for ExpectSpeech Directives which use the NAMESPACE_SPEECH_RECOGNIZER namespace.
static const std::string NAME_EXPECT_SPEECH = "ExpectSpeech";
// This string to be used for SetMute Directives which use the NAMESPACE_SPEAKER namespace.
static const std::string NAME_SET_MUTE = "SetMute";
// This string to be used for SpeechStarted Directives which use the NAMESPACE_SPEECH_SYNTHESIZER namespace.
static const std::string NAME_SPEECH_STARTED = "SpeechStarted";
// This string to be used for SpeechFinished Directives which use the NAMESPACE_SPEECH_SYNTHESIZER namespace.
static const std::string NAME_SPEECH_FINISHED = "SpeechFinished";
// This String to be used to register the SpeechRecognizer namespace to a DirectiveHandler.
static const std::string NAMESPACE_SPEECH_RECOGNIZER = "SpeechRecognizer";
// This String to be used to register the SpeechSynthesizer namespace to a DirectiveHandler.
static const std::string NAMESPACE_SPEECH_SYNTHESIZER = "SpeechSynthesizer";
// This String to be used to register the Speaker namespace to a DirectiveHandler.
static const std::string NAMESPACE_SPEAKER = "Speaker";
/// Namespace for Alerts.
static const std::string NAMESPACE_ALERTS("Alerts");
/// Namespace for AudioPlayer.
static const std::string NAMESPACE_AUDIO_PLAYER("AudioPlayer");
/// Namespace for Notifications.
static const std::string NAMESPACE_NOTIFICATIONS("Notifications");

/// Name for SpeechSynthesizer state.
static const std::string NAME_SPEECH_STATE("SpeechState");
/// Name for AudioPlayer state.
static const std::string NAME_PLAYBACK_STATE("PlaybackState");
/// Name for Alerts state.
static const std::string NAME_ALERTS_STATE("AlertsState");
/// Name for IndicatorState.
static const std::string NAME_INDICATOR_STATE("IndicatorState");
// King end

/// Default time @c doProvide sleeps before it calls @c setState.
static const std::chrono::milliseconds DEFAULT_SLEEP_TIME = std::chrono::milliseconds(10);
/// Time @c doProvide sleeps before it calls @c setState to induce a timeout.
static const std::chrono::milliseconds TIMEOUT_SLEEP_TIME = std::chrono::milliseconds(100);
/**
 * Default timeout for the @c ContextRequester to get the context.This needs to be modified if the
 * @c TIMEOUT_SLEEP_TIME is modified. The value should be less than the @c TIMEOUT_SLEEP_TIME.
 */
static const std::chrono::milliseconds DEFAULT_TIMEOUT = std::chrono::milliseconds(2000);
/// Timeout for the @c ContextRequester to get the failure.
static const std::chrono::milliseconds FAILURE_TIMEOUT = std::chrono::milliseconds(110);

//static const std::chrono::seconds WAIT_FOR_DIRECTIVE_TIMEOUT(5);

static const std::string APP_ROOT_FOLDER = "/usr/nut2_avs/";
static const std::string STORE_ROOT_FOLDER = "/data/nut2_avs/";
static const std::string SKU_DETECTION_PATH = "/sys/devices/soc0/platform_subtype";
static const std::string GET_DEVICE_NAME_PATH = "/data/misc/wifi/hostapd.conf";
static const int UDP_RECEIVE_MULTICAST_PORT = 6000;
static const int UDP_RECEIVE_IP_PORT        = 6002;
static const int UDP_SEND_IP_PORT           = 6001;
// King test
//static const std::string UDP_MULTICAST_IP_ADDRESS = "234.1.2.3";
static const std::string UDP_MULTICAST_IP_ADDRESS = "224.0.0.251";

static const std::string IMAGE_BUILD_VERSION_FILE_PATH = "/etc/version";
static const std::string IMAGE_BUILD_DATE_FILE_PATH = "/etc/timestamp";
static const std::string BATTERY_CAPACITY_FILE_PATH = "/sys/class/power_supply/battery/capacity";
static const std::string BATTERY_STATUS_FILE_PATH = "/sys/class/power_supply/battery/status";
static const std::string SKU_CX = "CX";
static const std::string SKU_WCD = "WCD";

static const std::string SKU_CX_DEFAULT_CONFIG = APP_ROOT_FOLDER + "cx_settings.cfg";
static const std::string SKU_WCD_DEFAULT_CONFIG = APP_ROOT_FOLDER + "wcd_settings.cfg";

static const std::string COMPANION_APP_SETTINGS_FILE_PATH = STORE_ROOT_FOLDER + "companion_app_settings.cfg";
static const std::string CONFIG_KEY_WAKEWORD_DETECTOR = "wakeWordDetector";
static const std::string CONFIG_KEY_RECORDING_SOURCE = "recordingSource";
static const std::string CONFIG_KEY_ENABLE_PRE_REC_SOUND_PROMPT = "enablePreRecSoundPrompt";
static const std::string CONFIG_KEY_ENABLE_POST_REC_SOUND_PROMPT = "enablePostRecSoundPrompt";
static const std::string CONFIG_KEY_ENABLE_REC_LED_PROMPT = "enableRecLEDPrompt";
static const std::string CONFIG_KEY_SVA_KEYWORD_CONFIDENCE_LEVEL = "svaKeywordConfidenceLevel";
static const std::string CONFIG_KEY_SVA_NUMBER_OF_KEYWORDS = "svaNumberOfKeywords";
static const std::string CONFIG_KEY_SVA_NUMBER_OF_USERS_PER_KEYWORD = "svaNumberOfUsersPerKeyword";
static const std::string CONFIG_KEY_RECORDING_TIMEOUT = "recordingTimeout";
static const std::string CONFIG_KEY_WAIT_DIRECTIVE_TIMEOUT = "waitDirectiveTimeout";
static const std::string CONFIG_KEY_WAIT_FOR_RESPONSE_TIMEOUT = "waitForResponseTimeout";
static const std::string CONFIG_KEY_SYNC_NTP_SERVER = "syncNTPServer";
static const std::string CONFIG_KEY_PING_ADDRESS = "pingAddress";
static const std::string CONFIG_KEY_ENABLE_SOUND_WHEN_RECEIVED_PHONE_CMD = "enableSoundWhenReceivedPhoneCmd";
static const std::string CONFIG_KEY_OTA_FTP_SERVER = "otaFTPServer";
static const std::string CONFIG_KEY_OTA_FTP_USER_NAME = "otaFTPUserName";
static const std::string CONFIG_KEY_OTA_FTP_PASSWORD = "otaFTPPassword";
static const std::string CONFIG_KEY_OTA_FTP_REMOTE_VERSION_FILE_PATH = "otaFTPRemoteVersionFilePath";

// Battery info
static const std::string BATTERY_INFO_KEY_CAPACITY = "capacity";

// Sound cue
static const std::string SOUND_CUE_TYPE_NOTIFICATION = "notification";
static const std::string SOUND_CUE_TYPE_PRE_RECORDING = "preRecording";
static const std::string SOUND_CUE_TYPE_POST_RECORDING = "postRecording";
static const std::string SOUND_CUE_TYPE_DONG= "dong";
static const std::string SOUND_CUE_TYPE_MIC_ON = "micOn";
static const std::string SOUND_CUE_TYPE_MIC_OFF = "micOff";

static const std::string DEFAULT_NTP_SERVER = "pool.ntp.org";
static const std::string DEFAULT_PING_ADDRESS = "www.bing.com";
static const int VOLUME_LEVEL = 10;

extern std::string configPath;
extern std::string inputPath;
extern std::string awsAccountPath;

// CMD between Nut2 with Phone
extern const char* CMD_FIND_DEVICES;
extern const char* CMD_PLAY_MUSIC;
extern const char* CMD_PLAY_LOCAL_MUSIC;
extern const char* CMD_SVA_AVS;
extern const char* CMD_RESET_WIFI;
extern const char* CMD_AVS_DATA;
extern const char* CMD_AVS_AUTH_CODE;
extern const char* CMD_AVS_CLEAR;
extern const std::string CMD_WIFI_SETTING;
extern const std::string CMD_WIFI_SETTING_ACTION_SCAN;
extern const std::string CMD_WIFI_SETTING_ACTION_SCAN_RESULT;
extern const std::string CMD_WIFI_SETTING_TAG_SSID;
extern const std::string CMD_WIFI_SETTING_TAG_ENCRYPTION;
extern const std::string CMD_WIFI_SETTING_TAG_PASSWORD;
extern const std::string CMD_WIFI_SETTING_TAG_LEVEL;
extern const std::string CMD_AVS_SETTING;
extern const std::string CMD_AVS_SETTING_ACTION_GET_DATA;
extern const std::string CMD_AVS_SETTING_ACTION_LOGIN_INFO;
extern const std::string CMD_AVS_SETTING_ACTION_LOGOUT;
extern const std::string CMD_AVS_SETTING_ACTION_LOGOUT_DONE;
extern const std::string CMD_AVS_SETTING_TAG_CLIENT_ID;
extern const std::string CMD_AVS_SETTING_TAG_REDIRECT_URI;
extern const std::string CMD_AVS_SETTING_TAG_AUTH_CODE;
extern const std::string CMD_AVS_SETTING_TAG_USER_NAME;
extern const std::string CMD_AVS_SETTING_TAG_USER_EMAIL;
extern const std::string CMD_AVS_SETTING_TAG_AVS_DATA;
extern const std::string CMD_AVS_SETTING_TAG_PRODUCT_ID;
extern const std::string CMD_AVS_SETTING_TAG_PRODUCT_DSN;
extern const std::string CMD_AVS_SETTING_TAG_CODE_VERIFIER;
extern const std::string CMD_AVS_SETTING_TAG_CODE_CHALLENGE;
extern const std::string CMD_AVS_SETTING_TAG_DEVICE_ONLINE;
extern const char* CMD_SET_LOCALE;
extern const std::string CMD_OTA_UPDATE;
extern const std::string CMD_OTA_UPDATE_ACTION_CHECK_CUR_VER;
extern const std::string CMD_OTA_UPDATE_ACTION_CHECK_UPDATE;
extern const std::string CMD_OTA_UPDATE_ACTION_UPGRADE;
extern const std::string CMD_OTA_UPDATE_ACTION_CANCEL;
extern const std::string CMD_OTA_UPDATE_TAG_CUR_VER;
extern const std::string CMD_OTA_UPDATE_TAG_NEW_VER;
extern const std::string CMD_OTA_UPDATE_TAG_UPGRADE_PROGRESS;
extern const char* CMD_ADVANCED_SETTING;
//Bob add for FM 20170821
extern const std::string CMD_FM_DATA;
extern const std::string CMD_FM_DATA_ENABLE;
extern const std::string CMD_FM_DATA_DISABLE;
extern const std::string CMD_FM_DATA_FREQUENCY;
extern const std::string CMD_FM_DATA_GET_VOLUME;
extern const std::string CMD_FM_DATA_SET_VOLUME;
extern const std::string CMD_FM_DATA_SEARCH;
//Bob end 20170821
extern const std::string CMD_REBOOT_DEVICE;
extern const std::string CMD_FACTORY_RESET;
extern const char* CMD_BATTERY_INFO;

// Shell script
static const std::string SHELL_SCRIPT_CX_PLAY_SOUND_CUE = "cx_play_sound_cue.sh";
static const std::string SHELL_SCRIPT_FACTORY_RESET = "factory_reset.sh";
static const std::string SHELL_SCRIPT_START_SOFT_AP_MODE = "start_soft_ap_mode.sh";
static const std::string SHELL_SCRIPT_OTA_UPGRADE = "ota_upgrade.sh";
static const std::string SHELL_SCRIPT_WCD_PLAY_SOUND_CUE = "wcd_play_sound_cue.sh";
static const std::string SHELL_SCRIPT_WIFI_RECONFIGURE = "wifi_reconfigure.sh";

// store current value
static const std::string STORE_PRODUCT_DATA_FILE_PATH = STORE_ROOT_FOLDER + "product_data.txt";
static const std::string STORE_REFRESH_TOKEN_FILE_PATH = STORE_ROOT_FOLDER + "refreshtoken.json";
static const std::string STORE_AUTH_CODE_FILE_PATH = STORE_ROOT_FOLDER + "auth_code.txt";
static const std::string STORE_AVS_ACCOUNT_FILE_PATH = STORE_ROOT_FOLDER + "avs_account.txt";
static const std::string STORE_CURRENT_VOLUME_FILE_PATH = STORE_ROOT_FOLDER + "volume.txt";

// extra
static const std::string LED_PROCESS_NAME = "nled_control";

// LED type
/*1. Listening
2. Microphones_Off
3. Notification_Arrives
4. Notifications_Queued
5. Speaking
6. System_Error
7. Thinking
8. Default*/
static const int LED_TYPE_LISTENING           = 1;
static const int LED_TYPE_MICRO_OFF           = 2;
static const int LED_TYPE_NOTIFICATION_ARRIVES = 3;
static const int LED_TYPE_NOTIFICATION_QUEUED = 4;
static const int LED_TYPE_SPEAKING = 5;
static const int LED_TYPE_SYSTEM_ERROR= 6;
static const int LED_TYPE_THINKING = 7;
static const int LED_TYPE_DEFAULT = 8;
static const int LED_TYPE_HOTSPOT_MODE = 9;
static const int LED_TYPE_LOW_BATTERY = 10;

// OTA Function
static const std::string CHECK_SERVICE_URL = "https://9kpz7si5j2.execute-api.us-west-2.amazonaws.com/pretest/nut2otaupdatecheckservice";
static const std::string LOG_SERVICE_URL = "https://9kpz7si5j2.execute-api.us-west-2.amazonaws.com/pretest/nut2otaupdatelogservice";
static std::string otaFTPServer = "mhdqftp.wistron.com.tw";
static std::string otaFTPUserName = "Nut2rw";
static std::string otaFTPPassword = "ie02dent";
static std::string otaFTPRemoteVersionFilePath = "/Internal/SW/temp/Nut2_OTA_test/version.txt";
static const std::string OTA_LOCAL_VERSION_FILE_PATH = STORE_ROOT_FOLDER + "version_check.txt";
static const std::string OTA_BUILD_VERSION_KEY = "version";
static const std::string OTA_BUILD_DATE_KEY = "build_date";
static const std::string OTA_BUILD_DATE_LONG_KEY = "build_date_long";
static const std::string OTA_IMAGE_ADDRESS_KEY = "image_address";
//static const std::string OTA_REMOTE_IMAGE_FILE = "/Internal/SW/Release/W2.0_20170823-0455/DB2.1_W2.0_20170823-0455.zip";
//static const std::string OTA_REMOTE_IMAGE_FILE = "/Internal/SW/Release/W2.0_20170823-0455/DB2.1_W2.0_20170823-0455.zip";
static const std::string OTA_LOCAL_IMAGE_FILE = STORE_ROOT_FOLDER + "update.zip";

// AVS definition
static const std::string AVS_DIRECTIVE_NAMESPACE_NOTIFICATIONS = "Notifications";

// Test
static const std::string SOUND_CUE_TEST_FILE_PATH = STORE_ROOT_FOLDER + "sound_cue_test.cfg";
static const std::string SOUND_CUE_TEST_KEY_DELAY_IN_MILL_SECONDS = "delayInMillSeconds";
