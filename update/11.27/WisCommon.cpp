#include "WisCommon.h"
#include <dirent.h>
#include <unistd.h>
#include "WisDefineResources.h"

std::string WisCommon::getCurrentSKU(){
  std::string currentSKU = SKU_WCD;
  std::ifstream fin(SKU_DETECTION_PATH, std::ios::in);
  if (fin.is_open()) {
    std::string line;
    if(getline(fin,line)) {
      printf("SKU detect: %s\n", line.c_str());
      if (line.find(SKU_CX.c_str(), 0) == 0) {
        currentSKU = SKU_CX;
      } else if (line.find(SKU_WCD.c_str(), 0) == 0) {
        currentSKU = SKU_WCD;
      }
    }
    fin.clear();
    fin.close();
  }
  printf("Current SKU is: %s\n", currentSKU.c_str());
  return currentSKU;
}

std::string WisCommon::getSettingsFilePath(){
  std::string currentSKU = this->getCurrentSKU();
  std::string settingsFiePath = SKU_WCD_DEFAULT_CONFIG;
  if (currentSKU == SKU_CX) {
    settingsFiePath = SKU_CX_DEFAULT_CONFIG;
  }
  return settingsFiePath;
}

void WisCommon::getTestParamFromSetting(std::string key, std::string &value){  // useless, reserved
    if(strcmp(key.c_str(), CONFIG_KEY_SYNC_NTP_SERVER.c_str()) == 0){
       value = DEFAULT_NTP_SERVER;  // default NTP server
    }else if(strcmp(key.c_str(), CONFIG_KEY_PING_ADDRESS.c_str()) == 0){
      value = DEFAULT_PING_ADDRESS;  // default Ping address
    }

    std::ifstream fin(getSettingsFilePath(), std::ios::in);
    if (fin.is_open()) {
      char line[1024]={0};

      while(fin.getline(line, sizeof(line))) {
        if(line[0] == '#' || strlen(line) <= 0){
          continue;
        }
        char *p = strtok(line, "=");
        if(strcmp(p, key.c_str()) == 0) {
           p = strtok(NULL, "=");
           value = p;
           printf("&&& SettingParameters::  &&& -->  %s get param is: %s\n", key.c_str(), value.c_str());
           break;
        }
      }
      fin.clear();
      fin.close();
    }
}

void WisCommon::sysLocalTime(const char* tag){
  struct timeval tv;
  gettimeofday(&tv,NULL);

  time_t t;
  struct tm *p;
  t=tv.tv_sec + 28800; //加8小时
  p=gmtime(&t);
  char s[80];
  strftime(s, 80, "%Y-%m-%d %H:%M:%S", p);
  printf("\t\t%s --> %s:%ld\n", tag, s,tv.tv_usec/1000);
}

