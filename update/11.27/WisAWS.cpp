#include <iostream>
#include <aws/s3/S3Client.h>
#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/model/Object.h>
#include <aws/core/utils/ratelimiter/DefaultRateLimiter.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/transfer/TransferManager.h>
#include <fstream>
#include "WisAWS.h"
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <string>
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::S3;
using namespace Aws::S3::Model;
using namespace Aws::Transfer;
using namespace Aws::Http;
using namespace Aws::Utils;
using namespace std;

WisAWS::WisAWS(){

  getSecretKey();
}
void WisAWS::getSecretKey(){
  ifstream accountFile(awsAccountPath,std::ios::in);
  if(!accountFile.is_open())
    std::cout << "open secretkey file fail!!!!!" << '\n';
  else{
    std::cout << "open secretkey file success!!!!!" << '\n';
    string s;
    while(getline(accountFile,s)){

      s.erase(0,s.find_first_not_of(" "));
      s.erase(s.find_last_not_of(" ") + 1);
      if(s[0]=='#'||s.size()<=1)
        continue;
      if(s.find("aws_access_key_id")<s.size()){
        mSecretKeyId = s.substr(s.find("=")+1);
      }else if(s.find("aws_secret_access_key")<s.size()){
        mSecretKey = s.substr(s.find("=")+1);
      }

    }
    accountFile.clear();
    accountFile.close();
  }

}


void WisAWS::sendLog(int logLevel){
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  LogContent logContent;
  logContent.deviceId = getDeviceId();
  logContent.messageId = getMessageId();
  logContent.logLevel = logLevel;
  logContent.oldImageVersion = getOldVersion();
  logContent.newImageVersion = getNewVersion();
  logContent.logMessage = mLogMessage;
  logContent.timeStamp = getTimeStamp();
  string logJson = logContent.toJson();
  std::cout << "send Log----" <<logJson<< '\n';
  if(curl){
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers,"Content-Type: application/json");
    curl_easy_setopt(curl,CURLOPT_POST,1l);
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,logJson.c_str());
    curl_easy_setopt(curl,CURLOPT_URL,LOG_SERVICE_URL.c_str());
    res = curl_easy_perform(curl);
    if(res!=0){
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
    }
  }
}

/*
  send check version request
*/
Check_Result_Code WisAWS::sendCheckOTAUpgrade(string oldBuildVersion,string deviceId){
  if(isDownloading()) return NEED_UPDATE;
  refreshMessageId();
  receiveCheckContent.reset();
  mLogMessage = "---";
  mNewVersion = oldBuildVersion;
  mOldVersion = oldBuildVersion;
  mDeviceId = "NT"+deviceId;
  CURL *curl;
  CURLcode res;
  FILE *fp;
  if ((fp = fopen("/data/nut2_avs/temp_json.txt", "w")) == NULL){  // 返回结果用文件存储
    addLogMessage("open local file /data/nut2_avs/temp_json.txt failed");
    sendLog(WisAWS::LOG_ERROR);
    return CHECK_FAILED;
  }
  curl = curl_easy_init();
  sendCheckContent.deviceId = "NT"+deviceId;
  sendCheckContent.messageId = getMessageId();
  sendCheckContent.timeStamp = getTimeStamp();
  sendCheckContent.oldImageVersion = oldBuildVersion;
  string jsonContent = sendCheckContent.toJson();
  std::cout << "sendCheckContent:" <<jsonContent<< '\n';
  if(curl){
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers,"Content-Type: application/json");
    curl_easy_setopt(curl,CURLOPT_POST,1l);
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,jsonContent.c_str());
    curl_easy_setopt(curl,CURLOPT_URL,CHECK_SERVICE_URL.c_str());
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
    res = curl_easy_perform(curl);
    if(res!=0){
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
    }
    fclose(fp);
    if ((fp = fopen("/data/nut2_avs/temp_json.txt", "r")) == NULL){  // 返回结果用文件存储
      addLogMessage("open local file /data/nut2_avs/temp_json.txt failed");
      sendLog(WisAWS::LOG_ERROR);
      return CHECK_FAILED;
    }
    char buf[1024];
    fgets(buf,1024,fp);
    fclose(fp);
    if(!receiveCheckContent.setjson(buf)){
      addLogMessage("Receive the check result but Parse the json error!");
      sendLog(WisAWS::LOG_ERROR);
      return CHECK_FAILED;
    }
    mNewVersion = receiveCheckContent.newImageVersion;
    if(receiveCheckContent.checkResult) {
      addLogMessage("Check image version successfully and image needs to update!");
      return NEED_UPDATE;
    }
    else {
      addLogMessage("Check image version successfully and image is the latest version");
      sendLog(WisAWS::LOG_INFO);
      return NO_NEED_UPDATE;
    }
  }
  std::cout << "error : can not init curl " << '\n';
  return CHECK_FAILED;
}
string WisAWS::getDeviceId(){

  return mDeviceId;
}
string WisAWS::getS3Bucket(){
  return receiveCheckContent.bucket;
}

