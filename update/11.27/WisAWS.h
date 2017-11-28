

#include <curl/curl.h>
#include <rapidjson/document.h>
#include <aws/transfer/TransferManager.h>
#include <iostream>
#include <string.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include "WisDefineResources.h"
using namespace Aws::Transfer;
using namespace std;
using namespace rapidjson;
enum Check_Result_Code{
  NEED_UPDATE=0,
  NO_NEED_UPDATE,
  CHECK_FAILED
};

enum Download_Result_Code{
  DOWNLOAD_SUCCESS=0,
  DOWNLOAD_FAIL,
  DOWNLOAD_CANCEL
};


class CheckContent{
public:
  string deviceId ="";
  string messageId ="";
  string oldImageVersion ="";
  string newImageVersion ="";
  bool checkResult = false;
  string bucket = "";
  string imageFile = "";
  string errorMsg="";
  string timeStamp="";

  void reset(){
    deviceId ="";
    messageId ="";
    oldImageVersion ="";
    newImageVersion ="";
    checkResult = false;
    bucket = "";
    imageFile = "";
    errorMsg="";
    timeStamp="";

  }
  string toJson(){
    string jsonContent = "{\"DeviceId\":\""+
                    deviceId+"\",\"MessageId\":\""+
                    messageId+"\",\"OldImageVersion\":\""+
                    oldImageVersion+"\",\"NewImageVersion\":null,\"CheckResult\":\"false\",\"S3Bucket\":null,\"ImageFile\":null,\"ErrorMsg\":null,\"TimeStamp\":\""+timeStamp+"\"}";
    return jsonContent;
  }


  bool setjson(char *json){
    std::cout << "start setjson" << '\n';
    std::cout << "receiveContent:" <<json<< '\n';
    Document doc;
    if(doc.Parse(json).HasParseError()){
      //sendErrorLog.
      std::cout << "parse json failed" << '\n';
      return false;
    }
    if(doc["DeviceId"].IsString()){
      deviceId = doc["DeviceId"].GetString();
      std::cout << "DeviceId:" <<deviceId<< '\n';
    }

    if(doc["MessageId"].IsString()){
        messageId = doc["MessageId"].GetString();
        std::cout << "MessageId:" <<messageId<< '\n';
    }
    if(doc["OldImageVersion"].GetString()){
        oldImageVersion = doc["OldImageVersion"].GetString();
        std::cout << "OldImageVersion:" <<oldImageVersion<< '\n';
    }
    if(doc["NewImageVersion"].IsString()){
        newImageVersion = doc["NewImageVersion"].GetString();
        std::cout << "NewImageVersion:" <<newImageVersion<< '\n';
    }
    if(doc["CheckResult"].IsBool()){
        checkResult = doc["CheckResult"].GetBool();
        std::cout << "CheckResult:" <<checkResult<< '\n';
    }
    if(doc["S3Bucket"].GetString()){
        bucket = doc["S3Bucket"].GetString();
        std::cout << "S3Bucket:" <<bucket<< '\n';
    }
    if(doc["ImageFile"].IsString()){
        imageFile = doc["ImageFile"].GetString();
        std::cout << "ImageFile:" <<imageFile<< '\n';
    }
    if(doc["ErrorMsg"].IsString()){
        errorMsg=doc["ErrorMsg"].GetString();
        std::cout << "ErrorMsg:" <<errorMsg<< '\n';
    }
    if(doc["TimeStamp"].IsString()){
        timeStamp= doc["TimeStamp"].GetString();
        std::cout << "TimeStamp:" <<timeStamp<< '\n';
    }

    return true;
  }
};

class LogContent{
public:
  string deviceId ="";
  string messageId ="";
  string logType = "OTAResult";
  int logLevel = 0; // info - 0 ,warning - 1,error - 2
  string oldImageVersion ="";
  string newImageVersion ="";
  string logMessage = "";
  string timeStamp="";

  string toJson(){
      StringBuffer s;
	    Writer<StringBuffer> writer(s);
	    writer.StartObject();
	    writer.Key("DeviceId");
	    writer.String(deviceId);
	    writer.Key("MessageId");
	    writer.String(messageId);
	    writer.Key("LogType");
	    writer.String("OTAResult");
	    writer.Key("LogLevel");
	    writer.Int(logLevel);
	    writer.Key("OldImageVersion");
	    writer.String(oldImageVersion);
	    writer.Key("NewImageVersion");
	    writer.String(newImageVersion);
	    writer.Key("LogMessage");
	    writer.String(logMessage);
	    writer.Key("TimeStamp");
	    writer.String(timeStamp);
	    writer.EndObject();
      return s.GetString();
  }
};


class WisAWS{
public:
  static const int LOG_INFO = 0;
  static const int LOG_WARNING = 1;
  static const int LOG_ERROR = 2;
  WisAWS();
  Check_Result_Code sendCheckOTAUpgrade(string oldBuildVersion,string deviceId);
  Download_Result_Code downloadImage(string fileLocation,DownloadProgressCallback progressCallBack);
  string getNewVersion();
  string getNewBuildDate();
  void sendLog(int logLevel);
  void cancelDownload();
  bool isDownloading();
  bool isDownloadSuccess();
  void setIsDownloading(bool isDownloading);
  void addLogMessage(string logMessage);
private:

  string AWS_TAG = "damon_aws";
  CheckContent sendCheckContent;
  CheckContent receiveCheckContent;
  std::shared_ptr<Aws::Transfer::TransferHandle> mRequestPtr;
  string mLogMessage = "---";
  string mMessageId;
  string mNewBuildDate;
  string mNewVersion;
  string mSecretKeyId ;//= "AKIAJHKJ55GEJOTGCUYQ";
  string mSecretKey ;//= "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";
  string mDeviceId;
  string mOldVersion;
  bool mIsDownloading = false;
  bool mIsDownloadSuccess = false;
  string getDeviceId();
  string getS3Bucket();
  string getImageFile();
  string getMessageId();
  string getTimeStamp();
  string getOldVersion();
  void refreshMessageId();
  void getSecretKey();
};