void WisCommon::getSettingParameters(std::string filePath, int &wakeWordDetector, int &recordingSource, int &enablePreRecSoundPrompt, int &enablePostRecSoundPrompt, int &enableRecLEDPrompt,
                int &svaKeywordConfidenceLevel, int &svaNumberOfKeywords, int &svaNumberOfUsersPerKeyword, int &recordingTimeout, int &waitDirectiveTimeout, int &waitForResponseTimeout,
                std::string &syncNTPServer, std::string &pingAddress, int &enableSoundWhenReceivedPhoneCmd,
                std::string otaFTPServer, std::string otaFTPUserName,std::string otaFTPPassword,std::string otaFTPRemoteVersionFilePath) {
  printf("Loading settings file: %s\n", filePath.c_str());
  std::ifstream fin(filePath, std::ios::in);
  if (fin.is_open()) {
    char line[1024]={0};

    while(fin.getline(line, sizeof(line))) {
      if(line[0] == '#' || strlen(line) <= 0){
        continue;
      }
      char *p = strtok(line, "=");
       if(strcmp(p, CONFIG_KEY_WAKEWORD_DETECTOR.c_str()) == 0) {
         p = strtok(NULL, "=");
         wakeWordDetector = atoi(p);
         printf("&&& SettingParameters::  &&& -->  wakeWordDetector = %d\n", wakeWordDetector);
      } else if (strcmp(p, CONFIG_KEY_RECORDING_SOURCE.c_str()) == 0) {
        p = strtok(NULL, "=");
        recordingSource = atoi(p);
        printf("&&& SettingParameters::  &&& -->  recordingSource = %d\n", recordingSource);
      } else if(strcmp(p, CONFIG_KEY_ENABLE_PRE_REC_SOUND_PROMPT.c_str()) == 0) {
         p = strtok(NULL, "=");
         enablePreRecSoundPrompt = atoi(p);
         printf("&&& SettingParameters::  &&& -->  enablePreRecSoundPrompt = %d\n", enablePreRecSoundPrompt);
      } else if(strcmp(p, CONFIG_KEY_ENABLE_POST_REC_SOUND_PROMPT.c_str()) == 0) {
         p = strtok(NULL, "=");
         enablePostRecSoundPrompt = atoi(p);
         printf("&&& SettingParameters::  &&& -->  enablePostRecSoundPrompt = %d\n", enablePostRecSoundPrompt);
      } else if(strcmp(p, CONFIG_KEY_ENABLE_REC_LED_PROMPT.c_str()) == 0) {
         p = strtok(NULL, "=");
         enableRecLEDPrompt = atoi(p);
         printf("&&& SettingParameters::  &&& -->  enableRecLEDPrompt = %d\n", enableRecLEDPrompt);
      } else if(strcmp(p, CONFIG_KEY_SVA_KEYWORD_CONFIDENCE_LEVEL.c_str()) == 0) {
         p = strtok(NULL, "=");
         svaKeywordConfidenceLevel = atoi(p);
         printf("&&& SettingParameters::  &&& -->  svaKeywordConfidenceLevel = %d\n", svaKeywordConfidenceLevel);
      } else if(strcmp(p, CONFIG_KEY_SVA_NUMBER_OF_KEYWORDS.c_str()) == 0) {
         p = strtok(NULL, "=");
         svaNumberOfKeywords = atoi(p);
         printf("&&& SettingParameters::  &&& -->  svaNumberOfKeywords = %d\n", svaNumberOfKeywords);
      } else if(strcmp(p, CONFIG_KEY_SVA_NUMBER_OF_USERS_PER_KEYWORD.c_str()) == 0) {
         p = strtok(NULL, "=");
         svaNumberOfUsersPerKeyword = atoi(p);
         printf("&&& SettingParameters::  &&& -->  svaNumberOfUsersPerKeyword = %d\n", svaNumberOfUsersPerKeyword);
      } else if(strcmp(p, CONFIG_KEY_RECORDING_TIMEOUT.c_str()) == 0) {
        p = strtok(NULL, "=");
        recordingTimeout = atoi(p);
        printf("&&& SettingParameters::  &&& -->  recordingTimeout = %d\n", recordingTimeout);
      } else if(strcmp(p, CONFIG_KEY_WAIT_DIRECTIVE_TIMEOUT.c_str()) == 0) {
        p = strtok(NULL, "=");
        waitDirectiveTimeout = atoi(p);
        printf("&&& SettingParameters::  &&& -->  waitDirectiveTimeout = %d\n", waitDirectiveTimeout);
      } else if(strcmp(p, CONFIG_KEY_WAIT_FOR_RESPONSE_TIMEOUT.c_str()) == 0) {
        p = strtok(NULL, "=");
        waitForResponseTimeout = atoi(p);
        printf("&&& SettingParameters::  &&& -->  waitForResponseTimeout = %d\n", waitForResponseTimeout);
      } else if(strcmp(p, CONFIG_KEY_SYNC_NTP_SERVER.c_str()) == 0) {
        p = strtok(NULL, "=");
        syncNTPServer = p;
        printf("&&& SettingParameters::  &&& -->  syncNTPServer = %s\n", syncNTPServer.c_str());
      } else if(strcmp(p, CONFIG_KEY_PING_ADDRESS.c_str()) == 0) {
        p = strtok(NULL, "=");
        pingAddress = p;
        printf("&&& SettingParameters::  &&& -->  pingAddress = %s\n", pingAddress.c_str());
      } else if(strcmp(p, CONFIG_KEY_ENABLE_SOUND_WHEN_RECEIVED_PHONE_CMD.c_str()) == 0) {
         p = strtok(NULL, "=");
         enableSoundWhenReceivedPhoneCmd = atoi(p);
         printf("&&& SettingParameters::  &&& -->  enableSoundWhenReceivedPhoneCmd = %d\n", enableSoundWhenReceivedPhoneCmd);
      }else if(strcmp(p, CONFIG_KEY_OTA_FTP_SERVER.c_str()) == 0) {
        p = strtok(NULL, "=");
        otaFTPServer = p;
        printf("&&& SettingParameters::  &&& -->  otaFTPServer = %s\n", otaFTPServer.c_str());
      } else if(strcmp(p, CONFIG_KEY_OTA_FTP_USER_NAME.c_str()) == 0) {
        p = strtok(NULL, "=");
        otaFTPUserName = p;
        printf("&&& SettingParameters::  &&& -->  otaFTPUserName = %s\n", otaFTPUserName.c_str());
      } else if(strcmp(p, CONFIG_KEY_OTA_FTP_PASSWORD.c_str()) == 0) {
        p = strtok(NULL, "=");
        otaFTPPassword = p;
        printf("&&& SettingParameters::  &&& -->  otaFTPPassword = %s\n", otaFTPPassword.c_str());
      } else if(strcmp(p, CONFIG_KEY_OTA_FTP_REMOTE_VERSION_FILE_PATH.c_str()) == 0) {
        p = strtok(NULL, "=");
        otaFTPRemoteVersionFilePath = p;
        printf("&&& SettingParameters::  &&& -->  otaFTPRemoteVersionFilePath = %s\n", otaFTPRemoteVersionFilePath.c_str());
      }
    }
    fin.clear();
    fin.close();
  }
}