string WisAWS::getImageFile(){
  return receiveCheckContent.imageFile;
}

void WisAWS::addLogMessage(string logMessage){
  mLogMessage += logMessage+"---";
}

bool WisAWS::isDownloading(){
  return mIsDownloading;
}

bool WisAWS::isDownloadSuccess(){
  return mIsDownloading;
}

void WisAWS::setIsDownloading(bool isDownloading){
  mIsDownloading = isDownloading;
}

string WisAWS::getOldVersion(){
  return mOldVersion;
}

void WisAWS::refreshMessageId(){
  int i;
  int random;
  char uuid[38] ;
  int index=0;
  srand(time(0));
  for(i=0;i<16;i++)
  {
       random = rand()%128;
      sprintf(uuid+index,"%02X",random);
       index+=2;
        if(i==3||i==5||i==7||i==9){
        *(uuid+index) ='-';
        index++;

        }
  }
  for(i = 0;i<38;i++){
     if(uuid[i]<='Z'&&uuid[i]>='A')
       uuid[i] +=32;
  }
  string messageId = uuid;
  cout<<messageId<<endl;
	mMessageId = messageId;
}

string WisAWS::getMessageId(){
  return mMessageId;
}

string WisAWS::getTimeStamp(){
    time_t t = time(0);
    char tmp[64];
    strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %z",localtime(&t) );
    string timeStamp = tmp;
    return timeStamp;
}
string WisAWS::getNewVersion(){
  return mNewVersion;
}
string WisAWS::getNewBuildDate(){
  string newVersion = getNewVersion();
  mNewBuildDate = newVersion.substr(5,8);
  return mNewBuildDate;
}

void WisAWS::cancelDownload(){
  mRequestPtr->Cancel();
  mRequestPtr->UpdateStatus(Aws::Transfer::TransferStatus::CANCELED);
  mRequestPtr->finishDownload();
}

