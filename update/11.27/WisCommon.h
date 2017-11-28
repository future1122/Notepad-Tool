#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <fstream>

// waitWlanReady
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <netinet/in.h>

// sleep
#include <unistd.h>

#define random(x) (rand()%x)

class WisCommon {
public:
    void sysLocalTime(const char* tag);
    std::string getCurrentSKU();
    void getSettingParametersFromDefaultConfig(int &wakeWordDetector, int &recordingSource, int &enablePreRecSoundPrompt, int &enablePostRecSoundPrompt, int &enableRecLEDPrompt,
                    int &svaKeywordConfidenceLevel, int &svaNumberOfKeywords, int &svaNumberOfUsersPerKeyword, int &recordingTimeout, int &waitDirectiveTimeout, int &waitForResponseTimeout,
                    std::string &syncNTPServer, std::string &pingAddress, int &enableSoundWhenReceivedPhoneCmd,
                    std::string otaFTPServer, std::string otaFTPUserName,std::string otaFTPPassword,std::string otaFTPRemoteVersionFilePath);
    void getSettingParametersFromCompanionConfig(int &wakeWordDetector, int &recordingSource, int &enablePreRecSoundPrompt, int &enablePostRecSoundPrompt, int &enableRecLEDPrompt,
                    int &svaKeywordConfidenceLevel, int &svaNumberOfKeywords, int &svaNumberOfUsersPerKeyword, int &recordingTimeout, int &waitDirectiveTimeout, int &waitForResponseTimeout,
                    std::string &syncNTPServer, std::string &pingAddress, int &enableSoundWhenReceivedPhoneCmd,
                    std::string otaFTPServer, std::string otaFTPUserName,std::string otaFTPPassword,std::string otaFTPRemoteVersionFilePath);
    std::string getValueWithKey(std::string file, std::string key);
    void subString(char *src, char *key, char *value);
    void waitWlanReady();
    std::string getDeviceName();
    std::string getIPAddress();
    void checkIPAddress();
    void pingAddress(std::string pingAddress);
    void syncNtpServer(std::string syncNTPServer);
    bool isWpaSupplicantProcessExist();
    void getAVSRequestResult(int &totalAVSRequestNum, int &failAVSRequestNum);
    void backupAVSFailedAudio(std::string filePath, int &totalAVSRequestNum, int &failAVSRequestNum);
    std::string replace_all(std::string str,std::string old_value,std::string new_value);
    std::string getCurrentImageBuildVersion();
    std::string getCurrentImageBuildDate();
    std::string getBatteryCapacity();
    std::string getBatteryStatus();
    void sendUDPDataToSender(std::string senderIPAddress, std::string sendContent);
    void saveCompanionSettings(int wakeWordDetector, int recordingSource, int enablePreRecSoundPrompt, int enablePostRecSoundPrompt,
                                      int svaKeywordConfidenceLevel, int svaNumberOfKeywords, int svaNumberOfUsersPerKeyword, int recordingTimeout,int waitDirectiveTimeout, int waitForResponseTimeout,
                                      std::string syncNTPServer, std::string pingAddress, int enableSoundWhenReceivedPhoneCmd,
                                      std::string otaFTPServer, std::string otaFTPUserName,std::string otaFTPPassword,std::string otaFTPRemoteVersionFilePath);
    void killProcess(std::string processName);
    void lightLED(int ledType);
    void turnOffSoftAPMode();
private:
    std::string getSettingsFilePath();
    void getTestParamFromSetting(std::string key, std::string &value);    // useless, reserved
    void getSettingParameters(std::string filePath, int &wakeWordDetector, int &recordingSource, int &enablePreRecSoundPrompt, int &enablePostRecSoundPrompt, int &enableRecLEDPrompt,
                    int &svaKeywordConfidenceLevel, int &svaNumberOfKeywords, int &svaNumberOfUsersPerKeyword, int &recordingTimeout, int &waitDirectiveTimeout, int &waitForResponseTimeout,
                    std::string &syncNTPServer, std::string &pingAddress, int &enableSoundWhenReceivedPhoneCmd,
                    std::string otaFTPServer, std::string otaFTPUserName,std::string otaFTPPassword,std::string otaFTPRemoteVersionFilePath);
    bool isLedLighting = false;
    int lastLedType = -1;
};