void WisCommon::getSettingParametersFromDefaultConfig(int &wakeWordDetector, int &recordingSource, int &enablePreRecSoundPrompt, int &enablePostRecSoundPrompt, int &enableRecLEDPrompt,
                int &svaKeywordConfidenceLevel, int &svaNumberOfKeywords, int &svaNumberOfUsersPerKeyword, int &recordingTimeout, int &waitDirectiveTimeout, int &waitForResponseTimeout,
                std::string &syncNTPServer, std::string &pingAddress, int &enableSoundWhenReceivedPhoneCmd,
                std::string otaFTPServer, std::string otaFTPUserName,std::string otaFTPPassword,std::string otaFTPRemoteVersionFilePath) {
  getSettingParameters(getSettingsFilePath(), wakeWordDetector, recordingSource, enablePreRecSoundPrompt, enablePostRecSoundPrompt,
                enableRecLEDPrompt, svaKeywordConfidenceLevel, svaNumberOfKeywords, svaNumberOfUsersPerKeyword, recordingTimeout, waitDirectiveTimeout, waitForResponseTimeout,
                syncNTPServer, pingAddress, enableSoundWhenReceivedPhoneCmd,
                otaFTPServer, otaFTPUserName, otaFTPPassword, otaFTPRemoteVersionFilePath);
}

void WisCommon::getSettingParametersFromCompanionConfig(int &wakeWordDetector, int &recordingSource, int &enablePreRecSoundPrompt, int &enablePostRecSoundPrompt, int &enableRecLEDPrompt,
                int &svaKeywordConfidenceLevel, int &svaNumberOfKeywords, int &svaNumberOfUsersPerKeyword, int &recordingTimeout, int &waitDirectiveTimeout, int &waitForResponseTimeout,
                std::string &syncNTPServer, std::string &pingAddress, int &enableSoundWhenReceivedPhoneCmd,
                std::string otaFTPServer, std::string otaFTPUserName,std::string otaFTPPassword,std::string otaFTPRemoteVersionFilePath) {
  getSettingParameters(COMPANION_APP_SETTINGS_FILE_PATH, wakeWordDetector, recordingSource, enablePreRecSoundPrompt, enablePostRecSoundPrompt,
                enableRecLEDPrompt, svaKeywordConfidenceLevel, svaNumberOfKeywords, svaNumberOfUsersPerKeyword, recordingTimeout, waitDirectiveTimeout, waitForResponseTimeout,
                syncNTPServer, pingAddress, enableSoundWhenReceivedPhoneCmd,
                otaFTPServer, otaFTPUserName, otaFTPPassword, otaFTPRemoteVersionFilePath);
}