Download_Result_Code WisAWS::downloadImage(string fileLocation,DownloadProgressCallback progressCallBack){
  string bucket = getS3Bucket();
  string imageFile = getImageFile();
  std::cout << "bucket   =" <<bucket<< '\n';
  std::cout << "imageFile" <<imageFile<< '\n';
  addLogMessage("bucket = "+bucket);
  addLogMessage("imageFile = "+imageFile);
  //string bucket = "ivansource1";
  //string imageFile = "version.txt";
  if(bucket == "" || imageFile ==""){
    std::cout << "downloadImage failed : could not get the bucket or imageFile" << '\n';
    addLogMessage("Bucket or imageFile is null");
    sendLog(LOG_ERROR);
    return DOWNLOAD_FAIL;
  }

  Aws::SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
  Aws::InitAPI(options);
  std::shared_ptr<Aws::Utils::RateLimits::RateLimiterInterface> Limiter;
  Limiter = Aws::MakeShared<Aws::Utils::RateLimits::DefaultRateLimiter<>>(AWS_TAG.c_str(), 50000000);
  Aws::Client::ClientConfiguration config;
  config.region = Aws::Region::US_WEST_2;
  config.scheme = Aws::Http::Scheme::HTTP;
  config.readRateLimiter = Limiter;
  config.writeRateLimiter = Limiter;
  config.connectTimeoutMs = 30000;
  config.requestTimeoutMs = 600000;
  config.verifySSL = false;

  Aws::Auth::AWSCredentials cred(mSecretKeyId.c_str(), mSecretKey.c_str());
  auto client = Aws::MakeShared<Aws::S3::S3Client>(AWS_TAG.c_str(),cred, config, AWSAuthV4Signer::PayloadSigningPolicy::Never , true);
  std::shared_ptr<Aws::Utils::Threading::Executor> m_executor;
  m_executor = Aws::MakeShared<Aws::Utils::Threading::PooledThreadExecutor>(AWS_TAG.c_str(), 4);
  TransferManagerConfiguration transferConfig(m_executor.get());
  transferConfig.s3Client = client;

  transferConfig.downloadProgressCallback = progressCallBack;

  auto transferManager=TransferManager::Create(transferConfig);
  Aws::Transfer::CreateDownloadStreamCallback downloadStreamCallback= [&fileLocation](){
  Aws::FStream *stream = Aws::New<Aws::FStream>("s3file", fileLocation.c_str(), std::ios_base::out);
  stream->rdbuf()->pubsetbuf(NULL, 0);
  return stream; };
  DownloadConfiguration downloadConfig;
  mIsDownloading = true;
  addLogMessage("start to download image !");
  std::cout << "start to download" << '\n';
  mRequestPtr = transferManager->DownloadFile(bucket.c_str(), imageFile.c_str(),downloadStreamCallback,downloadConfig,fileLocation.c_str());
  mRequestPtr->WaitUntilFinished();
  std::cout << "download finished!" << '\n';
  	// Check status
  Download_Result_Code downloadResult = DOWNLOAD_FAIL;
  if ( mRequestPtr->GetStatus() == Aws::Transfer::TransferStatus::COMPLETED ) {
  	 if ( mRequestPtr->GetBytesTotalSize() == mRequestPtr->GetBytesTransferred() ) {
   	      std::cout << "<AWS DOWNLOAD> Get FW success!" << std::endl;
          mIsDownloadSuccess = true;
          addLogMessage("Download successfully!");
   	      downloadResult = DOWNLOAD_SUCCESS;
   	 }else {

          addLogMessage("<AWS DOWNLOAD> Get FW failed - Bytes downloaded did not equal requested number of bytes: "
          +(mRequestPtr->GetBytesTotalSize())
          +(mRequestPtr->GetBytesTransferred()));
          sendLog(LOG_ERROR);
   	      std::cout << "<AWS DOWNLOAD> Get FW failed - Bytes downloaded did not equal requested number of bytes: " << mRequestPtr->GetBytesTotalSize() << mRequestPtr->GetBytesTransferred() << std::endl;
  	  }
  }else if(mRequestPtr->GetStatus() == Aws::Transfer::TransferStatus::CANCELED){
    downloadResult = DOWNLOAD_CANCEL;
    mIsDownloading = false;
    addLogMessage("The user chooses to cancel the download. Download failed");
    sendLog(LOG_INFO);
    std::cout << "The user chooses to cancel the download. Download failed" << '\n';
  }else {
      std::cout << "<AWS DOWNLOAD> Get FW failed - download was never completed even after retries" << std::endl;
      addLogMessage("update failed! Download was not completed.");
      sendLog(WisAWS::LOG_ERROR);

  }
  Aws::ShutdownAPI(options);
  mIsDownloading = false;
  return downloadResult;
}