std::string WisCommon::getValueWithKey(std::string file, std::string key) {
  std::string value;
  std::ifstream fin(file, std::ios::in);
  if (fin.is_open()) {
    printf("Succeed to open file: %s\n", file.c_str());
    char line[1024]={0};

    while(fin.getline(line, sizeof(line)))
    {
      //if(line.compare(0, 1, "#") == 0){
      if(line[0] == '#' || strlen(line) <= 0){
        continue;
      }
      char *p = strtok(line, "=");
      if(strcmp(p, key.c_str()) == 0) {
         p = strtok(NULL, "=");
         if (p) {
           value = p;
           printf("&&& SettingParameters::  &&& -->  %s = %s\n", key.c_str(), value.c_str());
         }
         break;
      }
    }
    fin.clear();
    fin.close();
  }
  return value;
}

void WisCommon::subString(char *src, char *key, char *value){
  char *start, *end;
  start = src;
  while(1){
    if (src[0] == '=') {
      break;
    }

    src++;
  }
  end = src;
  int len = end - start;
  memcpy(key,start,len);
  memcpy(value,end+1, strlen(start));

  printf("subString key = %s, value=%s\n", key,value);
}

void WisCommon::waitWlanReady(){
  // detect Wifi module is ready
  int isReady=0;
  struct ifaddrs *ifap, *ifa;
  int family;
  while (1) {
    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        family = ifa->ifa_addr->sa_family;
        if (family == AF_PACKET) {
            //printf("interface name is: %s, running: %d\n", ifa->ifa_name, ifa->ifa_flags);
            if (strcmp(ifa->ifa_name, "wlan0") == 0) {
              isReady = 1;
              break;
            }
        }
    }
    if (isReady == 1) {
      break;
    }
    printf("\tNut2AVS: Waiting for Wlan module ready......\n");
    sleep(1);
  }
  freeifaddrs(ifap);
}

std::string WisCommon::getDeviceName(){
  std::string deviceName = "";
  std::ifstream fin(GET_DEVICE_NAME_PATH, std::ios::in);
  if (fin.is_open()) {
    std::string line;
    while(getline(fin,line)) {
      //ssid=NUT2_09d405fb
      if (line.find("ssid", 0) == 0) {
        deviceName = line.substr(5); // ssid=
        break;
      }
    }
    fin.clear();
    fin.close();
  }
  printf("WisCommon getDeviceName: %s\n", deviceName.c_str());
  return deviceName;
}
std::string WisCommon::getIPAddress(){
  std::string ipAddress;
  char buff[200];
  FILE *fp = popen("ifconfig wlan0|grep 'inet addr'|awk -F'[ :]' '{print $13}'", "r");
  if(fp != NULL){
      if(fgets(buff, sizeof(buff), fp)){
         printf("ifconfig wlan0: %s\n",buff);
         ipAddress = buff;
      }
  }
  pclose(fp);
  if (ipAddress.find("\n") != std::string::npos) {
    ipAddress = replace_all(ipAddress, "\n", "\0");
  }
  printf("WisCommon getIPAddress: %s\n", ipAddress.c_str());
  return ipAddress;
}

void WisCommon::checkIPAddress(){
  // Check IP address
  char buff[200];
  int isReady = 0;
  while (1) {
    FILE *fp = popen("ifconfig wlan0|grep 'inet addr'|awk -F'[ :]' '{print $13}'", "r");
    if(fp != NULL){
        while(fgets(buff, sizeof(buff), fp)){
           printf("ifconfig wlan0: %s\n",buff);
           if(strstr(buff, ".")){
              isReady = 1;
           }
        }
    }
    pclose(fp);

    if (isReady == 1) {
      break;
    }
    printf("\tNut2AVS: Waiting for get wlan0 IP address......\n");
    sleep(2);
  }
}

void WisCommon::pingAddress(std::string pingAddress){
  bool isPingSuccess = false;

  std::string mCmd = "ping -I wlan0 -c 2 "+pingAddress;
  printf("ping command is: %s\n", mCmd.c_str());
  char buff[200];
  const std::string compareContent = "64 bytes from";
  while (!isPingSuccess) {
    int passCount = 0;
    FILE *fp = popen(mCmd.c_str(), "r");
    if(fp != NULL){
        while(fgets(buff, sizeof(buff), fp)){
	         printf("WisCommon::pingAddress--> %s\n",buff);
           if(strncmp(buff, compareContent.c_str(), strlen(compareContent.c_str())) == 0){
              passCount++;
           }
        }
    }
    pclose(fp);
    if (passCount >= 1) {
      isPingSuccess = true;
    }else{
      sleep(2);
    }
  }
}

/*void WisCommon::syncNtpServer(std::string syncNTPServer){
   bool isSyncSuccess = false;

   std::string mSyncCmd = "ntpd -p "+syncNTPServer+" -qNn";
   printf("sync Ntp server command is: %s\n", mSyncCmd.c_str());
   while (!isSyncSuccess) {
     system(mSyncCmd.c_str());
     pid_t status = system("echo $?");
     int returnCode = WEXITSTATUS(status);
     if (returnCode == 0) {
        isSyncSuccess = true;
        printf("*** Succeed in syncing from NTP server\n");
     } else {
        printf(">>> Fail in syncing from NTP server, try it again\n");
     }

     sleep(1);
   }
}*/

void WisCommon::syncNtpServer(std::string syncNTPServer){
   bool isSyncSuccess = false;

   std::string mSyncCmd = "ntpdate "+syncNTPServer+";echo $?";
   printf("sync Ntp server command is: %s\n", mSyncCmd.c_str());

   char buff[200]={0};
   while (!isSyncSuccess) {
     FILE *fp = popen(mSyncCmd.c_str(), "r");
     if(fp != NULL){
         std::string outputResult="";
         while(fgets(buff, sizeof(buff), fp)){
           outputResult = buff;
           if (outputResult.find("\n") != std::string::npos) {
             outputResult = replace_all(outputResult, "\n", "\0");
           }
           printf("WisCommon::syncNtpServer--> %s,%d\n",outputResult.c_str(),strlen(outputResult.c_str()));
         }
         printf("WisCommon::syncNtpServer result is--> %s\n",outputResult.c_str());
         if (outputResult == "0") {  // 0 Success; 1 Fail
           isSyncSuccess = true;
           break;
         }
     }
     pclose(fp);
     if (!isSyncSuccess) {
       sleep(3);
     }
   }
}

bool WisCommon::isWpaSupplicantProcessExist(){
  bool isExist = false;

  char buff[200];
  FILE *fp = popen("ps | grep wpa_supplicant", "r");
  if(fp != NULL){
    while(fgets(buff, sizeof(buff), fp)){
      printf("%s\n",buff);
      if (strstr(buff,"/data/misc/wifi/wpa_supplicant.conf")) {
        isExist = true;
        break;
      }
    }
  }
  pclose(fp);

  if (isExist) {
    printf("wpa_supplicant process is exist!\n");
  }else{
    printf("wpa_supplicant process is not exist!\n");
  }

  return isExist;
}

void WisCommon::getAVSRequestResult(int &totalAVSRequestNum, int &failAVSRequestNum){
  std::ifstream fin(STORE_ROOT_FOLDER + "recording/avs_request_count.txt", std::ios::in);
  if (fin.is_open()) {
    char line[1024]={0};

    while(fin.getline(line, sizeof(line))) {
      if(line[0] == '#' || strlen(line) <= 0){
        continue;
      }
      char *p = strtok(line, "=");
      if(strcmp(p, "total") == 0) {
         p = strtok(NULL, "=");
         totalAVSRequestNum = atoi(p);
         printf("&&& getAVSRequestResult::  &&& -->  totalAVSRequestNum = %d\n", totalAVSRequestNum);
      } else if(strcmp(p, "fail") == 0) {
         p = strtok(NULL, "=");
         failAVSRequestNum = atoi(p);
         printf("&&& getAVSRequestResult::  &&& -->  failAVSRequestNum = %d\n", failAVSRequestNum);
      }
    }
    fin.clear();
    fin.close();
  }
}

void WisCommon::backupAVSFailedAudio(std::string filePath, int &totalAVSRequestNum, int &failAVSRequestNum){
  system(("mkdir " + STORE_ROOT_FOLDER + "recording").c_str());

  // delete the first file if count >= 50
  char buff[200];
  FILE *fp = popen(("ls -l " + STORE_ROOT_FOLDER + "recording/ |grep '^-' | wc -l").c_str(), "r");
  if(fp != NULL){
      if(fgets(buff, sizeof(buff), fp)){
         printf("get recording file count: %s\n",buff);
         if(atoi(buff) >= 50){
           struct   dirent    *ptr;
           DIR *dir = opendir((STORE_ROOT_FOLDER + "recording/").c_str()); ///open the dir
           while((ptr = readdir(dir)) != NULL) ///read the list of this dir
           {
             char *find = strstr(ptr->d_name,".wav");
             if(find){
               if(strcmp(find,".wav") == 0){
                 std::string cmd = "rm " + STORE_ROOT_FOLDER + "recording/";
                 cmd.append(ptr->d_name);
                 printf("To execute: %s\n", cmd.c_str());
                 system(cmd.c_str());
                 break;
               }
             }
           }
           closedir(dir);
         }
      }
  }
  pclose(fp);

  // copy
  std::string copyCmd = "cp ";
  copyCmd.append(filePath);
  std::string fileName = " " + STORE_ROOT_FOLDER + "recording/recording_";
  struct timeval tv;
  gettimeofday(&tv,NULL);

  time_t t;
  struct tm *p;
  t=tv.tv_sec;
  p=gmtime(&t);
  char s[80];
  strftime(s, 80, "%Y%m%d%H%M%S", p);
  fileName.append(s);
  fileName.append(".wav");

  copyCmd.append(fileName);
  printf("To execute copy command: %s\n", copyCmd.c_str());
  system(copyCmd.c_str());

  // save AVS fail rate to fileName
  std::ofstream avsCountFileStream(STORE_ROOT_FOLDER + "recording/avs_request_count.txt");
  if (avsCountFileStream.is_open()) {
    avsCountFileStream << "total=" << totalAVSRequestNum << "\n";
    avsCountFileStream << "fail=" << failAVSRequestNum << "\n";
    int failRate = 0;
    if (totalAVSRequestNum > 0 && failAVSRequestNum > 0) {
      failRate = (failAVSRequestNum * 100) / totalAVSRequestNum;
    }
    avsCountFileStream << "failRate=" << failRate << " %" << "\n";
    avsCountFileStream.close();
  }
}
void WisCommon::saveCompanionSettings(int wakeWordDetector, int recordingSource, int enablePreRecSoundPrompt, int enablePostRecSoundPrompt,
                                  int svaKeywordConfidenceLevel, int svaNumberOfKeywords, int svaNumberOfUsersPerKeyword, int recordingTimeout, int waitDirectiveTimeout, int waitForResponseTimeout,
                                  std::string syncNTPServer, std::string pingAddress, int enableSoundWhenReceivedPhoneCmd,
                                  std::string otaFTPServer, std::string otaFTPUserName,std::string otaFTPPassword,std::string otaFTPRemoteVersionFilePath) {
  std::ofstream companionSettingsFileStream(COMPANION_APP_SETTINGS_FILE_PATH, std::ios::trunc);
  if (companionSettingsFileStream.is_open()) {
    companionSettingsFileStream << CONFIG_KEY_WAKEWORD_DETECTOR << "=" << wakeWordDetector << "\n";
    companionSettingsFileStream << CONFIG_KEY_RECORDING_SOURCE << "=" << recordingSource << "\n";
    companionSettingsFileStream << CONFIG_KEY_ENABLE_PRE_REC_SOUND_PROMPT << "=" << enablePreRecSoundPrompt << "\n";
    companionSettingsFileStream << CONFIG_KEY_ENABLE_POST_REC_SOUND_PROMPT << "=" << enablePostRecSoundPrompt << "\n";

    companionSettingsFileStream << CONFIG_KEY_SVA_KEYWORD_CONFIDENCE_LEVEL <<"=" << svaKeywordConfidenceLevel << "\n";
    companionSettingsFileStream << CONFIG_KEY_SVA_NUMBER_OF_KEYWORDS << "=" << svaNumberOfKeywords << "\n";
    companionSettingsFileStream << CONFIG_KEY_SVA_NUMBER_OF_USERS_PER_KEYWORD << "=" << svaNumberOfUsersPerKeyword << "\n";
    companionSettingsFileStream << CONFIG_KEY_RECORDING_TIMEOUT << "=" << recordingTimeout << "\n";
    companionSettingsFileStream << CONFIG_KEY_WAIT_DIRECTIVE_TIMEOUT << "=" << waitDirectiveTimeout << "\n";
    companionSettingsFileStream << CONFIG_KEY_WAIT_FOR_RESPONSE_TIMEOUT << "=" << waitForResponseTimeout << "\n";

    companionSettingsFileStream << CONFIG_KEY_SYNC_NTP_SERVER << "=" << syncNTPServer << "\n";
    companionSettingsFileStream << CONFIG_KEY_PING_ADDRESS << "=" << pingAddress << "\n";
    companionSettingsFileStream << CONFIG_KEY_ENABLE_SOUND_WHEN_RECEIVED_PHONE_CMD <<  "=" << enableSoundWhenReceivedPhoneCmd << "\n";

    companionSettingsFileStream << CONFIG_KEY_OTA_FTP_SERVER << "=" << otaFTPServer << "\n";
    companionSettingsFileStream << CONFIG_KEY_OTA_FTP_USER_NAME <<  "=" << otaFTPUserName << "\n";
    companionSettingsFileStream << CONFIG_KEY_OTA_FTP_PASSWORD << "=" << otaFTPPassword << "\n";
    companionSettingsFileStream << CONFIG_KEY_OTA_FTP_REMOTE_VERSION_FILE_PATH << "=" << otaFTPRemoteVersionFilePath << "\n";

    companionSettingsFileStream.close();
  }
}

std::string WisCommon::replace_all(std::string str,std::string old_value,std::string new_value){
    while(true){
        std::string::size_type   pos(0);
        if((pos=str.find(old_value))!=std::string::npos)
            str.replace(pos,old_value.length(),new_value);
        else
            break;
    }
    return str;
}

std::string WisCommon::getCurrentImageBuildVersion(){
  std::string currentBuildVersion;

  printf("OTA check current build version command is: %s\n", IMAGE_BUILD_VERSION_FILE_PATH.c_str());
  std::ifstream fin(IMAGE_BUILD_VERSION_FILE_PATH, std::ios::in);
  if (fin.is_open()) {
    char line[1024]={0};

    if(fin.getline(line, sizeof(line))) {
      currentBuildVersion = line;
    }
    fin.clear();
    fin.close();
  }
  printf("OTA check current build version is: %s\n", currentBuildVersion.c_str());
  return currentBuildVersion;
}

std::string WisCommon::getCurrentImageBuildDate(){
  std::string currentBuildDate;

  printf("OTA check current build date command is: %s\n", IMAGE_BUILD_DATE_FILE_PATH.c_str());
  std::ifstream fin(IMAGE_BUILD_DATE_FILE_PATH, std::ios::in);
  if (fin.is_open()) {
    char line[1024]={0};

    if(fin.getline(line, sizeof(line))) {
      currentBuildDate = line;
    }
    fin.clear();
    fin.close();
    if (!currentBuildDate.empty()) {  // 20171016060104
      currentBuildDate = currentBuildDate.substr(0,12);
    }
  }
  printf("OTA check current build date is: %s\n", currentBuildDate.c_str());
  return currentBuildDate;
}

std::string WisCommon::getBatteryCapacity(){
  std::string currentBatteryCapacity;

  printf("Check battery capacity command is: %s\n", BATTERY_CAPACITY_FILE_PATH.c_str());
  std::ifstream fin(BATTERY_CAPACITY_FILE_PATH, std::ios::in);
  if (fin.is_open()) {
    char line[1024]={0};

    if(fin.getline(line, sizeof(line))) {
      currentBatteryCapacity = line;
    }
    fin.clear();
    fin.close();
  }
  printf("Check battery capacity is: %s\n", currentBatteryCapacity.c_str());
  return currentBatteryCapacity;
}

std::string WisCommon::getBatteryStatus(){
  std::string currentBatteryStatus;

  printf("Check battery status command is: %s\n", BATTERY_STATUS_FILE_PATH.c_str());
  std::ifstream fin(BATTERY_STATUS_FILE_PATH, std::ios::in);
  if (fin.is_open()) {
    char line[1024]={0};

    if(fin.getline(line, sizeof(line))) {
      currentBatteryStatus = line;
    }
    fin.clear();
    fin.close();
  }
  printf("Check battery status is: %s\n", currentBatteryStatus.c_str());
  return currentBatteryStatus;
}

void WisCommon::sendUDPDataToSender(std::string senderIPAddress, std::string sendContent){
  sendContent += "\n";

  int socketfd;
  socklen_t addr_len;
  struct sockaddr_in server_addr;
  if((socketfd = socket(PF_INET,SOCK_DGRAM,0)) < 0) {
    perror("socket");
    exit(-1);
  }
  printf("socketfd = %d\n",socketfd);

  int i=1;
  socklen_t len = sizeof(i);
  setsockopt(socketfd,SOL_SOCKET,SO_BROADCAST,&i,len);

  memset(&server_addr,0,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(senderIPAddress.c_str());
  server_addr.sin_port = htons(UDP_SEND_IP_PORT);
  addr_len=sizeof(server_addr);
  printf("send content is: %s\n", sendContent.c_str());
  int ret=sendto(socketfd, sendContent.c_str(), sendContent.length(), 0, (struct sockaddr*)&server_addr, addr_len);
  if(ret<0){
      printf("\t\t\t\tsend avs data error....\n");
  }else{
      printf("\t\t\t\tsend avs data ok \n");
  }
  close(socketfd);
}

void WisCommon::killProcess(std::string processName){
  /*std::string execCmd = "ps -ef |grep " + processName + " |grep -v grep | awk \'{print $1}\'";
  printf("--> execCmd: %s\n",execCmd.c_str());
  std::string processNumber;
  char buff[200];
  FILE *fp = popen(execCmd.c_str(), "r");
  if(fp != NULL){
    if(fgets(buff, sizeof(buff), fp)){
      processNumber = buff;
    }
  }
  pclose(fp);
  printf("processNumber = %s\n",processNumber.c_str());
  if (processNumber.length() > 0) {
    system(("kill -9 "+processNumber).c_str());
    printf("killed the process: %s\n", processName.c_str());
  }*/
  //system(("pkill -9 "+processName).c_str());
}

void WisCommon::lightLED(int ledType){
  while (isLedLighting) {
    printf("lightLED:: Wait the previous operation done!\n");
    usleep(50*1000);
  }
  if (lastLedType == ledType) {
    sysLocalTime("< LED status is same as last status, so discard this request! >");
    return;
  }

  printf("start:: turn on LED\n");
  isLedLighting = true;
  switch (ledType) {
    case LED_TYPE_LISTENING:
      sysLocalTime("Start to open < LISTENING > LED");
      system("nled_control 1");
      break;
    case LED_TYPE_MICRO_OFF:
      sysLocalTime("Start to open < MICRO_OFF > LED");
      system("nled_control 2");
      break;
    case LED_TYPE_NOTIFICATION_ARRIVES:
      sysLocalTime("Start to open < NOTIFICATION_ARRIVES > LED");
      system("nled_control 3");
      break;
    case LED_TYPE_NOTIFICATION_QUEUED:
      sysLocalTime("Start to open < NOTIFICATION_QUEUED > LED");
      system("nled_control 4");
      break;
    case LED_TYPE_SPEAKING:
      sysLocalTime("Start to open < SPEAKING > LED");
      system("nled_control 5");
      break;
    case LED_TYPE_SYSTEM_ERROR:
      sysLocalTime("Start to open < SYSTEM_ERROR > LED");
      system("nled_control 6");
      break;
    case LED_TYPE_THINKING:
      sysLocalTime("Start to open < THINKING > LED");
      system("nled_control 7");
      break;
    case LED_TYPE_DEFAULT:
      sysLocalTime("Start to open < DEFAULT > LED");
      system("nled_control 8");
      break;
    case LED_TYPE_HOTSPOT_MODE:
      sysLocalTime("Start to open < HOTSPOT_MODE > LED");
      system("nled_control 9");
      break;
    case LED_TYPE_LOW_BATTERY:
      sysLocalTime("Start to open < LOW_BATTERY > LED");
      system("nled_control 10");
      break;
    default:
      break;
  }
  lastLedType = ledType;
  isLedLighting = false;
  printf("end:: turn on LED\n");
}

void WisCommon::turnOffSoftAPMode(){
  system("iwpriv wlan0 exitAP");
  system("pkill -9 hostapd");
}
