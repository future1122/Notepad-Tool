/*
 * AlexaAudioTest.cpp
 *
 * Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#include <gtest/gtest.h>
#include <aws/s3/S3Client.h>
#include <string>
#include <future>
#include <fstream>
#include <chrono>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <iostream>

#include <algorithm>
#include <condition_variable>
#include <memory>
#include <thread>

#include <gmock/gmock.h>

// King add: Main app
#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>  // File exist
#include <fcntl.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/time.h>
#include <ifaddrs.h>
#include <signal.h>  // Detect Wifi connection

// Multi cast
#include <netdb.h>

//for gstreamer
#include <gst/gst.h>
#include <glib.h>

#include <AVSCommon/Utils/Logger/Logger.h>
#include <AVSCommon/Utils/Memory/Memory.h>

#include "ACL/Transport/HTTP2MessageRouter.h"
#include "ACL/Transport/HTTPContentFetcherFactory.h"
#include <ACL/Transport/PostConnectObject.h>
#include "ADSL/DirectiveSequencer.h"
#include "ADSL/MessageInterpreter.h"
#include "AFML/FocusManager.h"
#include "AuthDelegate/AuthDelegate.h"
#include "AVSCommon/AVS/Attachment/AttachmentManager.h"
#include "AVSCommon/AVS/Attachment/InProcessAttachmentReader.h"
#include "AVSCommon/AVS/Attachment/InProcessAttachmentWriter.h"
#include "AVSCommon/AVS/BlockingPolicy.h"
#include "AVSCommon/Utils/JSON/JSONUtils.h"
#include "AVSCommon/SDKInterfaces/DirectiveHandlerInterface.h"
#include "AVSCommon/SDKInterfaces/DirectiveHandlerResultInterface.h"
#include "AVSCommon/AVS/Initialization/AlexaClientSDKInit.h"
#include "AVSCommon/AVS/ExceptionEncounteredSender.h"
#include "CertifiedSender/CertifiedSender.h"
#include "CertifiedSender/SQLiteMessageStorage.h"
#include "ContextManager/ContextManager.h"
#include "Integration/AuthObserver.h"
#include "Integration/ClientMessageHandler.h"
#include "Integration/ConnectionStatusObserver.h"
#include "Integration/ObservableMessageRequest.h"
#include "Integration/TestMessageSender.h"
#include "Integration/TestSpeechSynthesizerObserver.h"
#include "AVSCommon/Utils/Logger/LogEntry.h"
#include "SpeechSynthesizer/SpeechSynthesizer.h"
#include "Integration/TestDirectiveHandler.h"
#include "Integration/TestExceptionEncounteredSender.h"
#include "System/UserInactivityMonitor.h"

// Bob start
#include "ACL/AVSConnectionManager.h"
#include "AFML/Channel.h"
#include "AIP/AudioInputProcessor.h"
#include "AIP/AudioProvider.h"
#include "AIP/Initiator.h"
#include "AVSCommon/AVS/MessageRequest.h"
#include "AVSCommon/SDKInterfaces/ExceptionEncounteredSenderInterface.h"
#include "AVSCommon/SDKInterfaces/ChannelObserverInterface.h"
#include "AVSCommon/SDKInterfaces/ContextManagerInterface.h"
#include "AVSCommon/SDKInterfaces/KeyWordObserverInterface.h"
#include "Integration/AipStateObserver.h"
#include "AudioPlayer/AudioPlayer.h"
#include "PlaybackController/PlaybackController.h"

// If the tests are created with both Kittai and Sensory, Kittai is chosen.
#ifdef KWD_KITTAI
#include "KittAi/KittAiKeyWordDetector.h"
#elif KWD_SENSORY
#include "Sensory/SensoryKeywordDetector.h"
#endif
// Bob end

//Bob add 20170731
#include "Alerts/AlertsCapabilityAgent.h"
#include "Alerts/AlertObserverInterface.h"
#include "Alerts/Storage/SQLiteAlertStorage.h"
#include "Integration/TestAlertObserver.h"
//#include "System/StateSynchronizer.h"
#include "AVSCommon/Utils/UUIDGeneration/UUIDGeneration.h"
//Bob end 20170731

//Bob add for settings 20171019
#include <sstream>
#include <iterator>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include "AVSCommon/SDKInterfaces/GlobalSettingsObserverInterface.h"
#include "AVSCommon/SDKInterfaces/SingleSettingObserverInterface.h"
#include "Settings/Settings.h"
#include "Settings/SettingsUpdatedEventSender.h"
#include "Settings/SettingsStorageInterface.h"
#include "Settings/SQLiteSettingStorage.h"
//Bob end 20171019

// King add
#include "wistron/LocalMediaPlayer.h"
#include "wistron/WisSVAHeader.h"
#include "WisHalRec.h"
#include "wistron/WisAVS.h"
#include "wistron/WisVolumeControl.h"
#include "wistron/WisDefineResources.h"
#include "wistron/MockContextRequester.h"
#include "wistron/MockStateProvider.h"
#include "wistron/ftplib.h"
#include "wistron/WisFM.h"
//damon add
#include "wistron/WisAWS.h"

#include "wistron/RingBuffer.h"
#include "wistron/RingBufferConsumer.h"
#include "wistron/RingBufferProducer.h"
//#ifndef WISTRON_WISDEFINERESOURCES_H
//#define WISTRON_WISDEFINERESOURCES_H

// King add start
#define WAKE_WORD_DETECTOR_SVA            0
#define WAKE_WORD_DETECTOR_SENSORY        1
int wakeWordDetector = WAKE_WORD_DETECTOR_SVA;

#define REC_SOURCE_SVA_LAB_DATA      0
#define REC_SOURCE_HAL_REC_TEST      1
int recordingSource = REC_SOURCE_HAL_REC_TEST;

int enablePreRecSoundPrompt = 0;
int enablePostRecSoundPrompt = 0;
int enableRecLEDPrompt = 1;
int svaKeywordConfidenceLevel = 60;
int svaNumberOfKeywords = 3;
int svaNumberOfUsersPerKeyword = 0;
int recordingTimeout = 6;
int waitDirectiveTimeout = 1500;
int waitForResponseTimeout = 7;
int enableSoundWhenReceivedPhoneCmd = 1;
std::string syncNTPServer = "ptbtime1.ptb.de";
std::string pingAddress = "www.bing.com";

int totalAVSRequestNum = 0;
int failAVSRequestNum = 0;
int downloadProgress = -1;

bool isAVSReady = false;
bool isSVAReady = false;
bool exitSva = false;
bool exitAVS = false;
bool isAVSFinish = true;
bool isSVAFinish = false;
bool isSVALABRecording = false;
bool isAVSPlayingResponse = true;
bool isPlayEnabled = true;
bool isRecordingFinish = false;
bool isNtpSynced = false;
//damon add
bool isScanningWifiFinish = false;

std::string m_LocalMusicPath = APP_ROOT_FOLDER + "test_music.mp3";

// UDP communication
std::string mSenderIPAddress;

// Global status definition
static bool isLaunchAVSKeyPressed = false; //Bob add for TAP key press 20170907
bool isSoftAPModeOn = false;
std::string wifiInfoData;
bool isAudioPlayerPlaying = false;
bool isNotificationOngoing = false;
bool isExpectDirectStatus = false;
// King add end

// status
bool isWifiConnected = false;
bool isTTSPlaying = false;

// King add start
WisCommon mWisCommon;
WisAVS mWisAvs;
WisVolumeControl mWisVolumeControl;
WisFM mWisFM;
WisAWS mWisAws;
//std::shared_ptr<MockStateProvider> m_speechSynthesizerStateProvider;
//std::shared_ptr<MockStateProvider> m_audioPlayerStateProvider;
//std::shared_ptr<MockStateProvider> m_alertsStateProvider;
//std::shared_ptr<MockContextRequester> m_contextRequester;

// MediaPlayer object
LocalMediaPlayer m_localMediaPlayer;  // To play local music
std::shared_ptr<MediaPlayer> m_speakMediaPlayer;  // To play TTS
std::shared_ptr<MediaPlayer> m_rendererMediaPlayer;  // To play Alert or Timer
std::shared_ptr<MediaPlayer> m_contentMediaPlayer;  // To play AudioPlayer

// King add
const std::string APP_TAG = "Nut2AVS";
std::mutex m_wisMutex;
std::condition_variable m_wisStateTrigger;

void stopMediaPlayer(){
  printf("stopMediaPlayer 11111\n");
  if (m_localMediaPlayer.m_mediaPlayer != nullptr /*&& m_localMediaPlayer.m_mediaPlayer->getOffset() != std::chrono::milliseconds::zero()*/) {
    printf("stopLocalMediaPlayer(Local) 22222\n");
    m_localMediaPlayer.m_mediaPlayer->stop();
  }
  if (m_speakMediaPlayer != nullptr /*&& m_speakMediaPlayer->getOffset() != std::chrono::milliseconds::zero()*/) {
    printf("stopSpeakMediaPlayer(TTS) 33333\n");
    m_speakMediaPlayer->stop();
  }
  // King test
  if (m_rendererMediaPlayer != nullptr) {
    printf("stopRendererMediaPlayer(Alert)  44444\n");
    m_rendererMediaPlayer->stop();
  }
  printf("stopMediaPlayer 55555\n");
}

// FM
vector<int> fm_searchStations;
void main_ps_name_callback(int ps_name_len, char *ps_name);
void closeFMDuringTest(){
  printf("%s : isFMOpened = %d\n", "openFMDuringTest", mWisFM.isFMOpened);
  if (mWisFM.isFMOpened) {
    mWisFM.mLastFMOpened=true;
    mWisFM.disable();
  }
}
void openFMDuringTest(){
  printf("%s : isFMOpened = %d , mLastFMOpened = %d\n", "openFMDuringTest",mWisFM.isFMOpened, mWisFM.mLastFMOpened);
  if (!mWisFM.isFMOpened && mWisFM.mLastFMOpened) {
    mWisFM.enable(main_ps_name_callback);

    printf("%s\n", "FM set frequency");
    mWisFM.setFrequency(mWisFM.mLastFMFrequency);

    int current_volume = mWisFM.getVolume();
    std::string content;
    content = "fm:volume="+std::to_string(current_volume);
    mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
  }
}

// Volume
int getCurrentMediaVolume(){
  pid_t status;
  status = system("volume_control get");
  int current_volume = WEXITSTATUS(status);
  mWisCommon.sysLocalTime(("current mediaPlayer voluem value: " + std::to_string(current_volume)).c_str());
  return current_volume;
}
void sendCurrentMediaVolume(){
  int current_volume = getCurrentMediaVolume();
  std::string content;
  content = "play_music:volume="+std::to_string(current_volume);
  mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
}

void restoreDefaultLED(){
  if (isNotificationOngoing) {
    mWisCommon.lightLED(LED_TYPE_NOTIFICATION_QUEUED);
  } else if (isTTSPlaying) {
    mWisCommon.lightLED(LED_TYPE_SPEAKING);
  } else {
    mWisCommon.lightLED(LED_TYPE_DEFAULT);
  }
}

bool isStopCaptureWaitNextDirectiveFlag = false;
bool isWaitPlayingEventFlag = false;
void *checkAvsResponseTimeout(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("checkAvsResponseTimeout thread: detachCode = %d\n", detachCode);

  std::unique_lock<std::mutex> lock(m_wisMutex);
  std::chrono::milliseconds WAIT_TIMEOUT(waitDirectiveTimeout);
  if (!m_wisStateTrigger.wait_for(lock, WAIT_TIMEOUT, []() { return !isStopCaptureWaitNextDirectiveFlag;})) {
      mWisCommon.sysLocalTime("checkAvsResponseTimeout: have not a valid directive, so restore the default light------");
      restoreDefaultLED();
  }
  lock.unlock();

  pthread_exit(0);
  return 0;
}

void *checkPlayingEventTimeout(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("checkPlayingEventTimeout thread: detachCode = %d\n", detachCode);

  std::unique_lock<std::mutex> lock(m_wisMutex);
  std::chrono::seconds WAIT_TIMEOUT(waitForResponseTimeout);
  if (!m_wisStateTrigger.wait_for(lock, WAIT_TIMEOUT, []() { return !isWaitPlayingEventFlag;})) {
      mWisCommon.sysLocalTime("checkPlayingEventTimeout: have not a valid playing event, so restore the default light------");
      restoreDefaultLED();
  }
  lock.unlock();

  pthread_exit(0);
  return 0;
}
// King add end

// King debug
const int InitiatorType_TAP           = 0;
const int InitiatorType_WAKEWORD      = 1;
int recognizeInitiatorType = InitiatorType_WAKEWORD;
const int WCD_PLAY_SOUND_CUE_TYPE_MEDIA_PLAYER  = 0;
const int WCD_PLAY_SOUND_CUE_TYPE_TINY_PLAY     = 1;
int wcdPlaySoundCueType = WCD_PLAY_SOUND_CUE_TYPE_TINY_PLAY;
bool isForVickLogEnable = true;  // King test

#ifdef GSTREAMER_MEDIA_PLAYER
#include "MediaPlayer/MediaPlayer.h"
#else
#include "Integration/TestMediaPlayer.h"
#endif

namespace alexaClientSDK {
namespace integration {
namespace test {

using namespace acl;
using namespace adsl;
using namespace authDelegate;
using namespace avsCommon;
using namespace avsCommon::avs;
using namespace avsCommon::avs::attachment;
using namespace avsCommon::sdkInterfaces;
using namespace avsCommon::avs::initialization;
using namespace avsCommon::utils::mediaPlayer;
using namespace avsCommon::utils::memory;  // Bob add
using namespace ::testing;  // Bob add
using namespace certifiedSender;
using namespace contextManager;
using namespace capabilityAgents::aip;  // Bob add
using namespace capabilityAgents::system;
using namespace capabilityAgents::speechSynthesizer;
using namespace sdkInterfaces;
using namespace avsCommon::utils::sds;
using namespace avsCommon::utils::json;
using namespace afml;
using namespace avsCommon::utils;  // King 201708
//Bob add for settings 20171019
using namespace capabilityAgents::settings;
//Bob end 20171019
//Bob add for PlaybackController 20171020
using namespace capabilityAgents::playbackController;
//Bob end 20171020
/*#ifdef GSTREAMER_MEDIA_PLAYER
using namespace mediaPlayer;
#endif*/

// This is a 16 bit 16 kHz little endian linear PCM audio file of "Joke" to be recognized.
static const std::string RECOGNIZE_JOKE_AUDIO_FILE_NAME = "/recognize_joke_test.wav";
// This is a 16 bit 16 kHz little endian linear PCM audio file of "Wikipedia" to be recognized.
static const std::string RECOGNIZE_WIKI_AUDIO_FILE_NAME = "/recognize_wiki_test.wav";
// This is a 16 bit 16 kHz little endian linear PCM audio file of "Lions" to be recognized.
static const std::string RECOGNIZE_LIONS_AUDIO_FILE_NAME = "/recognize_lions_test.wav";
// This is a 16 bit 16 kHz little endian linear PCM audio file of "Flashbriefing" to be recognized.
static const std::string RECOGNIZE_FLASHBRIEFING_AUDIO_FILE_NAME = "/recognize_flashbriefing_test.wav";
// This is a 16 bit 16 kHz little endian linear PCM audio file of "What's up" to be recognized.
static const std::string RECOGNIZE_WHATS_UP_AUDIO_FILE_NAME = "/recognize_whats_up_test.wav";
// This is a 16 bit 16 kHz little endian linear PCM audio file of "Volume up" to be recognized.
static const std::string RECOGNIZE_VOLUME_UP_AUDIO_FILE_NAME = "/recognize_volume_up_test.wav";
// String to be used as a basic DialogRequestID.
#define FIRST_DIALOG_REQUEST_ID "DialogRequestID123"
// String to be used as a DialogRequestID when the first has already been used.
#define SECOND_DIALOG_REQUEST_ID "DialogRequestID456"
// This string specifies a Recognize event using the CLOSE_TALK profile and uses the first DialogRequestID.
static const std::string CT_FIRST_RECOGNIZE_EVENT_JSON = RECOGNIZE_EVENT_JSON(CLOSE_TALK, FIRST_DIALOG_REQUEST_ID);
// This string specifies a Recognize event using the CLOSE_TALK profile and uses the first DialogRequestID.
static const std::string CT_FIRST_RECOGNIZE_EVENT_JSON_NEAR = RECOGNIZE_EVENT_JSON(NEAR_FIELD, FIRST_DIALOG_REQUEST_ID);
// This string specifies a Recognize event using the CLOSE_TALK profile and uses the second DialogRequestID.
static const std::string CT_SECOND_RECOGNIZE_EVENT_JSON = RECOGNIZE_EVENT_JSON(CLOSE_TALK, SECOND_DIALOG_REQUEST_ID);

// This pair connects a Speak name and SpeechSynthesizer namespace for use in DirectiveHandler registration.
static const NamespaceAndName SPEAK_PAIR = {NAMESPACE_SPEECH_SYNTHESIZER, NAME_SPEAK};
// This pair connects a ExpectSpeech name and SpeechRecognizer namespace for use in DirectiveHandler registration.
static const NamespaceAndName EXPECT_SPEECH_PAIR = {NAMESPACE_SPEECH_RECOGNIZER, NAME_EXPECT_SPEECH};
// This pair connects a SetMute name and Speaker namespace for use in DirectiveHandler registration.
static const NamespaceAndName SET_MUTE_PAIR = {NAMESPACE_SPEAKER, NAME_SET_MUTE};
// This pair connects a SetMute name and Speaker namespace for use in DirectiveHandler registration.
static const NamespaceAndName ADJUST_VOLUME_PAIR = {NAMESPACE_SPEAKER, NAME_ADJUST_VOLUME};
// This pair connects a SetMute name and Speaker namespace for use in DirectiveHandler registration.
static const NamespaceAndName VOLUME_STATE_PAIR = {NAMESPACE_SPEAKER, NAME_VOLUME_STATE};  // Bob add

//King add
/// SpeechSynthesizer namespace and name
static const NamespaceAndName SPEECH_STATE_PAIR(NAMESPACE_SPEECH_SYNTHESIZER, NAME_SPEECH_STATE);
/// AudioPlayer namespace and name
static const NamespaceAndName PLAYBACK_STATE_PAIR(NAMESPACE_AUDIO_PLAYER, NAME_PLAYBACK_STATE);
/// Alerts namespace and name
static const NamespaceAndName ALERTS_STATE_PAIR(NAMESPACE_ALERTS, NAME_ALERTS_STATE);
/// Notifications namespace and name
static const NamespaceAndName INDICATOR_STATE_PAIR(NAMESPACE_NOTIFICATIONS, NAME_INDICATOR_STATE);

/// Sample alerts activity id.
static const std::string ALARM_ACTIVITY_ID = "Alarms";  // Bob add
// This Integer to be used to specify a timeout in seconds for a directive to reach the DirectiveHandler.
static const std::chrono::seconds WAIT_FOR_TIMEOUT_DURATION(15);
// This Integer to be used to specify a timeout in seconds for AuthDelegate to wait for LWA response.
static const std::chrono::seconds SEND_EVENT_TIMEOUT_DURATION(20);
// This Integer to be used to specify a timeout in seconds for a directive to reach the DirectiveHandler.
static const std::chrono::seconds DIRECTIVE_TIMEOUT_DURATION(7);
// This Integer to be used when it is expected the duration will timeout.
static const std::chrono::seconds WANTING_TIMEOUT_DURATION(1);

static const std::chrono::seconds WAIT_FOR_TIMEOUT_DURATION_TEMP(5);   // Bob add

/// JSON key to get the event object of a message.
static const std::string JSON_MESSAGE_EVENT_KEY = "event";
/// JSON key to get the directive object of a message.
static const std::string JSON_MESSAGE_DIRECTIVE_KEY = "directive";
/// JSON key to get the header object of a message.
static const std::string JSON_MESSAGE_HEADER_KEY = "header";
/// JSON key to get the namespace value of a header.
static const std::string JSON_MESSAGE_NAMESPACE_KEY = "namespace";
/// JSON key to get the name value of a header.
static const std::string JSON_MESSAGE_NAME_KEY = "name";
/// JSON key to get the messageId value of a header.
static const std::string JSON_MESSAGE_MESSAGE_ID_KEY = "messageId";
/// JSON key to get the dialogRequestId value of a header.
static const std::string JSON_MESSAGE_DIALOG_REQUEST_ID_KEY = "dialogRequestId";
/// JSON key to get the payload object of a message.
static const std::string JSON_MESSAGE_PAYLOAD_KEY = "payload";

// Bob start
#ifdef KWD_KITTAI
  static const double KITTAI_SENSITIVITY = 0.6;
  /// The name of the resource file required for Kitt.ai.
  static const std::string RESOURCE_FILE = "/KittAiModels/common.res";
  /// The name of the Alexa model file for Kitt.ai.
  static const std::string MODEL_FILE = "/KittAiModels/alexa.umdl";
  /// The keyword associated with alexa.umdl.
  static const std::string MODEL_KEYWORD = "ALEXA";
#elif KWD_SENSORY
  /// The name of the resource file required for Sensory
  //static const std::string RESOURCE_FILE = "/SensoryModels/spot-alexa-rpi-31000.snsr";
  //static const std::string RESOURCE_FILE = "/SensoryModels/spot-hbg-enUS-1.2.2-m.snsr";
  static const std::string RESOURCE_FILE = "/SensoryModels/thfft_alexa_enus_v2_1mb.snsr";
#endif
/// The compatible encoding for Kitt.ai.
static const avsCommon::utils::AudioFormat::Encoding COMPATIBLE_ENCODING =
        avsCommon::utils::AudioFormat::Encoding::LPCM;
/// The compatible endianness for Kitt.ai.
static const avsCommon::utils::AudioFormat::Endianness COMPATIBLE_ENDIANNESS =
        avsCommon::utils::AudioFormat::Endianness::LITTLE;
/// The compatible sample rate for Kitt.ai.
static const unsigned int COMPATIBLE_SAMPLE_RATE = 16000;
/// The compatible bits per sample for Kitt.ai.
static const unsigned int COMPATIBLE_SAMPLE_SIZE_IN_BITS = 16;
/// The compatible number of channels for Kitt.ai
static const unsigned int COMPATIBLE_NUM_CHANNELS = 1;
// Bob end

//Bob add 20170818 for audioplayer
/// The dialog Channel name used in intializing the FocusManager.
static const std::string DIALOG_CHANNEL_NAME = "Dialog";
/// The content Channel name used in intializing the FocusManager.
static const std::string CONTENT_CHANNEL_NAME = "Content";
static const std::string ALERTS_CHANNEL_NAME = "Alerts";

/// An incorrect Channel name that is never initialized as a Channel.
static const std::string TEST_CHANNEL_NAME = "Test";
/// Sample content activity id.
static const std::string TEST_ACTIVITY_ID = "test";
/// The priority of the dialog Channel used in intializing the FocusManager.
static const unsigned int DIALOG_CHANNEL_PRIORITY = 100;
/// The priority of the content Channel used in intializing the FocusManager.
static const unsigned int CONTENT_CHANNEL_PRIORITY = 300;
/// The priority of the content Channel used in intializing the FocusManager.
static const unsigned int TEST_CHANNEL_PRIORITY = 400;
static const unsigned int ALERTS_CHANNEL_PRIORITY=200;

// Sample dialog activity id.
static const std::string DIALOG_ACTIVITY_ID = "Dialog";
// Sample content activity id.
static const std::string CONTENT_ACTIVITY_ID = "Content";
/// Sample alerts activity id.
static const std::string ALERTS_ACTIVITY_ID = "Alerts";
//Bob end 20170818

bool sensoryKeyWordDetected = false;
std::vector<void *> wakeWordCache; //1s
AudioInputStream::Index m_wakewordBeginIndex, m_wakewordEndIndex;

class tapToTalkButton{
    public:
    bool startRecognizing(std::shared_ptr<AudioInputProcessor> aip, std::shared_ptr<AudioProvider> audioProvider) {
        return aip->recognize(*audioProvider, Initiator::TAP).get();
    }
};

#ifdef KWD
class wakeWordTrigger  : public KeyWordObserverInterface{
    public:
        wakeWordTrigger(AudioFormat compatibleAudioFormat, std::shared_ptr<AudioInputProcessor> aip) {
            m_compatibleAudioFormat = compatibleAudioFormat;
            m_aip = aip;
        }
    void onKeyWordDetected(
        std::shared_ptr<AudioInputStream> stream,
        std::string keyword,
        AudioInputStream::Index beginIndex,
        AudioInputStream::Index endIndex) {
            // King 201708
            printf("%s  King:: wakeWordTrigger::onKeyWordDetected.....beginIndex = %llu, endIndex = %llu,\n", APP_TAG.c_str(), beginIndex, endIndex);
            if (keyWordDetected) {
              printf("Alexa wakeWordTrigger::wakeword has detected!!!!!\n");
              return;
            }

            keyWordDetected = true;
            ASSERT_NE(nullptr, stream);
            bool alwaysReadable = true;
            bool canOverride = false;
            bool canBeOverridden = true;
            auto audioProvider = AudioProvider( stream, m_compatibleAudioFormat,
            ASRProfile::NEAR_FIELD, alwaysReadable, !canOverride, canBeOverridden);

            if (m_aip) {
                AudioInputStream::Index aipBegin = AudioInputProcessor::INVALID_INDEX;
                AudioInputStream::Index aipEnd = AudioInputProcessor::INVALID_INDEX;

                if (endIndex != KeyWordObserverInterface::UNSPECIFIED_INDEX) {
                    if (beginIndex != KeyWordObserverInterface::UNSPECIFIED_INDEX) {
                        // If we know where the keyword starts and ends, pass both of those along to AIP.
                        aipBegin = beginIndex;
                        aipEnd = endIndex;
                    } else {
                        // If we only know where the keyword ends, AIP should begin recording there.
                        aipBegin = endIndex;
                    }
                }
                // Else we don't have any indices to pass along; AIP will begin recording ASAP.
#ifdef KWD_KITTAI
                m_aip->recognize(audioProvider, Initiator::TAP, aipBegin, aipEnd, keyword);
#elif KWD_SENSORY
                printf("%s King:: wakeWordTrigger::Sensory recognize.....beginIndex = %llu, endIndex = %llu\n", APP_TAG.c_str(), aipBegin, aipEnd);  // King 201708
                if (recognizeInitiatorType == InitiatorType_WAKEWORD) {
                  mWisCommon.sysLocalTime("---> reset state!");
                  mWisAvs.m_AudioInputProcessor->resetState();  // King end
                  m_aip->recognize(audioProvider, Initiator::WAKEWORD, aipBegin, aipEnd, keyword);  // King debug
                }
#endif
            }
        }

    bool keyWordDetected = false;
    AudioFormat m_compatibleAudioFormat;
    std::shared_ptr<AudioInputProcessor> m_aip;
};
#endif

class testStateProvider : public StateProviderInterface{
public:
    testStateProvider(std::shared_ptr<avsCommon::sdkInterfaces::ContextManagerInterface> contextManager) {
        m_contextManager = contextManager;
    }
    ~testStateProvider() {
    }
    void provideState(const unsigned int stateRequestToken) {
        std::ostringstream context;

        //Bob update
        int current_volume = getCurrentMediaVolume();

        bool isMuted;
        if(current_volume > 0){
          isMuted = false;
        }else {
          isMuted = true;
        } //Bob end

        context << R"({)"
                   R"("volume":)" << std::to_string((int)(current_volume*100.0/VOLUME_LEVEL)) << R"(,)"
            << R"("muted":)" << isMuted << R"(})";

        m_contextManager->setState(VOLUME_STATE_PAIR,
            context.str(), avsCommon::avs::StateRefreshPolicy::ALWAYS,
            stateRequestToken);
        m_stateRequested = true;
    }
    bool checkStateRequested() {
        bool savedResult = false;
        if (m_stateRequested) {
            savedResult = true;
            m_stateRequested = false;
        }
        return savedResult;
    }
private:
   bool m_stateRequested  = false;
   std::shared_ptr<avsCommon::sdkInterfaces::ContextManagerInterface> m_contextManager;

};

class testKeyWordObserverDebug : public KeyWordObserverInterface {
public:
  testKeyWordObserverDebug(AudioFormat compatibleAudioFormat, std::shared_ptr<AudioInputProcessor> aip) {
      m_compatibleAudioFormat = compatibleAudioFormat;
      m_aip = aip;
  }
void onKeyWordDetected(
  std::shared_ptr<AudioInputStream> stream,
  std::string keyword,
  AudioInputStream::Index beginIndex,
  AudioInputStream::Index endIndex) {
      printf(" *** testKeyWordObserverDebug:: onKeyWordDetected\n");
      if (isSoftAPModeOn) {
        printf(" *** testKeyWordObserverDebug:: keyWordDetected, but Hotspot_Mode is on, so skip this request.\n");
        return;
      }
      isLaunchAVSKeyPressed = false;
      closeFMDuringTest();
      stopMediaPlayer();
      // King 201708

      m_wakewordBeginIndex = beginIndex;
      m_wakewordEndIndex = endIndex;
      printf("%s  King:: testKeyWordObserverDebug::onKeyWordDetected.....beginIndex = %llu, endIndex = %llu,\n", APP_TAG.c_str(), beginIndex, endIndex);
      sensoryKeyWordDetected = true;
      m_wisStateTrigger.notify_all();
  }

  AudioFormat m_compatibleAudioFormat;
  std::shared_ptr<AudioInputProcessor> m_aip;
};

/// A test observer that mocks out the KeyWordDetectorStateObserverInterface##onStateChanged() call.
class testStateObserverDebug : public KeyWordDetectorStateObserverInterface {
public:
    testStateObserverDebug() :
        m_state(KeyWordDetectorStateObserverInterface::KeyWordDetectorState::STREAM_CLOSED),
        m_stateChangeOccurred{false} {
    }

    /// Implementation of the KeyWordDetectorStateObserverInterface##onStateChanged() call.
    void onStateChanged(KeyWordDetectorStateObserverInterface::KeyWordDetectorState keyWordDetectorState) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_state = keyWordDetectorState;
        m_stateChangeOccurred = true;
        m_stateChanged.notify_one();
    }

    /**
     * Waits for the KeyWordDetectorStateObserverInterface##onStateChanged() call.
     *
     * @param timeout The amount of time to wait for the call.
     * @param stateChanged An output parameter that notifies the caller whether a call occurred.
     * @return Returns the state of the observer.
     */
    KeyWordDetectorStateObserverInterface::KeyWordDetectorState waitForStateChange(
            std::chrono::milliseconds timeout, bool* stateChanged) {
        std::unique_lock<std::mutex> lock(m_mutex);
        bool success = m_stateChanged.wait_for(lock, timeout, [this] () {
            return m_stateChangeOccurred;
        });

        if (!success) {
            *stateChanged = false;
        } else {
            m_stateChangeOccurred = false;
            *stateChanged = true;
        }
        return m_state;
    }
private:
    /// The state of the observer.
    KeyWordDetectorStateObserverInterface::KeyWordDetectorState m_state;
    /// A boolean flag so that we can re-use the observer even after a callback has occurred.
    bool m_stateChangeOccurred;
    /// A lock to guard against state changes.
    std::mutex m_mutex;
    /// A condition variable to wait for state changes.
    std::condition_variable m_stateChanged;
};
// King end

// King add
class TestWisStatusChanged : public WisStatusChangedInterface {
public:
  bool isNewDirectiveArrived = false;
  bool isStopCaptureFlag = false;
  std::vector<std::shared_ptr<AVSDirective>> m_avsDirectives;

  void onAudioPlayerPlayStatusChanged(int state){
    /*
    switch (playerActivity) {
        case PlayerActivity::IDLE:
            return 0;
        case PlayerActivity::PLAYING:
            return 1;
        case PlayerActivity::STOPPED:
            return 2;
        case PlayerActivity::PAUSED:
            return 3;
        case PlayerActivity::BUFFER_UNDERRUN:
            return 4;
        case PlayerActivity::FINISHED:
            return 5;
    }
    */
    printf("\tKing:: onAudioPlayerPlayStatusChanged to %d\n", state);
    std::string content;
    if (state == 1) { // PLAYING
      isWaitPlayingEventFlag = false;
      m_wisStateTrigger.notify_all();

      content = "play_music:status=1";
      isAudioPlayerPlaying = true;
    }else{ // STOPPED
      content = "play_music:status=0";
      isAudioPlayerPlaying = false;
    }

    if (state == 2 || state == 5) {
      openFMDuringTest();
    }

    mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
  }

  void onNotificationStatusChanged(NotificatonState state){
    printf("\tKing  TestWisStatusChanged::onNotificationStatusChanged to %d\n", state);
    if (state == WisStatusChangedInterface::NotificatonState::SET_INDICATOR) {
      if (mWisCommon.getCurrentSKU() == SKU_WCD) {
        if (access(SOUND_CUE_TEST_FILE_PATH.c_str(),F_OK) != -1) { // File exist
          std::string delayInMillSeconds = mWisCommon.getValueWithKey(SOUND_CUE_TEST_FILE_PATH, SOUND_CUE_TEST_KEY_DELAY_IN_MILL_SECONDS);
          m_speakMediaPlayer->sleepInMillSeconds = atoi(delayInMillSeconds.c_str());
          m_speakMediaPlayer->isTesting=true; // King test: [Sound cue]
        }
      }
      std::cout << "Receive Notifications and open LED" << '\n';
      isNotificationOngoing = true;
      mWisCommon.lightLED(LED_TYPE_NOTIFICATION_ARRIVES);
      sleep(1);
      mWisCommon.lightLED(LED_TYPE_NOTIFICATION_QUEUED);
      // Play Notification cue
      mWisCommon.sysLocalTime("Start to play med_alerts_notification.mp3");
      if(isPlayEnabled){
        if (mWisCommon.getCurrentSKU() == SKU_WCD) {
          if (wcdPlaySoundCueType == WCD_PLAY_SOUND_CUE_TYPE_MEDIA_PLAYER) {
            std::string LOCAL_AUDIO_FILE_PATH = APP_ROOT_FOLDER + "med_alerts_notification.mp3";
            std::cout<<LOCAL_AUDIO_FILE_PATH<<std::endl;
            m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);

            if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
              if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
                while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
                  ;
                }
                m_localMediaPlayer.m_mediaPlayer->stop();
              }
            }
          }else{
            system((APP_ROOT_FOLDER + SHELL_SCRIPT_WCD_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_NOTIFICATION).c_str());
          }
        }else{
          system((APP_ROOT_FOLDER + SHELL_SCRIPT_CX_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_NOTIFICATION).c_str());
        }
      }
      mWisCommon.sysLocalTime("End play med_alerts_notification.mp3");
    }else if (state == WisStatusChangedInterface::NotificatonState::CLEAR_INDICATOR) {
      isNotificationOngoing = false;
      restoreDefaultLED();
    }
  }

  void onDirectiveArrived(std::shared_ptr<AVSDirective> directive){
    printf("%s\n", "...onDirectiveArrived..." );
    m_currentDirective = directive;

    if (wakeWordDetector == WAKE_WORD_DETECTOR_SVA) {
      if(directive->getNamespace() != "Notifications" && directive->getName() != "SetIndicator"){
        m_currentNotificationAssetId = "";
      }
      if(directive->getNamespace() == "SpeechRecognizer"){
        if (directive->getName() == "StopCapture") {
          printf("onDirectiveArrived: %s\n", "To stop recording...");
          if (wakeWordDetector == WAKE_WORD_DETECTOR_SVA) {
            isStopCaptureFlag = true;
          } else if (wakeWordDetector == WAKE_WORD_DETECTOR_SENSORY) {
            if (sensoryKeyWordDetected) {
              isStopCaptureFlag = true;
            }
          }
        }else if (directive->getName() == "ExpectSpeech") {
          // blank
        }
        return;
      } else if(directive->getNamespace() == "Notifications"&& directive->getName() == "SetIndicator"){ //SetIndicator Directive
        std::string asset;
        std::string assetId;
        jsonUtils::lookupStringValue(directive->getPayload(), "asset", &asset);
        jsonUtils::lookupStringValue(asset, "assetId", &assetId);
        if(m_currentNotificationAssetId.find(assetId, 0) != 0){
  	       m_currentNotificationAssetId = assetId;
           onNotificationStatusChanged(WisStatusChangedInterface::NotificatonState::SET_INDICATOR);
        }
      } else if(directive->getNamespace() == "Notifications"&& directive->getName() == "ClearIndicator"){//ClearIndicator Directive
        onNotificationStatusChanged(WisStatusChangedInterface::NotificatonState::CLEAR_INDICATOR);
      } else if(directive->getNamespace() == "Alerts"&& directive->getName() == "SetAlert"){ //SetAlert Directive
        printf("%s\n", "SetAlert directive");
        mWisCommon.sysLocalTime("current directives is alerts interface");
      } else if(directive->getNamespace() == "Alerts"&& directive->getName() == "DeleteAlert"){ //DeleteAlert Directive
        printf("%s\n", "DeleteAlert directive");
        mWisCommon.sysLocalTime("current directives is alerts interface");
      } else if(directive->getNamespace() == "AudioPlayer" && directive->getName() == "Stop"){ // AudioPlayer stop
        printf("%s\n", "AudioPlayer Stop directive");
      } else {
        std::cout << "----------------- default directive"<< '\n';
      }

      isNewDirectiveArrived = true;
      if (directive->getNamespace() == "Speaker"
            || directive->getNamespace() == "SpeechRecognizer"
            || directive->getNamespace() == "SpeechSynthesizer") {
        m_avsDirectives.push_back(directive);
        if(directive->getNamespace() == "Speaker"){ //Speaker Interface
          if(directive->getName() == "SetVolume"){ //Speaker Interface SetVolume
             mWisVolumeControl.setVolume(directive->getPayload());
           }
         }
      }
      m_wisStateTrigger.notify_all();
    }
    else if (wakeWordDetector == WAKE_WORD_DETECTOR_SENSORY) {

      if(directive->getNamespace() != "Notifications" && directive->getName() != "SetIndicator"){
        m_currentNotificationAssetId = "";
      }
      if (isStopCaptureWaitNextDirectiveFlag) {
        isStopCaptureWaitNextDirectiveFlag = false;
        m_wisStateTrigger.notify_all();
      }

      if(directive->getNamespace() == "SpeechRecognizer"){  // *** SpeechRecognizer
        if (directive->getName() == "StopCapture") {
          printf("onDirectiveArrived: %s, sensoryKeyWordDetected = %d\n", "To stop recording...", sensoryKeyWordDetected);
          if (wakeWordDetector == WAKE_WORD_DETECTOR_SVA) {
            isStopCaptureFlag = true;
          } else if (wakeWordDetector == WAKE_WORD_DETECTOR_SENSORY) {
            if (sensoryKeyWordDetected) {
              isStopCaptureFlag = true;
            }
          }
          m_wisStateTrigger.notify_all();

          isStopCaptureWaitNextDirectiveFlag = true;
          pthread_t checkAvsResponseTimeoutThread;
          pthread_create(&checkAvsResponseTimeoutThread, NULL, checkAvsResponseTimeout,NULL);

          mWisCommon.lightLED(LED_TYPE_THINKING);
        }
        return;
      } else if(directive->getNamespace() == "Notifications"){  // *** Notifications
        if(directive->getName() == "SetIndicator"){ //SetIndicator Directive
          std::string asset;
          std::string assetId;
          jsonUtils::lookupStringValue(directive->getPayload(), "asset", &asset);
          jsonUtils::lookupStringValue(asset, "assetId", &assetId);
          if(m_currentNotificationAssetId.find(assetId, 0) != 0){
    	       m_currentNotificationAssetId = assetId;
             onNotificationStatusChanged(WisStatusChangedInterface::NotificatonState::SET_INDICATOR);
          }
        } else if (directive->getName() == "ClearIndicator") {  //ClearIndicator Directive
          onNotificationStatusChanged(WisStatusChangedInterface::NotificatonState::CLEAR_INDICATOR);
        }
        return;
      } else if(directive->getNamespace() == "Alerts"){  // *** Alerts
        if(directive->getName() == "SetAlert"){ //SetAlert Directive
          printf("%s\n", "SetAlert directive" );
          mWisCommon.sysLocalTime("current directives is alerts interface");
        } else if (directive->getName() == "DeleteAlert"){ //DeleteAlert Directive
          printf("%s\n", "DeleteAlert directive");
          mWisCommon.sysLocalTime("current directives is alerts interface");
        }
      } else if (directive->getNamespace() == "SpeechSynthesizer") {  // *** SpeechSynthesizer
        std::string directiveName = directive->getName();
        if (directiveName == "Speak") {
          isWaitPlayingEventFlag = true;
          pthread_t checkPlayingEventTimeoutThread;
          pthread_create(&checkPlayingEventTimeoutThread, NULL, checkPlayingEventTimeout,NULL);

          mWisCommon.lightLED(LED_TYPE_THINKING);
          return;
        }
      } else if(directive->getNamespace() == "Speaker"){ // *** Speaker
        std::string directiveName = directive->getName();
        mWisCommon.sysLocalTime(("namespace value: "+ directive->getNamespace()).c_str());
        mWisCommon.sysLocalTime(("name value: "+ directive->getName()).c_str());
        mWisCommon.sysLocalTime(("payload value: "+ directive->getPayload()).c_str());
        mWisCommon.sysLocalTime(("messageId value: "+ directive->getMessageId()).c_str());

        if(directiveName == "SetVolume"){ //Speaker Interface SetVolume
           mWisVolumeControl.setVolume(directive->getPayload());
        }else if(directiveName == "AdjustVolume"){ //Speaker Interface AdjustVolume
           mWisVolumeControl.adjustVolume(directive->getPayload());
        }else if(directiveName == "SetMute"){ //Speaker Interface SetMute
           mWisVolumeControl.setMuteOrUnmute(directive->getPayload());
        }

        int current_volume = getCurrentMediaVolume();

        std::string isMuted;
        if(current_volume > 0){
          isMuted = "0";
        }else {
          isMuted = "1";
        }

        std::string volumeEvent;
        if(directiveName == "SetVolume" || directiveName == "AdjustVolume"){
          volumeEvent = mWisCommon.replace_all(volumeChangedEvent,"current_volume",std::to_string((int)(current_volume*100.0/VOLUME_LEVEL)));
          volumeEvent = mWisCommon.replace_all(volumeEvent,"isMuted",isMuted);
        }else if(directiveName == "SetMute"){
          volumeEvent = mWisCommon.replace_all(muteChangedEvent,"current_volume",std::to_string((int)(current_volume*100.0/VOLUME_LEVEL)));
          volumeEvent = mWisCommon.replace_all(volumeEvent,"isMuted",isMuted);
        }
        std::string messageId = avsCommon::utils::uuidGeneration::generateUUID();
        volumeEvent = mWisCommon.replace_all(volumeEvent,"messageId123",messageId);
        mWisCommon.sysLocalTime(("Send volume event: "+volumeEvent).c_str());

        auto messageRequest = std::make_shared<ObservableMessageRequest>(volumeEvent, nullptr);
       //  m_avsConnectionManager->sendMessage(messageRequest);
        mWisAvs.m_connectionManager->sendMessage(messageRequest);
        sendCurrentMediaVolume();
      } else if (directive->getNamespace() == "AudioPlayer"){ // *** AudioPlayer
        /*std::string directiveName = directive->getName();
        if (directiveName == "Play") {
          isWaitPlayingEventFlag = true;
          pthread_t checkPlayingEventTimeoutThread;
          pthread_create(&checkPlayingEventTimeoutThread, NULL, checkPlayingEventTimeout,NULL);

          mWisCommon.lightLED(LED_TYPE_THINKING);
        } else { // Stop, ClearQueue
          restoreDefaultLED();
        }*/
      } else {
        std::cout << "----------------- default directive"<< '\n';
      }
      restoreDefaultLED();
    }
  }

  void onSpeechSynthesizerObserverStateChanged(SpeechSynthesizerObserver::SpeechSynthesizerState state){
    if (state == SpeechSynthesizerObserver::SpeechSynthesizerState::PLAYING) {
      isWaitPlayingEventFlag = false;
      m_wisStateTrigger.notify_all();

      isTTSPlaying = true;
      mWisCommon.lightLED(LED_TYPE_SPEAKING);
    } else if (state == SpeechSynthesizerObserver::SpeechSynthesizerState::FINISHED) {
      isTTSPlaying = false;

      printf("*** onSpeechSynthesizerObserverStateChanged::  isRecordingFinish = %d\n", isRecordingFinish);
      /*if (isAudioPlayerPlaying) {
        onAudioPlayerPlayStatusChanged(1);
        return;
      }*/
      if (!isSVADetected && !sensoryKeyWordDetected) {
        openFMDuringTest();
      }
      if (!sensoryKeyWordDetected) {
        restoreDefaultLED();
      }
      m_speakMediaPlayer->stop();
    }
  }

  void onSpeechSynthesizerStateChangeFailed(){
    restoreDefaultLED();
  }

  void onHandleExpectSpeechDirective(){
    printf("*** WisStatusChangedInterface::onHandleExpectSpeechDirective:: ***\n");
    isExpectDirectStatus = true;
    m_wisStateTrigger.notify_all();
  }

  void onHandleAudioPlayerPlayDirective(){
    printf("*** WisStatusChangedInterface::onHandleAudioPlayerPlayDirective:: ***\n");
    if (!sensoryKeyWordDetected) {
      restoreDefaultLED();
    }
  }

private:
  std::string m_currentNotificationAssetId;
  std::shared_ptr<AVSDirective> m_currentDirective;
};
// King end

/// A test observer that mocks out the ChannelObserverInterface##onFocusChanged() call.
class TestClient : public ChannelObserverInterface {
public:
    /**
     * Constructor.
     */
    TestClient() :
        m_focusState(FocusState::NONE) {
    }

    /**
     * Implementation of the ChannelObserverInterface##onFocusChanged() callback.
     *
     * @param focusState The new focus state of the Channel observer.
     */
    void onFocusChanged(FocusState focusState) override {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push_back(focusState);
        m_focusState = focusState;
        m_wakeTrigger.notify_all();
    }

    /**
     * Waits for the ChannelObserverInterface##onFocusChanged() callback.
     *
     * @param timeout The amount of time to wait for the callback.
     * @param focusChanged An output parameter that notifies the caller whether a callback occurred.
     * @return Returns @c true if the callback occured within the timeout period and @c false otherwise.
     */
    FocusState waitForFocusChange(std::chrono::milliseconds timeout) {
        FocusState ret;
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_wakeTrigger.wait_for(lock, timeout, [this]() { return !m_queue.empty(); })) {
            ret = m_focusState;
            return ret;
        }
        ret = m_queue.front();
        m_queue.pop_front();
        return ret;
    }

    FocusState getCurrentFocusState() {
        return m_focusState;
    }

private:
    /// The focus state of the observer.
    FocusState m_focusState;

    /// A lock to guard against focus state changes.
    std::mutex m_mutex;

    /// Trigger to wake up waitForNext calls.
    std::condition_variable m_wakeTrigger;
    /// Queue of received focus states that have not been waited on.
    std::deque<FocusState> m_queue;
};

class Nut2AVSTest : public ::testing::Test {  // King update
protected:
    virtual void SetUp() override {
        std::ifstream infile(configPath);
        ASSERT_TRUE(infile.good());
        ASSERT_TRUE(AlexaClientSDKInit::initialize({&infile}));
        m_authObserver = std::make_shared<AuthObserver>();
        m_authDelegate = AuthDelegate::create();
        m_authDelegate->addAuthObserver(m_authObserver);
        m_attachmentManager = std::make_shared<avsCommon::avs::attachment::AttachmentManager>(
                 AttachmentManager::AttachmentType::IN_PROCESS);
        m_connectionStatusObserver = std::make_shared<ConnectionStatusObserver>();
        m_clientMessageHandler = std::make_shared<ClientMessageHandler>(m_attachmentManager);
        bool isEnabled = false;
        m_messageRouter = std::make_shared<HTTP2MessageRouter>(m_authDelegate, m_attachmentManager);
        m_exceptionEncounteredSender = std::make_shared<TestExceptionEncounteredSender>();

        DirectiveHandlerConfiguration config;
        config[SET_MUTE_PAIR] = BlockingPolicy::BLOCKING;
        config[ADJUST_VOLUME_PAIR] = BlockingPolicy::BLOCKING;
        config[EXPECT_SPEECH_PAIR] = BlockingPolicy::BLOCKING;
        m_directiveHandler = std::make_shared<TestDirectiveHandler>(config);

        // King add
        m_wisStatusChanged = std::make_shared<TestWisStatusChanged>();

        m_directiveSequencer = DirectiveSequencer::create(m_exceptionEncounteredSender);
        m_messageInterpreter = std::make_shared<MessageInterpreter>(
            m_exceptionEncounteredSender, m_directiveSequencer, m_attachmentManager);
        m_directiveSequencer->setStatusObserver(m_wisStatusChanged);  // King add

		    // Bob start
        m_compatibleAudioFormat.sampleRateHz = COMPATIBLE_SAMPLE_RATE;
        m_compatibleAudioFormat.sampleSizeInBits = COMPATIBLE_SAMPLE_SIZE_IN_BITS;
        m_compatibleAudioFormat.numChannels = COMPATIBLE_NUM_CHANNELS;
        m_compatibleAudioFormat.endianness = COMPATIBLE_ENDIANNESS;
        m_compatibleAudioFormat.encoding = COMPATIBLE_ENCODING;

        unsigned int SAMPLE_RATE_HZ = 16000;
        size_t wordSize = 2;
        size_t maxReaders = 10;
        std::chrono::seconds AMOUNT_OF_AUDIO_DATA_IN_BUFFER = std::chrono::seconds(16);
        size_t BUFFER_SIZE_IN_SAMPLES = (SAMPLE_RATE_HZ) * AMOUNT_OF_AUDIO_DATA_IN_BUFFER.count();
        size_t bufferSize = AudioInputStream::calculateBufferSize(BUFFER_SIZE_IN_SAMPLES, wordSize, maxReaders);

        auto m_Buffer = std::make_shared<avsCommon::avs::AudioInputStream::Buffer>(bufferSize);
        auto m_Sds = avsCommon::avs::AudioInputStream::create(m_Buffer, wordSize, maxReaders);
        ASSERT_NE (nullptr, m_Sds);
        m_AudioBuffer = std::move(m_Sds);
        m_AudioBufferWriter = m_AudioBuffer->createWriter(
            avsCommon::avs::AudioInputStream::Writer::Policy::NONBLOCKABLE);
        ASSERT_NE (nullptr, m_AudioBufferWriter);
        // King add: fill 2000 bytes blank data
        std::vector<int16_t> audioData (4000, 0);
        m_AudioBufferWriter->write(audioData.data(), audioData.size());

        // Set up tap and hold to talk buttons.
        bool alwaysReadable = true;
        bool canOverride = true;
        bool canBeOverridden = true;
        m_TapToTalkAudioProvider = std::make_shared<AudioProvider>( m_AudioBuffer, m_compatibleAudioFormat,
            ASRProfile::NEAR_FIELD, alwaysReadable, canOverride, !canBeOverridden);

        m_tapToTalkButton = std::make_shared<tapToTalkButton>();
        m_dialogUXStateAggregator = std::make_shared<avsCommon::avs::DialogUXStateAggregator>();

        mWisAvs.m_contextManager = ContextManager::create();
        ASSERT_NE(nullptr, mWisAvs.m_contextManager);
        PostConnectObject::init(mWisAvs.m_contextManager );

        //m_speechSynthesizerStateProvider = MockStateProvider::create(mWisAvs.m_contextManager, SPEECH_STATE_PAIR,
        //      SPEECH_SYNTHESIZER_PAYLOAD_FINISHED, StateRefreshPolicy::NEVER, DEFAULT_SLEEP_TIME);
        //m_audioPlayerStateProvider = MockStateProvider::create(mWisAvs.m_contextManager, PLAYBACK_STATE_PAIR,
        //        AUDIO_PLAYER_PAYLOAD, StateRefreshPolicy::NEVER, DEFAULT_SLEEP_TIME);
        //mWisAvs.m_contextManager->setStateProvider(SPEECH_STATE_PAIR, m_speechSynthesizerStateProvider);
        //mWisAvs.m_contextManager->setStateProvider(PLAYBACK_STATE_PAIR, m_audioPlayerStateProvider);
        //m_contextRequester = MockContextRequester::create(mWisAvs.m_contextManager);
        m_stateProvider = std::make_shared<testStateProvider>(mWisAvs.m_contextManager);
        mWisAvs.m_contextManager->setStateProvider(VOLUME_STATE_PAIR, m_stateProvider);
		    // Bob end

        //Bob add 20170818
        FocusManager::ChannelConfiguration dialogChannelConfig{DIALOG_CHANNEL_NAME, DIALOG_CHANNEL_PRIORITY};
        FocusManager::ChannelConfiguration contentChannelConfig{CONTENT_CHANNEL_NAME, CONTENT_CHANNEL_PRIORITY};
        FocusManager::ChannelConfiguration testChannelConfig{TEST_CHANNEL_NAME, TEST_CHANNEL_PRIORITY};
        FocusManager::ChannelConfiguration alertsChannelConfig{ALERTS_CHANNEL_NAME, ALERTS_CHANNEL_PRIORITY};

        // std::vector<FocusManager::ChannelConfiguration> channelConfigurations {
            // dialogChannelConfig, contentChannelConfig, testChannelConfig
        // };
        std::vector<FocusManager::ChannelConfiguration> channelConfigurations {
            dialogChannelConfig, alertsChannelConfig,contentChannelConfig
        };

        m_focusManager = std::make_shared<FocusManager>(channelConfigurations);
        //Bob end 20170818

        // Set up connection and connect
        /*m_avsConnectionManager = std::make_shared<TestMessageSender>(
                m_messageRouter,
                isEnabled,
                m_connectionStatusObserver,
                m_messageInterpreter);
        ASSERT_NE (nullptr, m_avsConnectionManager);*/ // King remove
        mWisAvs.m_connectionManager = acl::AVSConnectionManager::create(m_messageRouter, isEnabled, { m_connectionStatusObserver }, { m_messageInterpreter }); // King add
        //mWisAvs.m_connectionManager->onStateChanged(StateSynchronizerObserverInterface::State::SYNCHRONIZED);
        //connect();

       //m_exceptionEncounteredSender->setMessageSender(mWisAvs.m_connectionManager);

		   // Bob start
        m_userInactivityMonitor = UserInactivityMonitor::create(
            //m_avsConnectionManager,
            mWisAvs.m_connectionManager, // King update
            m_exceptionEncounteredSender/*,
            std::chrono::seconds(30)*/); // King [System] debug
        mWisAvs.m_AudioInputProcessor = AudioInputProcessor::create(
            m_directiveSequencer,
            //m_avsConnectionManager,
            mWisAvs.m_connectionManager, // King updated
            mWisAvs.m_contextManager,
            m_focusManager,
            m_dialogUXStateAggregator,
            m_exceptionEncounteredSender,
            m_userInactivityMonitor
        );
        ASSERT_NE (nullptr, mWisAvs.m_AudioInputProcessor);
        mWisAvs.m_AudioInputProcessor->addObserver(m_dialogUXStateAggregator);
        mWisAvs.m_AudioInputProcessor->setStatusObserver(m_wisStatusChanged);  // King add
        m_directiveSequencer->addDirectiveHandler(m_userInactivityMonitor);  // King [System] debug
		    // Bob end

        m_testClient = std::make_shared<TestClient>();

        ASSERT_TRUE(m_focusManager->acquireChannel(CONTENT_CHANNEL_NAME, m_testClient, CONTENT_ACTIVITY_ID));
        ASSERT_EQ(m_testClient->waitForFocusChange(WAIT_FOR_TIMEOUT_DURATION), FocusState::FOREGROUND);

		    // Bob start
        m_StateObserver = std::make_shared<AipStateObserver>();
        ASSERT_NE(nullptr, m_StateObserver);
        mWisAvs.m_AudioInputProcessor->addObserver(m_StateObserver);

        ASSERT_TRUE(m_directiveSequencer->addDirectiveHandler(mWisAvs.m_AudioInputProcessor));

        if (wakeWordDetector !=  WAKE_WORD_DETECTOR_SVA) {
            #ifdef KWD
                    m_wakeWordTrigger = std::make_shared<wakeWordTrigger>(m_compatibleAudioFormat, mWisAvs.m_AudioInputProcessor);

                #ifdef KWD_KITTAI
                        kwd::KittAiKeyWordDetector::KittAiConfiguration config;
                        config = {inputPath+MODEL_FILE, MODEL_KEYWORD, KITTAI_SENSITIVITY};
                        m_detector = kwd::KittAiKeyWordDetector::create(
                                m_AudioBuffer,
                                m_compatibleAudioFormat,
                                {m_wakeWordTrigger},
                                // Not using an empty initializer list here to account for a GCC 4.9.2 regression
                                std::unordered_set<std::shared_ptr<KeyWordDetectorStateObserverInterface>>(),
                                inputPath + RESOURCE_FILE,
                                {config},
                                2.0,
                                false);
                        ASSERT_TRUE(m_detector);
                #elif KWD_SENSORY
                        m_detector = kwd::SensoryKeywordDetector::create(
                                m_AudioBuffer,
                                m_compatibleAudioFormat,
                                {m_wakeWordTrigger},
                                // Not using an empty initializer list here to account for a GCC 4.9.2 regression
                                std::unordered_set<std::shared_ptr<KeyWordDetectorStateObserverInterface>>(),
                                inputPath + RESOURCE_FILE);
                        ASSERT_TRUE(m_detector);
                #endif
          #endif
        }
		// Bob end

#ifdef GSTREAMER_MEDIA_PLAYER
        m_speakMediaPlayer = MediaPlayer::create(std::make_shared<HTTPContentFetcherFactory>());
#else
        m_speakMediaPlayer = std::make_shared<TestMediaPlayer>();
#endif

        // Create and register the SpeechSynthesizer.
        m_speechSynthesizer = SpeechSynthesizer::create(
                m_speakMediaPlayer,
                //m_avsConnectionManager,
                mWisAvs.m_connectionManager, // King updated
                m_focusManager,
                mWisAvs.m_contextManager,
                m_attachmentManager,
                m_exceptionEncounteredSender);
        m_directiveSequencer->addDirectiveHandler(m_speechSynthesizer);
        m_speechSynthesizer->setStatusObserver(m_wisStatusChanged);  // King add
        m_speechSynthesizerObserver = std::make_shared<TestSpeechSynthesizerObserver>();
        m_speechSynthesizer->addObserver(m_speechSynthesizerObserver);
        m_speechSynthesizerObserver->setStatusObserver(m_wisStatusChanged);  // King add

        // ASSERT_TRUE(m_directiveSequencer->addDirectiveHandler(m_directiveHandler)); //Bob update
//Bob add 20170818 for audioplayer
#ifdef GSTREAMER_MEDIA_PLAYER
        m_contentMediaPlayer = MediaPlayer::create(std::make_shared<HTTPContentFetcherFactory>());
#else
        m_contentMediaPlayer = std::make_shared<TestMediaPlayer>();
#endif
        // Create and register the AudioPlayer.
        mWisAvs.m_audioPlayer = capabilityAgents::audioPlayer::AudioPlayer::create(
            m_contentMediaPlayer,
            // m_avsConnectionManager,
            mWisAvs.m_connectionManager,
            m_focusManager,
            mWisAvs.m_contextManager,
            m_attachmentManager,
            m_exceptionEncounteredSender);
        ASSERT_NE(nullptr, mWisAvs.m_audioPlayer);
        m_directiveSequencer->addDirectiveHandler(mWisAvs.m_audioPlayer);
        mWisAvs.m_audioPlayer->setStatusObserver(m_wisStatusChanged);  // King add
//Bob end 20170818

        //Bob add 20170731
        #ifdef GSTREAMER_MEDIA_PLAYER
            m_rendererMediaPlayer = MediaPlayer::create(nullptr);
        #else
            m_rendererMediaPlayer = std::make_shared<TestMediaPlayer>();
        #endif
        m_alertRenderer = renderer::Renderer::create(m_rendererMediaPlayer);
        m_alertStorage = std::make_shared<storage::SQLiteAlertStorage>();
        m_alertObserver = std::make_shared<TestAlertObserver>();
        auto messageStorage = std::make_shared<SQLiteMessageStorage>();
        m_certifiedSender = CertifiedSender::create(
            mWisAvs.m_connectionManager, mWisAvs.m_connectionManager, messageStorage);

        m_alertsAgent = AlertsCapabilityAgent::create(
            // m_avsConnectionManager,
            mWisAvs.m_connectionManager, //Bob update
            m_certifiedSender,
            m_focusManager,
            mWisAvs.m_contextManager,
            m_exceptionEncounteredSender,
            m_alertStorage,
            m_alertRenderer);
        ASSERT_NE(m_alertsAgent, nullptr);
        m_alertsAgent->addObserver(m_alertObserver);
        m_alertsAgent->onLocalStop();
        // m_alertsAgent->removeAllAlerts();
        m_directiveSequencer->addDirectiveHandler(m_alertsAgent);

        // m_avsConnectionManager->addConnectionStatusObserver(m_alertsAgent);
        mWisAvs.m_connectionManager->addConnectionStatusObserver(m_alertsAgent); //Bob update

        //Bob add for settings 20171019
        m_settingsEventSender = SettingsUpdatedEventSender::create(mWisAvs.m_connectionManager);
        ASSERT_NE(m_settingsEventSender, nullptr);
        m_storage = std::make_shared<SQLiteSettingStorage>();
        mWisAvs.m_settingsObject = Settings::create(m_storage, {m_settingsEventSender});
        //Bob end 20171019

        //Bob add for PlaybackController 20171020
        mWisAvs.m_playbackController = PlaybackController::create(mWisAvs.m_contextManager, mWisAvs.m_connectionManager);
        //Bob end for 20171020

        connect();
        //Bob end 20170731
    }

    /**
     * Send and event to AVS. Blocks until a status is received.
     *
     * @param message The message to send.
     * @param expectStatus The status to expect from the call to send the message.
     * @param timeout How long to wait for a result from delivering the message.
     */
    void sendEvent(
        const std::string& jsonContent,
        std::shared_ptr<avsCommon::avs::attachment::AttachmentReader> attachmentReader,
        avsCommon::sdkInterfaces::MessageRequestObserverInterface::Status expectedStatus,
        std::chrono::seconds timeout) {
        auto messageRequest = std::make_shared<ObservableMessageRequest>(jsonContent, attachmentReader);
        m_avsConnectionManager->sendMessage(messageRequest);
        ASSERT_TRUE(messageRequest->waitFor(expectedStatus, timeout));
    }

    /**
     * Function to setup a message and send it to AVS.
     *
     * @param json A JSON string containing the message to send.
     * @param expectStatus The status to expect from the call to send the message.
     * @param timeout How long to wait for a result from delivering the message.
     */
    void setupMessageAndSend(
        const std::string& json,
        avsCommon::sdkInterfaces::MessageRequestObserverInterface::Status expectedStatus,
        std::chrono::seconds timeout) {
        sendEvent(json, nullptr, expectedStatus, timeout);
    }

    /**
     * Function to setup a message with an attachment and send it to AVS.
     *
     * @param json A JSON string containing the message to send.
     * @param file Name of the file to read the attachment from.
     * @param expectStatus The status to expect from the call to send the message.
     * @param timeout How long to wait for a result from delivering the message.
     */
    void setupMessageWithAttachmentAndSend(
        const std::string& json,
        std::string& file,
        avsCommon::sdkInterfaces::MessageRequestObserverInterface::Status expectedStatus,
        std::chrono::seconds timeout) {
        auto is = std::make_shared<std::ifstream>(file, std::ios::binary);
        ASSERT_TRUE(is->is_open());

        const int mbBytes = 1024 * 1024;

        std::vector<char> localBuffer(mbBytes);

        auto bufferSize = InProcessSDS::calculateBufferSize(localBuffer.size());
        auto buffer = std::make_shared<InProcessSDSTraits::Buffer>(bufferSize);
        std::shared_ptr<InProcessSDS> sds = InProcessSDS::create(buffer);

        auto attachmentWriter = InProcessAttachmentWriter::create(sds);

        while (*is) {
            is->read(localBuffer.data(), mbBytes);
            size_t numBytesRead = is->gcount();
            AttachmentWriter::WriteStatus writeStatus = AttachmentWriter::WriteStatus::OK;
            attachmentWriter->write(localBuffer.data(), numBytesRead, &writeStatus);

            // write status should be either OK or CLOSED
            bool writeStatusOk =
                (AttachmentWriter::WriteStatus::OK == writeStatus ||
                 AttachmentWriter::WriteStatus::CLOSED == writeStatus);
            ASSERT_TRUE(writeStatusOk);
        }

        attachmentWriter->close();

        std::shared_ptr<InProcessAttachmentReader> attachmentReader =
            InProcessAttachmentReader::create(AttachmentReader::Policy::NON_BLOCKING, sds);
        ASSERT_NE(attachmentReader, nullptr);

        sendEvent(json, attachmentReader, expectedStatus, std::chrono::seconds(timeout));
    }

    /**
     * Waits for the ChannelObserverInterface##onFocusChanged() callback.
     *
     * @param timeout The amount of time to wait for the callback.
     * @param focusChanged An output parameter that notifies the caller whether a callback occurred.
     * @return Returns @c true if the callback occured within the timeout period and @c false otherwise.
     */
    FocusState waitForFocusChange(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_focusChanged.wait_for(lock, timeout, [this]() { return m_focusChangeOccurred; });

        return m_focusState;
    }

    void TearDown() override {
printf("Start to execute TearDown 1.......\n");
        stopMediaPlayer();
printf("Start to execute TearDown 2.......\n");
        exitSva = true;
        exitAVS = true;
        isAVSFinish = true;
        m_wisStateTrigger.notify_all();
printf("Start to execute TearDown 3.......\n");
        if (m_alertsAgent) {
          m_alertsAgent->onLocalStop();
printf("Start to execute TearDown 3 1.......\n");
          m_alertsAgent->removeAllAlerts();
printf("Start to execute TearDown 3 2.......\n");
          m_alertsAgent->shutdown();
        }
printf("Start to execute TearDown 3 3.......\n");
        if (m_certifiedSender) {
          m_certifiedSender->shutdown();
        }

printf("Start to execute TearDown 4.......\n");
        disconnect();
printf("Start to execute TearDown 5.......\n");
        if (m_contentMediaPlayer) {
          m_contentMediaPlayer->stop();
        }
        /*if (mWisAvs.m_audioPlayer) {
            mWisAvs.m_audioPlayer->shutdown();
        }*/
        //Bob end 20170818
printf("Start to execute TearDown 6.......\n");
        if (mWisAvs.m_AudioInputProcessor) {
          mWisAvs.m_AudioInputProcessor->shutdown();
        }
printf("Start to execute TearDown ７.......\n");
        if (m_speechSynthesizer) {
          m_speechSynthesizer->shutdown();
        }
printf("Start to execute TearDown ８.......\n");
        if(m_directiveSequencer) {
          m_directiveSequencer->shutdown();
        }
printf("Start to execute TearDown ９.......\n");
        if (m_avsConnectionManager) {
          m_avsConnectionManager->shutdown();
        }
printf("Start to execute TearDown 10.......\n");
        if (mWisAvs.m_playbackController) {
          mWisAvs.m_playbackController->shutdown();
        }
printf("Start to execute TearDown 11.......\n");
        AlexaClientSDKInit::uninitialize();

        if (wakeWordDetector == WAKE_WORD_DETECTOR_SVA) {
          std::unique_lock<std::mutex> lock(m_wisMutex);
          while (!m_wisStateTrigger.wait_for(lock, WAIT_FOR_TIMEOUT_DURATION, [this]() { return isSVAFinish;})) {
            std::cout << "AVS::TearDown wait for SVA exit------" << std::endl;
          }
          lock.unlock();
        }

        mWisCommon.sysLocalTime("**********  AVS_SDK\tStop test AVS  **********");  // King add
    }

    /**
     * Connect to AVS.
     */
    void connect() {
        ASSERT_TRUE(m_authObserver->waitFor(AuthObserver::State::REFRESHED)) << "Retrieving the auth token timed out.";
        // m_avsConnectionManager->enable();
        mWisAvs.m_connectionManager->enable(); // King update
        ASSERT_TRUE(m_connectionStatusObserver->waitFor(ConnectionStatusObserverInterface::Status::CONNECTED))
            << "Connecting timed out.";
    }

    /**
     * Disconnect from AVS.
     */
    void disconnect() {
        if (mWisAvs.m_connectionManager) {
          //m_avsConnectionManager->disable();
          mWisAvs.m_connectionManager->disable(); // King update
          ASSERT_TRUE(m_connectionStatusObserver->waitFor(ConnectionStatusObserverInterface::Status::DISCONNECTED))
                  << "Connecting timed out.";
        }
    }

    bool checkSentEventName(TestMessageSender::SendParams sendParams, std::string expectedName) {
        if (TestMessageSender::SendParams::Type::SEND == sendParams.type) {
            std::string eventString;
            std::string eventHeader;
            std::string eventName;
            jsonUtils::lookupStringValue(sendParams.request->getJsonContent(), JSON_MESSAGE_EVENT_KEY, &eventString);
            jsonUtils::lookupStringValue(eventString, JSON_MESSAGE_HEADER_KEY, &eventHeader);
            jsonUtils::lookupStringValue(eventHeader, JSON_MESSAGE_NAME_KEY, &eventName);
            return eventName == expectedName;
        }
        return false;
    }

    // King add
    static void* playPreRecSoundPrompt(void *ptr) {
      int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
      printf("playPreRecSoundPrompt thread: detachCode = %d\n", detachCode);

      mWisCommon.sysLocalTime("Start to play med_ui_voice_start_listening.wav");
      if(isPlayEnabled){
          if (mWisCommon.getCurrentSKU() == SKU_CX) {
            stopMediaPlayer();
            system((APP_ROOT_FOLDER + SHELL_SCRIPT_CX_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_PRE_RECORDING).c_str());
          } else {
            m_speakMediaPlayer->isTesting=false;  // King test
            if (wcdPlaySoundCueType == WCD_PLAY_SOUND_CUE_TYPE_MEDIA_PLAYER) {
              std::string LOCAL_AUDIO_FILE_PATH = APP_ROOT_FOLDER + "med_ui_voice_start_listening.wav";
              std::cout<<LOCAL_AUDIO_FILE_PATH<<std::endl;
              m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);

              if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
                if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
                  while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
                    ;
                  }
                  m_localMediaPlayer.m_mediaPlayer->stop();
                }
              }
            } else {
              system((APP_ROOT_FOLDER + SHELL_SCRIPT_WCD_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_PRE_RECORDING).c_str());
            }
          }
      }
      mWisCommon.sysLocalTime("End play med_ui_voice_start_listening.wav");

      pthread_exit(0);
      return 0;
    }

    void preRecording() {
      if (enableRecLEDPrompt) {
        mWisCommon.lightLED(LED_TYPE_LISTENING);
      }

      if (!isExpectDirectStatus && enablePreRecSoundPrompt) {
        pthread_t playPreRecSoundPromptThread;
        pthread_create(&playPreRecSoundPromptThread, NULL, &playPreRecSoundPrompt, (void*) this);
      }

      printf("\n\n\n##### Start recording!\n\n\n");
    }

    void postRecording() {
      if (wakeWordDetector == WAKE_WORD_DETECTOR_SVA) {
        if (enableRecLEDPrompt) {
          mWisCommon.lightLED(LED_TYPE_THINKING);
        }
      }

      if (enablePostRecSoundPrompt) {
        mWisCommon.sysLocalTime("Start to play med_ui_voice_stop_listening.wav");
        if(isPlayEnabled){
            if (mWisCommon.getCurrentSKU() == SKU_CX) {
              stopMediaPlayer();
              system((APP_ROOT_FOLDER + SHELL_SCRIPT_CX_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_POST_RECORDING).c_str());
            } else {
              if (access(SOUND_CUE_TEST_FILE_PATH.c_str(),F_OK) != -1) { // File exist
                std::string delayInMillSeconds = mWisCommon.getValueWithKey(SOUND_CUE_TEST_FILE_PATH, SOUND_CUE_TEST_KEY_DELAY_IN_MILL_SECONDS);
                m_speakMediaPlayer->sleepInMillSeconds = atoi(delayInMillSeconds.c_str());
                m_speakMediaPlayer->isTesting=true; // King test: [Sound cue]
              }
              if (wcdPlaySoundCueType == WCD_PLAY_SOUND_CUE_TYPE_MEDIA_PLAYER) {
                std::string LOCAL_AUDIO_FILE_PATH = APP_ROOT_FOLDER + "med_ui_voice_stop_listening.wav";
                std::cout<<LOCAL_AUDIO_FILE_PATH<<std::endl;
                m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);

                if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
                    if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
                      while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
                        ;
                      }
                      m_localMediaPlayer.m_mediaPlayer->stop();
                    }
                }
              }else{
                system((APP_ROOT_FOLDER + SHELL_SCRIPT_WCD_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_POST_RECORDING).c_str());
              }
            }
        }
        mWisCommon.sysLocalTime("End play med_ui_voice_stop_listening.wav");
      }

      printf("\n\n\n##### End recording!\n\n\n");
    }

    void checkSpeechSynthesizerState(){
      if (m_speechSynthesizerObserver->getCurrentState() == SpeechSynthesizerObserver::SpeechSynthesizerState::FINISHED) {
        printf("\n\nKing --> %s\n\n", "checkSpeechSynthesizerState onSpeechSynthesizerObserverStateChanged to FINISHED");
        m_wisStatusChanged->onSpeechSynthesizerObserverStateChanged(SpeechSynthesizerObserver::SpeechSynthesizerState::FINISHED);
      }
    }

    // King add
    void handleOneSpeech(){
      mWisCommon.sysLocalTime("Start execute handleOneSpeech()");

      // SpeechSynthesizerObserver defaults to a FINISHED state.
      //ASSERT_EQ(m_speechSynthesizerObserver->waitForNext(
      //        WAIT_FOR_TIMEOUT_DURATION), SpeechSynthesizerObserver::SpeechSynthesizerState::FINISHED);

      // Send audio of "Joke" that will prompt SetMute and Speak.
      //m_directiveSequencer->setDialogRequestId(FIRST_DIALOG_REQUEST_ID);

      // Check that AIP is in an IDLE state before starting.
      //ASSERT_TRUE(m_StateObserver->checkState(AudioInputProcessor::State::IDLE, AUDIO_FILE_TIMEOUT_DURATION));

      // Request the alarm channel for the test channel client.
      //ASSERT_TRUE(m_focusManager->acquireChannel(FocusManager::ALERTS_CHANNEL_NAME, m_testClient, ALARM_ACTIVITY_ID));
      //ASSERT_EQ(m_testClient->waitForFocusChange(AUDIO_FILE_TIMEOUT_DURATION), FocusState::FOREGROUND);

      int MAX_CYCLE = 1;
      int testCycleIndex = 0;
      while(testCycleIndex < MAX_CYCLE){
        isPass = false;

        if(!isExpectDirectStatus){
          // Signal to the AIP to start recognizing.
          mWisAvs.m_AudioInputProcessor->resetState();
          ASSERT_TRUE(m_tapToTalkButton->startRecognizing(mWisAvs.m_AudioInputProcessor, m_TapToTalkAudioProvider));
        }
       // Check that AIP is now in RECOGNIZING state.
       //ASSERT_TRUE(m_StateObserver->checkState(AudioInputProcessor::State::RECOGNIZING, AUDIO_FILE_TIMEOUT_DURATION));
       std::string m_RecordingFilePath;
       if (recordingSource == REC_SOURCE_HAL_REC_TEST) {  // scenario 1: hal_rec_test data
             printf("**********  start hal_rec_test recording *********\n");
             m_RecordingFilePath = STORE_ROOT_FOLDER + "hal_rec_test_rec.wav";
             preRecording();

             // King start record
             m_wisStatusChanged->isStopCaptureFlag=false;
             WisHalRec mWisHalRec;
             mWisHalRec.initialRecorder();

             fprintf(mWisHalRec.log_file, "\n start first record thread \n");
             int rc = 0, ret = 0;
             time_t start_time = time(0);
             double time_elapsed = 0;
             char param[100] = "audio_stream_profile=";
             int data_sz = 0;
             qahw_in_buffer_t in_buf;

             // Open audio input stream
             qahw_stream_handle_t* in_handle = NULL;

             mWisCommon.sysLocalTime("qahw_open_input_stream......");
             rc = qahw_open_input_stream(mWisHalRec.qahw_mod_handle,
                                         mWisHalRec.params.handle, mWisHalRec.params.input_device,
                                         &mWisHalRec.params.config, &in_handle,
                                         mWisHalRec.params.flags, "input_stream",
                                         mWisHalRec.params.source);
             if (isForVickLogEnable) {
               printf("-----> For Vick: qahw_open_input_stream...\n");
             }
             if (rc) {
                 fprintf(mWisHalRec.log_file, "ERROR :::: Could not open input stream, handle(%d)\n", mWisHalRec.params.handle);
                 if (mWisHalRec.log_file != stdout)
                     fprintf(stdout, "ERROR :::: Could not open input stream, handle(%d)\n", mWisHalRec.params.handle);
             }

             // Get buffer size to get upper bound on data to read from the HAL
             size_t buffer_size = qahw_in_get_buffer_size(in_handle);
             char *buffer = (char *)calloc(1, buffer_size);
             size_t written_size;
             if (buffer == NULL) {
                 fprintf(mWisHalRec.log_file, "calloc failed!!, handle(%d)\n", mWisHalRec.params.handle);
                 if (mWisHalRec.log_file != stdout)
                     fprintf(stdout, "calloc failed!!, handle(%d)\n", mWisHalRec.params.handle);
             }

             fprintf(mWisHalRec.log_file, " input opened, buffer  %p, size %zu, handle(%d)", buffer, buffer_size, mWisHalRec.params.handle);
             // set profile for the recording session
             //strlcat(param, params.profile, sizeof(param));
             qahw_in_set_parameters(in_handle, param);
             if (isForVickLogEnable) {
               printf("-----> For Vick: qahw_in_set_parameters...\n");
             }

             fprintf(mWisHalRec.log_file, "\n Please speak into the microphone for %d seconds, handle(%d)\n", recordingTimeout, mWisHalRec.params.handle);
             if (mWisHalRec.log_file != stdout)
                 fprintf(stdout, "\n Please speak into the microphone for %d seconds, handle(%d)\n", recordingTimeout, mWisHalRec.params.handle);

             FILE *fd = fopen(m_RecordingFilePath.c_str(),"w");
             if (fd == NULL) {
                 fprintf(mWisHalRec.log_file, "File open failed \n");
                 if (mWisHalRec.log_file != stdout)
                     fprintf(stdout, "File open failed \n");
             }
             int bps = 16;

             struct wav_header hdr;
             hdr.riff_id = ID_RIFF;
             hdr.riff_sz = 0;
             hdr.riff_fmt = ID_WAVE;
             hdr.fmt_id = ID_FMT;
             hdr.fmt_sz = 16;
             hdr.audio_format = FORMAT_PCM;
             hdr.num_channels = mWisHalRec.params.channels;
             hdr.sample_rate = mWisHalRec.params.config.sample_rate;
             hdr.byte_rate = hdr.sample_rate * hdr.num_channels * (bps/8);
             hdr.block_align = hdr.num_channels * (bps/8);
             hdr.bits_per_sample = bps;
             hdr.data_id = ID_DATA;
             hdr.data_sz = 0;
             fwrite(&hdr, 1, sizeof(hdr), fd);

             memset(&in_buf,0, sizeof(qahw_in_buffer_t));
             start_time = time(0);
             std::vector<int16_t> retVal(buffer_size/2, 0);
             mWisCommon.sysLocalTime("Start Recording!!!!!!!   Start Recording!!!!!!!   Start Recording!!!!!!!");
             isRecordingFinish = false;
             while(1) {
                 if(time_elapsed < mWisHalRec.params.record_delay) {
                     usleep(1000000*(mWisHalRec.params.record_delay - time_elapsed));
                     time_elapsed = difftime(time(0), start_time);
                     continue;
                 } else if (time_elapsed > mWisHalRec.params.record_delay + recordingTimeout/*mWisHalRec.params.record_length*/) {
                     fprintf(mWisHalRec.log_file, "\n Test for session with handle(%d) completed.\n", mWisHalRec.params.handle);
                     if (mWisHalRec.log_file != stdout)
                         fprintf(stdout, "\n Test for session with handle(%d) completed.\n", mWisHalRec.params.handle);
                     break;
                 }

                 //in_buf.buffer = buffer;
                 in_buf.buffer = (char *)&retVal[0];
                 in_buf.bytes = buffer_size;
                 qahw_in_read(in_handle, &in_buf);

                 // To detect if end of recording
                 if (m_wisStatusChanged->isStopCaptureFlag) {
                   break;
                 }

                 time_elapsed = difftime(time(0), start_time);
                 written_size = fwrite(in_buf.buffer, 1, buffer_size, fd);
                 if (written_size < buffer_size) {
                    printf("Error in fwrite(%d)=%s\n",ferror(fd), strerror(ferror(fd)));
                    break;
                 }
                 m_AudioBufferWriter->write(retVal.data(), buffer_size/2);
                 retVal.clear();

                 // print recording time
                 /*fprintf(mWisHalRec.log_file, "recording data size: %d.\n", written_size);
                 mWisCommon.sysLocalTime("Recording test time");*/

                 data_sz += buffer_size;
             }
             isRecordingFinish = true;
             mWisCommon.sysLocalTime("End Recording!!!!!!!");

             isExpectDirectStatus = false;

             // update lengths in header
             hdr.data_sz = data_sz;
             hdr.riff_sz = data_sz + 44 - 8;
             fseek(fd, 0, SEEK_SET);
             fwrite(&hdr, 1, sizeof(hdr), fd);
             fclose(fd);

             fprintf(mWisHalRec.log_file, " closing input, handle(%d)", mWisHalRec.params.handle);
             printf("closing input");

             // Close input stream and device.
             rc = qahw_in_standby(in_handle);
             if (isForVickLogEnable) {
               printf("-----> For Vick: qahw_in_standby...\n");
             }
             if (rc) {
                 fprintf(mWisHalRec.log_file, "out standby failed %d, handle(%d)\n",rc, mWisHalRec.params.handle);
                 if (mWisHalRec.log_file != stdout)
                     fprintf(stdout, "out standby failed %d, handle(%d)\n",rc, mWisHalRec.params.handle);
             }

             rc = qahw_close_input_stream(in_handle);
             if (isForVickLogEnable) {
               printf("-----> For Vick: qahw_close_input_stream...\n");
             }
             if (rc) {
                 fprintf(mWisHalRec.log_file, "could not close input stream %d, handle(%d)\n",rc, mWisHalRec.params.handle);
                 if (mWisHalRec.log_file != stdout)
                     fprintf(stdout, "could not close input stream %d, handle(%d)\n",rc, mWisHalRec.params.handle);
             }

             // Print instructions to access the file.
             fprintf(mWisHalRec.log_file, "\n\n The audio recording has been saved to %s. Please use adb pull to get "
                    "the file and play it using audacity. The audio data has the "
                    "following characteristics:\n Sample rate: %i\n Format: %d\n "
                    "Num channels: %i, handle(%d)\n\n",
                    m_RecordingFilePath.c_str(), mWisHalRec.params.config.sample_rate, mWisHalRec.params.config.format, mWisHalRec.params.channels, mWisHalRec.params.handle);
             if (mWisHalRec.log_file != stdout)
                 fprintf(stdout, "\n\n The audio recording has been saved to %s. Please use adb pull to get "
                    "the file and play it using audacity. The audio data has the "
                    "following characteristics:\n Sample rate: %i\n Format: %d\n "
                    "Num channels: %i, handle(%d)\n\n",
                    m_RecordingFilePath.c_str(), mWisHalRec.params.config.sample_rate, mWisHalRec.params.config.format, mWisHalRec.params.channels, mWisHalRec.params.handle);

              ret = qahw_unload_module(mWisHalRec.qahw_mod_handle);
              if (isForVickLogEnable) {
                printf("-----> For Vick: qahw_unload_module...\n");
              }
               if (ret) {
                   fprintf(mWisHalRec.log_file, "could not unload hal %d \n",ret);
               }

               fprintf(mWisHalRec.log_file, "\n Done with hal record test \n");

               postRecording();
               // King end
       } else if(recordingSource == REC_SOURCE_SVA_LAB_DATA){  // scenario 2: SVA LAB data
         printf("**********  start SVA LAB data recording *********\n");
         isSVALABRecording = true;
         m_wisStatusChanged->isStopCaptureFlag=false;

         m_RecordingFilePath = STORE_ROOT_FOLDER + "sva_lab_data_rec.wav";
         preRecording();

         unsigned char *buffer;
         size_t bytes, written;
         size_t cur_bytes_read = 0;
         size_t total_bytes_to_read;
         struct sound_trigger_phrase_recognition_event phrase_event = qsthw_event_cache->phrase_event;
         sound_model_handle_t sm_handle = phrase_event.common.model;
         audio_config_t *audio_config = &phrase_event.common.audio_config;
         uint32_t sample_rate = audio_config->sample_rate;
         uint32_t channels =
                  audio_channel_count_from_in_mask(audio_config->channel_mask);
         audio_format_t format = audio_config->format;
         size_t samp_sz = audio_bytes_per_sample(format);

         bytes = qsthw_get_buffer_size(st_mod_handle, sm_handle);
         if (bytes <= 0) {
             printf("Invalid buffer size returned\n");
             return;
         }

         /* total bytes to read = bytes to read per sec * duration where
          * audio configuration passed as part of recognition event is
          * used to obtain bytes per sec.
          */
         total_bytes_to_read = ((sample_rate * channels * samp_sz) * total_duration_ms)/1000;
         printf("sample rate %d, channels %d, samp sz %d, duration %d\n, total_bytes_to_read %d",
         sample_rate, channels, samp_sz, total_duration_ms, total_bytes_to_read);

         buffer = (unsigned char *)calloc(1, bytes);
         if (buffer == NULL) {
             printf("Could not allocate memory for capture buffer\n");
             return;
         }

         FILE *fd = fopen(m_RecordingFilePath.c_str(),"w");
         int bps = 16;

         struct wav_header hdr;
         hdr.riff_id = ID_RIFF;
         hdr.riff_sz = 0;
         hdr.riff_fmt = ID_WAVE;
         hdr.fmt_id = ID_FMT;
         hdr.fmt_sz = 16;
         hdr.audio_format = FORMAT_PCM;
         hdr.num_channels = 1;
         hdr.sample_rate = 16000;
         hdr.byte_rate = hdr.sample_rate * hdr.num_channels * (bps/8);
         hdr.block_align = hdr.num_channels * (bps/8);
         hdr.bits_per_sample = bps;
         hdr.data_id = ID_DATA;
         hdr.data_sz = 0;
         fwrite(&hdr, 1, sizeof(hdr), fd);

         printf("lab capture file : %s\n", m_RecordingFilePath.c_str());

         std::vector<int16_t> retVal(bytes/2, 0);
         mWisCommon.sysLocalTime("Start Recording!!!!!!!   Start Recording!!!!!!!   Start Recording!!!!!!!");
         printf("%d - %d - %d\n",cur_bytes_read, total_bytes_to_read, !m_wisStatusChanged->isStopCaptureFlag);
         isRecordingFinish = false;
         while (cur_bytes_read < total_bytes_to_read && !m_wisStatusChanged->isStopCaptureFlag) {
             //qsthw_read_buffer(st_mod_handle, sm_handle,buffer, bytes);
             //written = fwrite(buffer, 1, bytes, fd);
             qsthw_read_buffer(st_mod_handle, sm_handle, (unsigned char*)&retVal[0], bytes);
             // To detect if end of recording
             if (m_wisStatusChanged->isStopCaptureFlag) {
               break;
             }
             written = fwrite((char *)&retVal[0], 1, bytes, fd);
             if (written != bytes) {
                 printf("written %d, bytes %d\n", written, bytes);
                 if (ferror(fd)) {
                     printf("Error writing lab capture data into file %s\n",strerror(errno));
                     break;
                 }
             }
             m_AudioBufferWriter->write(retVal.data(), bytes/2);
             retVal.clear();

             // print recording time
             /*printf("recording data size: %d.\n", written);
             mWisCommon.sysLocalTime("Recording test time");*/

             cur_bytes_read += bytes;
             memset(buffer, 0, bytes);
         }
         isRecordingFinish = true;
         isExpectDirectStatus = false;

         // update lengths in header
         hdr.data_sz = cur_bytes_read;
         hdr.riff_sz = cur_bytes_read + 44 - 8;
         fseek(fd, 0, SEEK_SET);
         fwrite(&hdr, 1, sizeof(hdr), fd);
         fclose(fd);

         printf("bytes to read %d, bytes read %d\n", total_bytes_to_read, cur_bytes_read);
         qsthw_stop_buffering(st_mod_handle, sm_handle);

         mWisCommon.sysLocalTime("End Recording!!!!!!!");

         postRecording();
         isSVALABRecording = false;
       }
       else {  // post a audio to AVS
         m_RecordingFilePath = STORE_ROOT_FOLDER + "hal_rec_test_rec.wav";
         std::string file = APP_ROOT_FOLDER + "inputs/king_hal_rec_test_rec.wav";
         std::cout << "file: " << file << std::endl;
         const int RIFF_HEADER_SIZE = 44;
         std::ifstream inputFile(file.c_str(), std::ifstream::binary);
         if (!inputFile.good()) {
           std::cout << "Couldn't open audio file!" << std::endl;
           return ;
         }
         std::cout << "open audio file success!" << std::endl;
         inputFile.seekg(0, std::ios::end);
         int fileLengthInBytes = inputFile.tellg();
         if (fileLengthInBytes <= RIFF_HEADER_SIZE) {
           std::cout << "File should be larger than 44 bytes, which is the size of the RIFF header" << std::endl;
           return ;
         }

         inputFile.seekg(RIFF_HEADER_SIZE, std::ios::beg);
         std::cout << "\t\tfile lenght In bytes: " << fileLengthInBytes << std::endl;
         int numSamples = (fileLengthInBytes - RIFF_HEADER_SIZE) / 5;
         std::cout << "\t\tnumSamples: " << numSamples << std::endl;
         std::vector<int16_t> retVal(115200, 0);

         int index = 0;
         while(!inputFile.eof()){
           index ++;
           inputFile.read((char *)&retVal[0], 115200);
           m_AudioBufferWriter->write(retVal.data(), inputFile.gcount());
           retVal.clear();
           std::cout << "index= " << index << std::endl;
         }
         inputFile.close();
       }

         totalAVSRequestNum ++;
         //wait for receive directive
         mWisCommon.sysLocalTime("---> Get Directive start!");
         m_wisStatusChanged->isNewDirectiveArrived=false;
         m_wisStatusChanged->m_avsDirectives.clear();
         std::unique_lock<std::mutex> lock(m_wisMutex);
         std::chrono::milliseconds WAIT_FOR_DIRECTIVE_TIMEOUT(waitDirectiveTimeout);
         if (!m_wisStateTrigger.wait_for(lock, WAIT_FOR_DIRECTIVE_TIMEOUT, [this]() { return m_wisStatusChanged->isNewDirectiveArrived;})) {
             std::cout << "Waiting for directive timeout ------" << std::endl;
         }
         lock.unlock();
         usleep(50*1000);  // micro_seconds
         mWisCommon.sysLocalTime("---> Get Directive end!");

         size_t m_directiveSize = m_wisStatusChanged->m_avsDirectives.size();
         for (size_t i =0; i < m_directiveSize; i ++) {
             isPass = false;
             std::shared_ptr<AVSDirective> currentDirective = m_wisStatusChanged->m_avsDirectives[i];
             printf("--> To handle the %d directive: %s - %s\n", i, currentDirective->getNamespace().c_str(), currentDirective->getName().c_str());

             if (currentDirective->getNamespace() == "SpeechRecognizer" &&
                    (currentDirective->getName() == "ExpectSpeech" || currentDirective->getName() == "StopCapture")) {
               printf("ignore %s directive...\n", currentDirective->getName().c_str());
               continue;
             }

             if (currentDirective) {
               if(currentDirective->getNamespace() == "Speaker"){ //Speaker Interface
                 std::string directiveName = currentDirective->getName();
                 mWisCommon.sysLocalTime(("namespace value: "+ currentDirective->getNamespace()).c_str());
                 mWisCommon.sysLocalTime(("name value: "+ currentDirective->getName()).c_str());
                 mWisCommon.sysLocalTime(("payload value: "+ currentDirective->getPayload()).c_str());
                 mWisCommon.sysLocalTime(("messageId value: "+ currentDirective->getMessageId()).c_str());

                 if(directiveName == "SetVolume"){ //Speaker Interface SetVolume
                    mWisVolumeControl.setVolume(currentDirective->getPayload());
                 }else if(directiveName == "AdjustVolume"){ //Speaker Interface AdjustVolume
                    mWisVolumeControl.adjustVolume(currentDirective->getPayload());
                 }else if(directiveName == "SetMute"){ //Speaker Interface SetMute
                    mWisVolumeControl.setMuteOrUnmute(currentDirective->getPayload());
                 }

                 //sleep(1);

                 int current_volume = getCurrentMediaVolume();

                 std::string isMuted;
                 if(current_volume > 0){
                   isMuted = "0";
                 }else {
                   isMuted = "1";
                 }

                 std::string volumeEvent;
                 if(directiveName == "SetVolume" || directiveName == "AdjustVolume"){
                   volumeEvent = mWisCommon.replace_all(volumeChangedEvent,"current_volume",std::to_string((int)(current_volume*100.0/VOLUME_LEVEL)));
                   volumeEvent = mWisCommon.replace_all(volumeEvent,"isMuted",isMuted);
                 }else if(directiveName == "SetMute"){
                   volumeEvent = mWisCommon.replace_all(muteChangedEvent,"current_volume",std::to_string((int)(current_volume*100.0/VOLUME_LEVEL)));
                   volumeEvent = mWisCommon.replace_all(volumeEvent,"isMuted",isMuted);
                 }
                 std::string messageId = avsCommon::utils::uuidGeneration::generateUUID();
                 volumeEvent = mWisCommon.replace_all(volumeEvent,"messageId123",messageId);
                 mWisCommon.sysLocalTime(("Send volume event: "+volumeEvent).c_str());

                 auto messageRequest = std::make_shared<ObservableMessageRequest>(volumeEvent, nullptr);
                //  m_avsConnectionManager->sendMessage(messageRequest);
                 mWisAvs.m_connectionManager->sendMessage(messageRequest);
                 sendCurrentMediaVolume();
                //  ASSERT_TRUE(messageRequest->waitFor(avsCommon::avs::MessageRequest::Status::SUCCESS, //std::chrono::seconds(SEND_EVENT_TIMEOUT_DURATION)));

                 /*if(messageRequest->waitFor(avsCommon::avs::MessageRequest::Status::SUCCESS, std::chrono::seconds(SEND_EVENT_TIMEOUT_DURATION))){
                   std::cout << "messageRequest is true" << '\n';
                   isPass = true;
                 }else {
                   std::cout << "messageRequest is false" << '\n';
                 }

                if (!isPass) {
                  failAVSRequestNum ++;
                  mWisCommon.backupAVSFailedAudio(m_RecordingFilePath, totalAVSRequestNum, failAVSRequestNum);
                }*/
               } else if(currentDirective->getNamespace() == "SpeechSynthesizer" // SpeechSynthesizer interface
                           || currentDirective->getNamespace() == "SpeechRecognizer") { //SpeechRecognizer Interface
                 #ifdef GSTREAMER_MEDIA_PLAYER
                 mWisCommon.sysLocalTime("Wait for get response audio!!!!!!!");
                 // King debug
                 //TestMessageSender::SendParams sendRecognizeParams = m_avsConnectionManager->waitForNext(DIRECTIVE_TIMEOUT_DURATION);
                 //ASSERT_TRUE(checkSentEventName(sendRecognizeParams, NAME_RECOGNIZE));

                 // Wait for the directive to route through to our handler.
                 /*TestDirectiveHandler::DirectiveParams params = m_directiveHandler->waitForNext(WAIT_FOR_TIMEOUT_DURATION);
                 ASSERT_EQ(params.type, TestDirectiveHandler::DirectiveParams::Type::PREHANDLE);
                 params = m_directiveHandler->waitForNext(WAIT_FOR_TIMEOUT_DURATION);
                 ASSERT_EQ(params.type, TestDirectiveHandler::DirectiveParams::Type::HANDLE);

                 // Unblock the queue so SpeechSynthesizer can do its work.
                 params.result->setCompleted();*/


                 /*TestDirectiveHandler::DirectiveParams mDirectiveParams = m_directiveHandler->waitForNext(DIRECTIVE_TIMEOUT_DURATION);
                   while (mDirectiveParams.type != TestDirectiveHandler::DirectiveParams::Type::TIMEOUT &&
                               (!mDirectiveParams.isPreHandle() || mDirectiveParams.directive->getName() != NAME_SPEAK)) {
                     std::cout << "+++++++++++++++++++++++++++++" << '\n';
                       if (mDirectiveParams.isHandle() && mDirectiveParams.directive->getName() == NAME_SPEAK) {
                               mDirectiveParams.result->setCompleted();
                       }
                       mDirectiveParams = m_directiveHandler->waitForNext(DIRECTIVE_TIMEOUT_DURATION);
                   }*/


                 // SpeechSynthesizer is now playing.
                 //ASSERT_EQ(m_speechSynthesizerObserver->waitForNext(WAIT_FOR_TIMEOUT_DURATION), SpeechSynthesizerObserver::SpeechSynthesizerState::PLAYING);
                 std::chrono::seconds avsResponseTimeout(waitForResponseTimeout);
                 if (m_speechSynthesizerObserver->getCurrentState() == SpeechSynthesizerObserver::SpeechSynthesizerState::PLAYING
                        || m_speechSynthesizerObserver->waitForNext(avsResponseTimeout, true) == SpeechSynthesizerObserver::SpeechSynthesizerState::PLAYING) {
                     mWisCommon.sysLocalTime("Playing AVS response audio via gstreamer.....");

                     mWisCommon.lightLED(LED_TYPE_SPEAKING);

                     isAVSPlayingResponse = true;
                     isLaunchAVSKeyPressed = false; //Bob add for TAP key pressed 20170907

                     //Check that SS grabs the channel focus by seeing that the test client has been backgrounded.
                     //ASSERT_EQ(m_testClient->waitForFocusChange(WAIT_FOR_TIMEOUT_DURATION), FocusState::BACKGROUND);

                     // King debug
                     // SpeechStarted was sent.
                     //TestMessageSender::SendParams sendStartedParams = m_avsConnectionManager->waitForNext(DIRECTIVE_TIMEOUT_DURATION);
                     //ASSERT_TRUE(checkSentEventName(sendStartedParams, NAME_SPEECH_STARTED));

                     // Media Player has finished.
                     //ASSERT_EQ(m_speechSynthesizerObserver->waitForNext(WAIT_FOR_TIMEOUT_DURATION), SpeechSynthesizerObserver::SpeechSynthesizerState::FINISHED);
                     /*while (m_speechSynthesizerObserver->waitForNext(WAIT_FOR_TIMEOUT_DURATION) != SpeechSynthesizerObserver::SpeechSynthesizerState::FINISHED) {
                         printf("Waiting for Mediaplayer finish.................... isSVADetected = %d, currentSpeechSynthesizerState: %d\n", isSVADetected, m_speechSynthesizerObserver->getCurrentState());
                         if (isSVADetected) {
                           m_speakMediaPlayer->stop();
                           printf("break m_speechSynthesizerObserver->waitForNext......\n");
                           break;
                         }*/
                         // King debug
                         // SpeechFinished is sent here.
                         //TestMessageSender::SendParams sendFinishedParams = m_avsConnectionManager->waitForNext(WAIT_FOR_TIMEOUT_DURATION);
                         //ASSERT_TRUE(checkSentEventName(sendFinishedParams, NAME_SPEECH_FINISHED));

                         // Alerts channel regains the foreground.
                         //ASSERT_EQ(m_testClient->waitForFocusChange(WAIT_FOR_TIMEOUT_DURATION), FocusState::FOREGROUND);
                     //}
                     printf("\n\nWaiting for Mediaplayer finish1.................... isSVADetected = %d, currentSpeechSynthesizerState: %d\n", isSVADetected, m_speechSynthesizerObserver->getCurrentState());
                     while (!m_speechSynthesizerObserver->checkState(SpeechSynthesizerObserver::SpeechSynthesizerState::FINISHED, WAIT_FOR_TIMEOUT_DURATION)) {
                        std::cout << "handleOneSpeech::waitForSpeechFinished------" << std::endl;
                     }
                     m_speakMediaPlayer->stop();
                     /*while(isSVADetected != 1 && m_speechSynthesizerObserver->getCurrentState() != SpeechSynthesizerObserver::SpeechSynthesizerState::FINISHED){
                       sleep(0.2);
                     }
                     printf("\n\nWaiting for Mediaplayer finish2.................... isSVADetected = %d, currentSpeechSynthesizerState: %d\n", isSVADetected, m_speechSynthesizerObserver->getCurrentState());
                     m_speakMediaPlayer->stop();
                     m_speechSynthesizerObserver->waitForNext(WAIT_FOR_TIMEOUT_DURATION);*/
                     printf("\n\nWaiting for Mediaplayer finish3.................... isSVADetected = %d, currentSpeechSynthesizerState: %d\n", isSVADetected, m_speechSynthesizerObserver->getCurrentState());

                     isPass = true;
                     printf("Media Player has finished....................\n");
                 }

                 if (!isPass) {
                   failAVSRequestNum ++;
                   mWisCommon.backupAVSFailedAudio(m_RecordingFilePath, totalAVSRequestNum, failAVSRequestNum);
                 }

                 /*if(!isPass){
                   std::string LOCAL_AUDIO_FILE_PATH = APP_ROOT_FOLDER + "failed.mp3";
                   std::cout<<"LOCAL_AUDIO_FILE_PATH= " << LOCAL_AUDIO_FILE_PATH <<std::endl;
                   m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);

                   if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
                     if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
                       while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
                         ;
                       }
                       m_localMediaPlayer.m_mediaPlayer->stop();
                     }
                   }

                   printf("Failed audio has finished....................\n");
                 }*/
             # else
                 // The test channel client has been notified the alarm channel has been backgrounded.
                 /*ASSERT_EQ(m_testClient->waitForFocusChange(AUDIO_FILE_TIMEOUT_DURATION), FocusState::BACKGROUND);

                 // Check that AIP is in BUSY state.
                 ASSERT_TRUE(m_StateObserver->checkState(
                         AudioInputProcessorObserverInterface::State::BUSY, AUDIO_FILE_TIMEOUT_DURATION));

                 // Check that AIP is in an IDLE state.
                 ASSERT_TRUE(m_StateObserver->checkState(
                         AudioInputProcessorObserverInterface::State::IDLE, AUDIO_FILE_TIMEOUT_DURATION));

                 // Check that the test context provider was asked to provide context for the event.
                 ASSERT_TRUE(m_stateProvider->checkStateRequested());

                 // The test channel client has been notified the alarm channel has been foregrounded.
                 ASSERT_EQ(m_testClient->waitForFocusChange(AUDIO_FILE_TIMEOUT_DURATION), FocusState::FOREGROUND);

                 // Check that a recognize event was sent.
                 ASSERT_TRUE(checkSentEventName(m_avsConnectionManager, NAME_RECOGNIZE));*/


                 // decode result
                 std::cout << "wait for next directive handler" << '\n';
                 mWisCommon.sysLocalTime("Wait for get response audio!!!!!!!");

                 // Check that prehandle and handle for setMute and Speak has reached the test SS.
                 TestDirectiveHandler::DirectiveParams mDirectiveParams = m_directiveHandler->waitForNext(DIRECTIVE_TIMEOUT_DURATION);
                 if(mDirectiveParams.type != TestDirectiveHandler::DirectiveParams::Type::TIMEOUT) {
                   while (mDirectiveParams.type != TestDirectiveHandler::DirectiveParams::Type::TIMEOUT &&
                               (!mDirectiveParams.isPreHandle() || mDirectiveParams.directive->getName() != NAME_SPEAK)) {
                     std::cout << "+++++++++++++++++++++++++++++" << '\n';
                       if (mDirectiveParams.isHandle() && mDirectiveParams.directive->getName() == NAME_SPEAK) {
                               mDirectiveParams.result->setCompleted();
                       }
                       mDirectiveParams = m_directiveHandler->waitForNext(DIRECTIVE_TIMEOUT_DURATION);
                   }

                   auto directive = mDirectiveParams.directive;
                   if(directive != NULL){
                       std::string payloadUrl;
                       jsonUtils::lookupStringValue(directive->getPayload(), "url", &payloadUrl);
                       ASSERT_TRUE(!payloadUrl.empty());

                       auto stringIndex = payloadUrl.find(":");
                       ASSERT_TRUE(stringIndex != std::string::npos);
                       ASSERT_TRUE(stringIndex != payloadUrl.size() - 1);

                       auto contentId = payloadUrl.substr(payloadUrl.find(':') + 1);
                       auto attachmentReader = directive->getAttachmentReader(contentId, AttachmentReader::Policy::BLOCKING);
                       mWisCommon.sysLocalTime("Save response audio Start!!!!!!!");
                       std::ofstream out(STORE_ROOT_FOLDER + "result.mp3");
                       if (out.is_open()){
                         char buf[1024];
                         avsCommon::avs::attachment::AttachmentReader::ReadStatus status;
                         mWisCommon.sysLocalTime("Saving!!!!!!!");
                         while(true){
                           auto len = attachmentReader->read(buf,sizeof(buf),&status);
                           mWisCommon.sysLocalTime("Saving!!!!!!! status = ");
                           if(status == avsCommon::avs::attachment::AttachmentReader::ReadStatus::OK){
                             out.write(buf,len);
                           }else if(status == avsCommon::avs::attachment::AttachmentReader::ReadStatus::CLOSED){
                             break;
                           }
                         }
                         attachmentReader->close();
                       }
                       ASSERT_NE(attachmentReader, nullptr);
                       out.close();
                       mWisCommon.sysLocalTime("Save response audio End!!!!!!!  start to play result.mp3");
                       isPass = true;
                   }
                 }

                 //std::thread t1(&AlexaAudioTest::reConnect, this);
                 if (isPlayEnabled) {
                   if (isPass) {
                       system("gst-play-1.0 " + STORE_ROOT_FOLDER + "result.mp3");
                   }else{
                       system("gst-play-1.0 " + APP_ROOT_FOLDER + "failed.mp3");
                   }
                 }
             # endif
             }
           }
       }

       isAVSPlayingResponse = true;
       isLaunchAVSKeyPressed = false; //Bob add for TAP key pressed 20170907

       checkSpeechSynthesizerState();

       testCycleIndex ++;
       std::cout << "^_^^^_^^^^_^^^^^^_^^^^^^_^^^^^^^^_^^^^ testCycleIndex: " << testCycleIndex << '\n';
     }
    }

    // King Start
    static void* produce_Recording(void *ptr) {
        int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
        printf("produce_Recording thread: detachCode = %d\n", detachCode);

        Nut2AVSTest *mThis = (Nut2AVSTest *)ptr;
        RingBufferProducer* p_producer = (RingBufferProducer *) mThis->p_producer1;  /* type cast to a pointer to thdata */

        printf("**********  start hal_rec_test recording *********\n");
        std::string m_RecordingFilePath = STORE_ROOT_FOLDER + "hal_rec_test_rec_original.wav";

        // King start record
        WisHalRec mWisHalRec;
        mWisHalRec.initialRecorder();

        fprintf(mWisHalRec.log_file, "\n start first record thread \n");
        int rc = 0, ret = 0;
        time_t start_time = time(0);
        double time_elapsed = 0;
        char param[100] = "audio_stream_profile=";
        qahw_in_buffer_t in_buf;

        // Open audio input stream
        qahw_stream_handle_t* in_handle = NULL;

        mWisCommon.sysLocalTime("qahw_open_input_stream......");
        rc = qahw_open_input_stream(mWisHalRec.qahw_mod_handle,
                                    mWisHalRec.params.handle, mWisHalRec.params.input_device,
                                    &mWisHalRec.params.config, &in_handle,
                                    mWisHalRec.params.flags, "input_stream",
                                    mWisHalRec.params.source);
        if (isForVickLogEnable) {
          printf("-----> For Vick: qahw_open_input_stream...\n");
        }
        if (rc) {
            fprintf(mWisHalRec.log_file, "ERROR :::: Could not open input stream, handle(%d)\n", mWisHalRec.params.handle);
            if (mWisHalRec.log_file != stdout)
                fprintf(stdout, "ERROR :::: Could not open input stream, handle(%d)\n", mWisHalRec.params.handle);
        }

        // Get buffer size to get upper bound on data to read from the HAL
        size_t buffer_size = qahw_in_get_buffer_size(in_handle);
        char *buffer = (char *)calloc(1, buffer_size);
        if (buffer == NULL) {
            fprintf(mWisHalRec.log_file, "calloc failed!!, handle(%d)\n", mWisHalRec.params.handle);
            if (mWisHalRec.log_file != stdout)
                fprintf(stdout, "calloc failed!!, handle(%d)\n", mWisHalRec.params.handle);
        }

        fprintf(mWisHalRec.log_file, " input opened, buffer  %p, size %zu, handle(%d)\n", buffer, buffer_size, mWisHalRec.params.handle);
        qahw_in_set_parameters(in_handle, param);
        if (isForVickLogEnable) {
          printf("-----> For Vick: qahw_in_set_parameters...\n");
        }

        fprintf(mWisHalRec.log_file, "\n Please speak into the microphone for %d seconds, handle(%d)\n", recordingTimeout, mWisHalRec.params.handle);
        if (mWisHalRec.log_file != stdout)
            fprintf(stdout, "\n Please speak into the microphone for %d seconds, handle(%d)\n", recordingTimeout, mWisHalRec.params.handle);

        memset(&in_buf,0, sizeof(qahw_in_buffer_t));
        start_time = time(0);
        mWisCommon.sysLocalTime("Start Recording!!!!!!!   Start Recording!!!!!!!   Start Recording!!!!!!!");

        bool isHaveDetected = false;
        isRecordingFinish = false;
        while(!exitAVS && !mThis->m_wisStatusChanged->isStopCaptureFlag) {
            in_buf.buffer = buffer;
            in_buf.bytes = buffer_size;
            ssize_t readSize = qahw_in_read(in_handle, &in_buf);
            /*if (isForVickLogEnable) {
              printf("-----> For Vick: qahw_in_read\n");
            }*/

            // To detect if end of recording
            if (mThis->m_wisStatusChanged->isStopCaptureFlag) {
              if (isForVickLogEnable) {
                printf("-----> For Vick: mThis->m_wisStatusChanged->isStopCaptureFlag\n");
              }
              break;
            }

            time_elapsed = difftime(time(0), start_time);

            if (sensoryKeyWordDetected) {  // King 201708
              if(time_elapsed < mWisHalRec.params.record_delay) {
                  usleep(1000000*(mWisHalRec.params.record_delay - time_elapsed));
                  time_elapsed = difftime(time(0), start_time);
                  continue;
              } else if (time_elapsed > mWisHalRec.params.record_delay + recordingTimeout/*mWisHalRec.params.record_length*/) {
                  fprintf(mWisHalRec.log_file, "\n Test for session with handle(%d) completed.\n", mWisHalRec.params.handle);
                  if (mWisHalRec.log_file != stdout)
                      fprintf(stdout, "\n Test for session with handle(%d) completed.\n", mWisHalRec.params.handle);
                  break;
              }
              if (!isHaveDetected) {
                isHaveDetected = true;
                if (!isExpectDirectStatus) {
                  if (isLaunchAVSKeyPressed || recognizeInitiatorType == InitiatorType_TAP) {
                     mThis->m_tapToTalkButton->startRecognizing(mWisAvs.m_AudioInputProcessor, mThis->m_TapToTalkAudioProvider);  // King debug
                  }
                }
                mThis->preRecording();
                continue;
              }
              // print recording time
              /*fprintf(mWisHalRec.log_file, "recording data size: %d.\n", written_size);
              mWisCommon.sysLocalTime("Recording test time");*/
            } else {
              start_time = time(0);
              /*if (isForVickLogEnable) {
                printf("-----> For Vick: start_time = time(0)\n");
              }*/
            }
            //mWisCommon.sysLocalTime("test.11111111111111111111111111111\n");
            if (!exitAVS) {
              long writeSize = p_producer->write(in_buf.buffer, buffer_size);

              // King test
              if (access("/data/test_file",F_OK) != -1) { // File exist
                  printf("produce_Recording readSize = %d, writeSize = %ld\n", readSize, writeSize);
              }
              /*if (isForVickLogEnable) {
                printf("-----> For Vick: p_producer->write\n");
              }*/
            }
            //mWisCommon.sysLocalTime("test.22222222222222222222222222\n");
        }
        sensoryKeyWordDetected = false;
        mWisCommon.sysLocalTime("End Recording!!!!!!!");
        /*mThis->m_wakeWordTrigger->keyWordDetected = false;  // King 201708*/

        fprintf(mWisHalRec.log_file, " closing input, handle(%d)\n", mWisHalRec.params.handle);
        printf("closing input\n");

        // Close input stream and device.
        rc = qahw_in_standby(in_handle);
        if (isForVickLogEnable) {
          printf("-----> For Vick: qahw_in_standby...\n");
        }
        if (rc) {
            fprintf(mWisHalRec.log_file, "out standby failed %d, handle(%d)\n",rc, mWisHalRec.params.handle);
            if (mWisHalRec.log_file != stdout)
                fprintf(stdout, "out standby failed %d, handle(%d)\n",rc, mWisHalRec.params.handle);
        }

        rc = qahw_close_input_stream(in_handle);
        if (isForVickLogEnable) {
          printf("-----> For Vick: qahw_close_input_stream...\n");
        }
        if (rc) {
            fprintf(mWisHalRec.log_file, "could not close input stream %d, handle(%d)\n",rc, mWisHalRec.params.handle);
            if (mWisHalRec.log_file != stdout)
                fprintf(stdout, "could not close input stream %d, handle(%d)\n",rc, mWisHalRec.params.handle);
        }

        // Print instructions to access the file.
        fprintf(mWisHalRec.log_file, "\n\n The audio recording has been saved to %s. Please use adb pull to get "
               "the file and play it using audacity. The audio data has the "
               "following characteristics:\n Sample rate: %i\n Format: %d\n "
               "Num channels: %i, handle(%d)\n\n",
               m_RecordingFilePath.c_str(), mWisHalRec.params.config.sample_rate, mWisHalRec.params.config.format, mWisHalRec.params.channels, mWisHalRec.params.handle);
        if (mWisHalRec.log_file != stdout)
            fprintf(stdout, "\n\n The audio recording has been saved to %s. Please use adb pull to get "
               "the file and play it using audacity. The audio data has the "
               "following characteristics:\n Sample rate: %i\n Format: %d\n "
               "Num channels: %i, handle(%d)\n\n",
               m_RecordingFilePath.c_str(), mWisHalRec.params.config.sample_rate, mWisHalRec.params.config.format, mWisHalRec.params.channels, mWisHalRec.params.handle);

         ret = qahw_unload_module(mWisHalRec.qahw_mod_handle);
         if (isForVickLogEnable) {
           printf("-----> For Vick: qahw_unload_module...\n");
         }
          if (ret) {
              fprintf(mWisHalRec.log_file, "could not unload hal %d \n",ret);
          }
          fprintf(mWisHalRec.log_file, "\n Done with hal record test \n");

          // King end
        fprintf(stderr, "producer is done\n");
        if (!exitAVS) {
          mThis->postRecording();
        }
        isRecordingFinish = true;

        pthread_exit(0);
        return 0;
    }

    static void* consume_wakewordDetect(void *ptr) {
      int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
      printf("consume_wakewordDetect thread: detachCode = %d\n", detachCode);

      printf("consume_wakewordDetect:: Start ...\n");
      Nut2AVSTest *mThis = (Nut2AVSTest *) ptr;

      auto alexaStopAlexaJokeBuffer = std::make_shared<avsCommon::avs::AudioInputStream::Buffer>(500000);
      auto alexaStopAlexaJokeSds = avsCommon::avs::AudioInputStream::create(alexaStopAlexaJokeBuffer, 2, 3);
      std::shared_ptr<AudioInputStream> alexaStopAlexaJokeAudioBuffer = std::move(alexaStopAlexaJokeSds);
      printf("consume_wakewordDetect:: create alexaStopAlexaJokeAudioBuffer ...\n");

      std::unique_ptr<AudioInputStream::Writer> alexaStopAlexaJokeAudioBufferWriter =
              alexaStopAlexaJokeAudioBuffer->createWriter(
                      avsCommon::avs::AudioInputStream::Writer::Policy::NONBLOCKABLE);
      printf("consume_wakewordDetect:: createWriter ...\n");

      AudioFormat compatibleAudioFormat;
      compatibleAudioFormat.sampleRateHz = COMPATIBLE_SAMPLE_RATE;
      compatibleAudioFormat.sampleSizeInBits = COMPATIBLE_SAMPLE_SIZE_IN_BITS;
      compatibleAudioFormat.numChannels = COMPATIBLE_NUM_CHANNELS;
      compatibleAudioFormat.endianness = COMPATIBLE_ENDIANNESS;
      compatibleAudioFormat.encoding = COMPATIBLE_ENCODING;

      std::shared_ptr<testKeyWordObserverDebug> keyWordObserverDebug;
      std::shared_ptr<testStateObserverDebug> stateObserverDebug;
      keyWordObserverDebug = std::make_shared<testKeyWordObserverDebug>(compatibleAudioFormat, (std::shared_ptr<alexaClientSDK::capabilityAgents::aip::AudioInputProcessor>) mWisAvs.m_AudioInputProcessor);
      stateObserverDebug = std::make_shared<testStateObserverDebug>();
      printf("consume_wakewordDetect:: create keyWordObserverDebug and stateObserverDebug ...\n");

      auto detector = kwd::SensoryKeywordDetector::create(
              alexaStopAlexaJokeAudioBuffer,
              compatibleAudioFormat,
              {keyWordObserverDebug},
              {stateObserverDebug},
              inputPath + RESOURCE_FILE);
      printf("consume_wakewordDetect:: kwd::SensoryKeywordDetector::create ...\n");

        RingBufferConsumer* p_consumer = (RingBufferConsumer *) mThis->p_consumer1;  // type cast to a pointer to thdata

        int buffer_size = 640;
        std::vector<int16_t> retVal(buffer_size/2, 0);
        //wakeWordCache.clear();
        printf("consume_wakewordDetect:: start read data from RingBuffer ...\n");
        while(!exitAVS && !sensoryKeyWordDetected) {
            int readSize = p_consumer->read((char *)&retVal[0], buffer_size);

            // King test
            if (access("/data/test_file",F_OK) != -1) { // File exist
                printf("consume_wakewordDetect readSize =  %d\n", readSize);
            }

            if (readSize != buffer_size) {
              usleep(100*1000);
              continue;
            }

            alexaStopAlexaJokeAudioBufferWriter->write(retVal.data(), buffer_size/2);

            /*if (wakeWordCache.size() >= 50) {
              printf("wakeWordCache size exceed 50.....\n");
              wakeWordCache.erase(wakeWordCache.begin());
            }
            wakeWordCache.push_back((char *)&retVal[0]);*/

            retVal.clear();
        }
        printf("consumer is done\n");

        pthread_exit(0);
        return 0;
    }

    RingBufferConsumer* p_consumer1;
    RingBufferProducer* p_producer1;
    // King end

    void handleOneSpeechBySensory(){
      printf("\n\n\n\n");
      mWisCommon.sysLocalTime("Start execute handleOneSpeechBySensory()");

      //Bob add
      m_wisStatusChanged->isStopCaptureFlag = false;
      m_wakeWordTrigger->keyWordDetected = false;
      isRecordingFinish = false;
      sensoryKeyWordDetected = false;

      int MAX_CYCLE = 1;
      int testCycleIndex = 0;
      while(testCycleIndex < MAX_CYCLE){
        isPass = false;

        std::string m_RecordingFilePath;
        recordingSource = REC_SOURCE_HAL_REC_TEST;  // King 201708
        if (recordingSource == REC_SOURCE_HAL_REC_TEST) {  // scenario 1: hal_rec_test data
            RingBuffer rb(19, YieldWaitConsumerStrategy());
            p_consumer1 = rb.createConsumer();
            RingBufferConsumer* p_consumer2 = rb.createConsumer();
            p_producer1 = rb.createProducer();

            pthread_t producer_thread1, consumer_thread1;
            int produceRecordingThreadError = pthread_create(&producer_thread1, NULL, &produce_Recording, (void*) this);
            int consumeWakewordDetectThreadError = pthread_create(&consumer_thread1, NULL, &consume_wakewordDetect, (void*) this);
            printf("\t%s: produceRecordingThreadError = %d, consumeWakewordDetectThreadError = %d\n", "handleOneSpeechBySensory", produceRecordingThreadError, consumeWakewordDetectThreadError);

            std::unique_lock<std::mutex> lock(m_wisMutex);
            while (!sensoryKeyWordDetected && !isExpectDirectStatus && !exitAVS
                          && !m_wisStateTrigger.wait_for(lock, WAIT_FOR_TIMEOUT_DURATION, [this]() { return sensoryKeyWordDetected || isExpectDirectStatus || exitAVS;})) {
                mWisCommon.sysLocalTime("handleOneSpeechBySensory::waitForWakeword------");
                mWisVolumeControl.sleep_ms(500);
            }
            lock.unlock();
            printf("sensoryKeyWordDetected = %d, isExpectDirectStatus = %d, exitAVS = %d\n", sensoryKeyWordDetected, isExpectDirectStatus, exitAVS);
            if (exitAVS) {
              printf("--> Exit AVS test process...\n");
              return;
            }

            if(isExpectDirectStatus){
              sensoryKeyWordDetected = true;
            }

            if(sensoryKeyWordDetected || isExpectDirectStatus){
              m_RecordingFilePath = STORE_ROOT_FOLDER + "hal_rec_test_rec_send.wav";
              FILE *fd = fopen(m_RecordingFilePath.c_str(),"w");
              if (fd == NULL) {
                  fprintf(stdout, "File open failed \n");
              }
              int bps = 16;

              struct wav_header hdr;
              hdr.riff_id = ID_RIFF;
              hdr.riff_sz = 0;
              hdr.riff_fmt = ID_WAVE;
              hdr.fmt_id = ID_FMT;
              hdr.fmt_sz = 16;
              hdr.audio_format = FORMAT_PCM;
              hdr.num_channels = 1;
              hdr.sample_rate = 16000;
              hdr.byte_rate = hdr.sample_rate * hdr.num_channels * (bps/8);
              hdr.block_align = hdr.num_channels * (bps/8);
              hdr.bits_per_sample = bps;
              hdr.data_id = ID_DATA;
              hdr.data_sz = 0;
              fwrite(&hdr, 1, sizeof(hdr), fd);

              int data_sz = 0;
              size_t buffer_size = 640;
              std::vector<int16_t> retVal(buffer_size/2, 0);

              if (isLaunchAVSKeyPressed || isExpectDirectStatus) {
                printf("--> handleOneSpeechBySensory::p_consumer1->getOffset = %ld\n", p_consumer1->getOffset());
                p_consumer2->setOffset(p_consumer1->getOffset());
	            }else if(sensoryKeyWordDetected){
                //int wakewordStartIndex = mWisAvs.m_AudioInputProcessor->readIndex;  // 17360
                //int chunkByteOffset = ((wakewordStartIndex /*+ 8000*/) * 1000 / 16000) / 10 * 320; // takes how much milliseconds, and it's 320byte per 10ms
                //int readOffset = p_consumer1->getOffset() - chunkByteOffset;
                //printf("--> %s oldOffset = %ld, readOffset = %d, chunkByteOffset = %d, wakewordStartIndex = %d\n", APP_TAG.c_str(), p_consumer1->getOffset(), readOffset, chunkByteOffset, wakewordStartIndex);

                long readOffset = ((m_wakewordBeginIndex - 8000) * 2)%(rb.getSize());  // 2 = (n*1000/16000)/10*320
                printf("--> handleOneSpeechBySensory::readOffset = %ld\n", readOffset);
                p_consumer2->setOffset(readOffset);

                /*long readOffset = (m_wakewordEndIndex - m_wakewordBeginIndex) * 2;
                int wakeWordChunks = readOffset/640;
                if (readOffset % 640 != 0) {
                  wakeWordChunks ++;
                }

                std::vector<int16_t> wakewordVal(640/2, 0);
                printf("need Chunks %d, wakeWordCache size: %d.....\n", wakeWordChunks, wakeWordCache.size());
                for(int i=wakeWordChunks; i > 0;i--){
                  //wakewordVal.push_back(wakeWordCache[wakeWordCache.size() - i]);
                  char *data= (char *)&wakeWordCache[wakeWordCache.size() - i];
                  //int written_size = fwrite((char *)&wakewordVal[0], 1, 640, fd);
                  int written_size = fwrite(data, 1, 640, fd);
                  //printf("written_size = %d, buffer_size = %d\n", written_size, buffer_size);
                  data_sz += written_size;
                  m_AudioBufferWriter->write(wakewordVal.data(), written_size/2);
                  wakewordVal.clear();
                }*/
                /*std::vector<int16_t> wakewordVal(readOffset/2, 0);
                //int readSize = p_consumer2->readWakeword((char *)&wakewordVal[0], readOffset);
                int written_size = fwrite((char *)&wakewordVal[0], 1, readOffset, fd);
                //printf("written_size = %d, buffer_size = %d\n", written_size, buffer_size);
                data_sz += written_size;
                m_AudioBufferWriter->write(wakewordVal.data(), written_size/2);
                wakewordVal.clear();*/

                /*printf("--> handleOneSpeechBySensory::p_consumer1->getOffset = %ld\n", p_consumer1->getOffset());
                p_consumer2->setOffset(p_consumer1->getOffset());*/
              }

              printf("exitAVS = %d, m_wisStatusChanged->isStopCaptureFlag = %d, isRecordingFinish = %d\n", exitAVS, m_wisStatusChanged->isStopCaptureFlag, isRecordingFinish );
              unsigned int readSize = 0;
              int written_size = 0;
              while(!exitAVS && !m_wisStatusChanged->isStopCaptureFlag && !isRecordingFinish) {
                  // King test
                  readSize = p_consumer2->read((char *)&retVal[0], buffer_size);
                  if (access("/data/test_file",F_OK) != -1) { // File exist
                    printf("consume to AVS readSize =  %d\n", readSize);
                  }

                  //printf("read mic data0....  \n");
                  if (readSize != buffer_size) {
                    usleep(100*1000);
                    //printf("read mic data0.... %d \n", readSize);
                    continue;
                  }

                  //printf("read mic data2.... %d \n", readSize);
                  written_size = fwrite((char *)&retVal[0], 1, buffer_size, fd);
                  if (access("/data/test_file",F_OK) != -1) { // File exist
                    printf("consume to AVS written_size =  %d\n", written_size);
                  }
                  data_sz += written_size;

                  m_AudioBufferWriter->write(retVal.data(), buffer_size/2);
                  retVal.clear();
              }

              hdr.data_sz = data_sz;
              hdr.riff_sz = data_sz + 44 - 8;
              fseek(fd, 0, SEEK_SET);
              fwrite(&hdr, 1, sizeof(hdr), fd);
              fclose(fd);
            }
        } else {  // post a audio to AVS
           m_RecordingFilePath = STORE_ROOT_FOLDER + "hal_rec_test_rec.wav";
           std::string file = APP_ROOT_FOLDER + "inputs/alexa_recognize_joke_test.wav";
           std::cout << "file: " << file << std::endl;
           const int RIFF_HEADER_SIZE = 44;
           std::ifstream inputFile(file.c_str(), std::ifstream::binary);
           if (!inputFile.good()) {
             std::cout << "Couldn't open audio file!" << std::endl;
             return ;
           }
           std::cout << "open audio file success!" << std::endl;
           inputFile.seekg(0, std::ios::end);
           int fileLengthInBytes = inputFile.tellg();
           if (fileLengthInBytes <= RIFF_HEADER_SIZE) {
             std::cout << "File should be larger than 44 bytes, which is the size of the RIFF header" << std::endl;
             return ;
           }

           inputFile.seekg(RIFF_HEADER_SIZE, std::ios::beg);
           std::cout << "\t\tfile lenght In bytes: " << fileLengthInBytes << std::endl;
           int numSamples = (fileLengthInBytes - RIFF_HEADER_SIZE) / 5;
           std::cout << "\t\tnumSamples: " << numSamples << std::endl;
           std::vector<int16_t> retVal(115200, 0);
           //std::vector<int16_t> retVal(320, 0);

           int index = 0;
           while(!inputFile.eof()){
             index ++;
             inputFile.read((char *)&retVal[0], 115200);
             m_AudioBufferWriter->write(retVal.data(), inputFile.gcount());
             retVal.clear();
             //std::cout << "index= " << index << std::endl;
           }
           std::cout << "Read file end!"<< std::endl;
           inputFile.close();
       }
       if (exitAVS) {
         printf("--> Exit AVS test process...\n");
         return;
       }

       isExpectDirectStatus = false;
       while (!isRecordingFinish) {
         usleep(200*1000);
       }

       totalAVSRequestNum ++;
       std::unique_lock<std::mutex> lock(m_wisMutex);
       std::chrono::milliseconds WAIT_FOR_STOP_CAPTURE(1500);
       if (!m_wisStateTrigger.wait_for(lock, WAIT_FOR_STOP_CAPTURE, [this]() { return m_wisStatusChanged->isStopCaptureFlag;})) {
           std::cout << "Can't detect a stopCapture directure ------" << std::endl;
       }
       lock.unlock();

       printf("--> m_wisStatusChanged->isStopCaptureFlag = %d\n", m_wisStatusChanged->isStopCaptureFlag);
       if (!m_wisStatusChanged->isStopCaptureFlag) {
         restoreDefaultLED();
         failAVSRequestNum ++;
         mWisCommon.backupAVSFailedAudio(m_RecordingFilePath, totalAVSRequestNum, failAVSRequestNum);
       }
       mWisCommon.sysLocalTime("---> Recognize done!");

       /*isAVSPlayingResponse = true;
       checkSpeechSynthesizerState();*/

       testCycleIndex ++;
       std::cout << "^_^^^_^^^^_^^^^^^_^^^^^^_^^^^^^^^_^^^^ testCycleIndex: " << testCycleIndex << '\n';
     }
    }

    //Bob debug for launch AVS while press key 20170830: PowerKey
  static void *launchAVSPressKey(void *argv){
      int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
      printf("launchAVSPressKey thread: detachCode = %d\n", detachCode);

      int keys_fd;
      struct input_event t;

      struct timeval keyPressedTime;
      struct timeval keyReleasedTime;

      keys_fd = open ("/dev/input/event0", O_RDONLY);  // event0 --> 114 --> vol down; event1 --> (766, 528, 115)  --> up to down
      if (keys_fd <= 0) {
        printf ("------------> open /dev/input/event0 device error!\n");
        return 0;
      }

      while (!exitAVS) {
        if (read(keys_fd, &t, sizeof (t)) == sizeof (t)) {
          if (t.type == EV_KEY)
            if (t.value == 0 || t.value == 1) {
              printf ("------------> key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
              if (t.value) {  // Pressed
                gettimeofday(&keyPressedTime,NULL);
              } else {  // Released
                gettimeofday(&keyReleasedTime,NULL);
                printf("------------> pressed time is %ld seconds\n", (keyReleasedTime.tv_sec-keyPressedTime.tv_sec));
                  if(t.code==116){
                    if (isSoftAPModeOn) {
                      printf(" *** launchAVSPressKey:: wakeword button pressed, but Hotspot_Mode is on, so skip this request.\n");
                    }else{
                      printf("detect the key and will launch AVS: %d\n", t.code);
                      closeFMDuringTest();
                      stopMediaPlayer(); //for barge-in
                      if (wakeWordDetector == WAKE_WORD_DETECTOR_SVA) {
                        printf("%s\n", "SVA type");
                        isLaunchAVSKeyPressed = true;
                        isSVADetected= true;
                        m_wisStateTrigger.notify_all();
                      } else if (wakeWordDetector == WAKE_WORD_DETECTOR_SENSORY) {
                        mWisCommon.sysLocalTime("---> reset state!");
                        mWisAvs.m_AudioInputProcessor->resetState();  // King end

                        printf("%s\n", "Sensory type");
                        isLaunchAVSKeyPressed = true;
                        sensoryKeyWordDetected = true;
                        m_wisStateTrigger.notify_all();
                      }
                    }
                  }
              }
            }
        }
      }
      close(keys_fd);

      pthread_exit(0);
      return 0;
    }
    //Bob end 20170830

    bool isPass = false;
    //Bob end

    std::shared_ptr<AuthObserver> m_authObserver;
    std::shared_ptr<AuthDelegate> m_authDelegate;
    std::shared_ptr<ConnectionStatusObserver> m_connectionStatusObserver;
    std::shared_ptr<MessageRouter> m_messageRouter;
    std::shared_ptr<TestMessageSender> m_avsConnectionManager;
    //std::shared_ptr<acl::AVSConnectionManager> mWisAvs.m_connectionManager;  // King add
    std::shared_ptr<TestExceptionEncounteredSender> m_exceptionEncounteredSender;
    std::shared_ptr<TestDirectiveHandler> m_directiveHandler;
    std::shared_ptr<DirectiveSequencer> m_directiveSequencer;
    std::shared_ptr<MessageInterpreter> m_messageInterpreter;
    //std::shared_ptr<ContextManager> m_contextManager;
    std::shared_ptr<TestSpeechSynthesizerObserver> m_speechSynthesizerObserver;
    std::shared_ptr<SpeechSynthesizer> m_speechSynthesizer;
    std::shared_ptr<avsCommon::avs::attachment::AttachmentManager> m_attachmentManager;
    std::shared_ptr<ClientMessageHandler> m_clientMessageHandler;
    std::shared_ptr<FocusManager> m_focusManager;
    std::shared_ptr<avsCommon::avs::DialogUXStateAggregator> m_dialogUXStateAggregator;
    std::shared_ptr<TestClient> m_testClient;
    std::shared_ptr<UserInactivityMonitor> m_userInactivityMonitor;
    FocusState m_focusState;
    std::mutex m_mutex;
    std::condition_variable m_focusChanged;
    bool m_focusChangeOccurred;
    // King add
    std::shared_ptr<TestWisStatusChanged> m_wisStatusChanged;

	  // Bob start
    //std::shared_ptr<AudioInputProcessor> m_AudioInputProcessor;
    std::shared_ptr<AipStateObserver> m_StateObserver;
    std::shared_ptr<tapToTalkButton> m_tapToTalkButton;
    std::shared_ptr<testStateProvider> m_stateProvider;
    std::unique_ptr<AudioInputStream::Writer> m_AudioBufferWriter;
    std::shared_ptr<AudioInputStream> m_AudioBuffer;
    std::shared_ptr<AudioProvider> m_TapToTalkAudioProvider;
    avsCommon::utils::AudioFormat m_compatibleAudioFormat;
	  // Bob end

    //Bob add 20170731
    std::shared_ptr<storage::SQLiteAlertStorage> m_alertStorage;
    std::shared_ptr<renderer::RendererInterface> m_alertRenderer;
    std::shared_ptr<TestAlertObserver> m_alertObserver;
    std::shared_ptr<AlertsCapabilityAgent> m_alertsAgent;
    std::shared_ptr<CertifiedSender> m_certifiedSender;
    //Bob end 20170731

    //Bob add 20170818 for audioplayer
    //std::shared_ptr<capabilityAgents::audioPlayer::AudioPlayer> m_audioPlayer;
    std::shared_ptr<TestClient> m_testContentClient;
    //Bob end 20170818

    //Bob add for settings 20171019
    /// Global Observer for all the settings which sends the event.
    std::shared_ptr<SettingsUpdatedEventSender> m_settingsEventSender;
    /// Settings Storage object.
    std::shared_ptr<SQLiteSettingStorage> m_storage;
    //Bob end 20171019

// Bob start
#ifdef KWD
    std::shared_ptr<wakeWordTrigger> m_wakeWordTrigger;
#ifdef KWD_KITTAI
    std::unique_ptr<kwd::KittAiKeyWordDetector> m_detector;
#elif KWD_SENSORY
    std::unique_ptr<kwd::SensoryKeywordDetector> m_detector;
#endif
// Bob end
#endif

};

    // ******************************************** King add start: SVA
    static void *event_handler_thread(void *);

    static struct sm_session_data *get_sm_session_data(int session_id)
    {
        int i;
        for (i = 0; i < num_sessions; i++) {
            if (sound_trigger_info[i].session_id == session_id)
                return &sound_trigger_info[i];
        }
        return NULL;
    }

    static struct sm_session_data *get_sound_trigger_info(sound_model_handle_t sm_handle)
    {
        int i;
        for (i = 0; i < num_sessions; i++) {
            if (sound_trigger_info[i].sm_handle == sm_handle)
                return &sound_trigger_info[i];
        }
        return NULL;
    }

    static void init_sm_session_data(void)
    {
        int i;
        for (i = 0; i < MAX_SOUND_TRIGGER_SESSIONS; i++) {
            sound_trigger_info[i].session_id = -1;
            sound_trigger_info[i].vendor_uuid = qc_uuid;
            sound_trigger_info[i].sm_file_path[0] = '\0';
            sound_trigger_info[i].sm_handle = -1;
            sound_trigger_info[i].num_kws = 0;
            sound_trigger_info[i].num_users = 0;
            sound_trigger_info[i].loaded = false;
            sound_trigger_info[i].started = false;
            sound_trigger_info[i].counter = 0;
            memset(&sound_trigger_info[i].rc_config, 0,
                   sizeof(struct sound_trigger_recognition_config));
            memset(&sound_trigger_info[i].qsthw_event, 0,
                   sizeof(struct qsthw_phrase_recognition_event));
        }
    }

    static void eventCallback(struct sound_trigger_recognition_event *event, __attribute__((unused)) void *sessionHndl) {
        int rc;
        pthread_attr_t attr;
        pthread_t callback_thread;
        sound_model_handle_t sm_handle = event->model;
        struct qsthw_phrase_recognition_event *qsthw_event;
        uint64_t event_timestamp;

        printf("[%d] Callback event received: %d\n", event->model, event->status);
        qsthw_event = (struct qsthw_phrase_recognition_event *)event;
        event_timestamp = qsthw_event->timestamp;
        printf("[%d] Event detection timestamp %llu\n", sm_handle, event_timestamp);

        rc = pthread_attr_init(&attr);
        if (rc != 0) {
            printf("pthread attr init failed %d\n",rc);
            return;
        }
        struct sm_session_data *sm_data = get_sound_trigger_info(sm_handle);
        if (sm_data == NULL) {
            printf("Error: Invalid sound model handle %d\n", sm_handle);
            return;
        }

        memcpy(&sm_data->qsthw_event, qsthw_event,
               sizeof(struct qsthw_phrase_recognition_event));

        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        rc = pthread_create(&callback_thread, &attr, event_handler_thread, sm_data);
        if (rc != 0)
            printf("event_handler_thread create failed %d\n",rc);
        pthread_attr_destroy(&attr);
    }

    void start_recognition(sound_model_handle_t sm_handle, struct sound_trigger_recognition_config *rc_config){
        int status = qsthw_start_recognition(st_mod_handle, sm_handle, rc_config, eventCallback, NULL);
        if (SVA_STATUS_OK != status) {
            mWisCommon.sysLocalTime("*****************************    WARNING:  start_recognition failed!!!    **********************************\n");
            exitSva = true;
            return;
        }
    }

    static void *event_handler_thread(void *context)
    {
        int system_ret;
        struct sm_session_data *sm_data = (struct sm_session_data *) context;
        if (!sm_data) {
            printf("Error: context is null\n");
            return NULL;
        }

        system_ret = system("echo qti_services > /sys/power/wake_lock");
        if (system_ret < 0)
           printf("%s: Failed to acquire qti lock\n", __func__);
        else
           printf("%s: Success to acquire qti lock\n", __func__);

        struct sound_trigger_recognition_config *rc_config = &sm_data->rc_config;
        struct qsthw_phrase_recognition_event *qsthw_event =
                                                           &sm_data->qsthw_event;
        sound_model_handle_t sm_handle = sm_data->sm_handle;
        printf("[%d] session params %p, %d\n", sm_handle, rc_config, total_duration_ms);

        if (qsthw_event && qsthw_event->phrase_event.common.capture_available) {
            printf ("capture LAB data\n");
            qsthw_event_cache = qsthw_event;
            //capture_lab_data(qsthw_event); // King remove
        }

        closeFMDuringTest();
        stopMediaPlayer();
        while (!isAVSFinish) {
          usleep(0.4*1000*1000);
        }

        // King add
        // handleOneSpeech();
        printf("*******************isLaunchAVSKeyPressed value: %d\n", isLaunchAVSKeyPressed);
        if(!isLaunchAVSKeyPressed){ //Bob add for TAP key pressed 20170907
          isSVADetected = true;
		      m_wisStateTrigger.notify_all();

          while (!isAVSFinish) {
            usleep(0.4*1000*1000);
          }
          isAVSFinish = false;
          isAVSPlayingResponse = false;
          while (!isAVSFinish && !isAVSPlayingResponse) {
            usleep(0.4*1000*1000);
          }
        }

        /* ignore error */
        start_recognition(sm_handle, rc_config);
        sm_data->counter++;
        printf("[%d] callback event processed, detection counter %d\n", sm_handle, sm_data->counter);
        printf("proceed with utterance or command \n");
        printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n\n");

        if (system_ret >= 0) {
            system_ret = system("echo qti_services > /sys/power/wake_unlock");
            if (system_ret < 0)
                printf("%s: Failed to release qti lock\n", __func__);
            else
                printf("%s: Success to release qti lock\n", __func__);
        }
        mWisCommon.sysLocalTime("****************************  SVA start to recognize...........   *********************************** ");

        return NULL;
    }

    const char *handle_set_parameters()
    {
        char kv_pair[128];
        char *id, *test_r, *temp_str;
        int i;

        printf("Enter set param key value string:(ex - <st_session_pause=1>");
        fgets(kv_pair, 128, stdin);
        printf("Received key value pair: %s\n", kv_pair);

        kv_pair[strcspn(kv_pair, "\r\n")] = '\0';
        temp_str = strdup(kv_pair);

        id = strtok_r(kv_pair, "= ", &test_r);
        if (!id) {
            printf("%s: incorrect key value pair", __func__);
            return NULL;
        }

        for (i = 0; i < MAX_SET_PARAM_KEYS; i++) {
             if (!strncmp(id, set_param_key_array[i], strlen(set_param_key_array[i])))
                 return temp_str;
        }

        return NULL;
    }

    static int string_to_uuid(const char *str, sound_trigger_uuid_t *uuid)
    {
        int tmp[10];

        if (str == NULL || uuid == NULL) {
            return -EINVAL;
        }

        if (sscanf(str, "%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x",
                tmp, tmp + 1, tmp + 2, tmp + 3, tmp + 4, tmp + 5, tmp + 6,
                tmp + 7, tmp+ 8, tmp+ 9) < 10) {
            return -EINVAL;
        }
        uuid->timeLow = (uint32_t)tmp[0];
        uuid->timeMid = (uint16_t)tmp[1];
        uuid->timeHiAndVersion = (uint16_t)tmp[2];
        uuid->clockSeq = (uint16_t)tmp[3];
        uuid->node[0] = (uint8_t)tmp[4];
        uuid->node[1] = (uint8_t)tmp[5];
        uuid->node[2] = (uint8_t)tmp[6];
        uuid->node[3] = (uint8_t)tmp[7];
        uuid->node[4] = (uint8_t)tmp[8];
        uuid->node[5] = (uint8_t)tmp[9];

        return 0;
    }

    void startSVAFirstRecognize(){
      mWisCommon.sysLocalTime("startSVAFirstRecognize  start ...............");
      struct sm_session_data *ses_data = get_sm_session_data(1);
      if (ses_data == NULL) {
          printf("Error: Invalid sound model handle %d\n", 1);
          return;
      }

      rc_config = &ses_data->rc_config;
      sm_handle = ses_data->sm_handle;

      printf("[%d] in start, ses state %d\n", 1, ses_data->started);
      if (ses_data->started)
          return;

      int status = qsthw_start_recognition(st_mod_handle, sm_handle,
                                      rc_config, eventCallback, NULL);

      if (SVA_STATUS_OK != status) {
          printf("start_recognition failed\n");
          return;
      }
      ses_data->started = true;
      ses_data->counter = 0;
      mWisCommon.sysLocalTime("startSVAFirstRecognize end...............");
    }

    void *openSVA(void *arg) {
        //bool exit_loop = false;
        int sm_data_size  = 0, opaque_data_size = 0;
        int sound_model_size = 0, i, k;
        unsigned int j;
        uint32_t rc_config_size;
        struct sound_trigger_phrase_sound_model *sound_model = NULL;
        struct sound_trigger_recognition_config *rc_config = NULL;
        struct keyword_buffer_config kb_config;
        bool user_verification = false;
        unsigned int kw_conf = 60; //default confidence level is 60
        unsigned int user_conf = 60;
        bool capture_requested = false;
        bool lookahead_buffer = false, keyword_buffer = false;
        bool usr_req_lookahead_buffer = false;
        int index = -1;

        // King debug
        int kingFlag = 0;

        // King : to find uim file under current folder
        DIR    *dir;
        struct   dirent    *ptr;
        dir = opendir(APP_ROOT_FOLDER.c_str()); ///open the dir
        char *uimFileName = NULL;
        while((ptr = readdir(dir)) != NULL) ///read the list of this dir
        {
          char *find = strstr(ptr->d_name,".uim");
          if(find){
            if(strcmp(find,".uim") == 0){
              printf("d_type:%d d_name: %s\n", ptr->d_type,ptr->d_name);
              uimFileName = ptr->d_name;
              break;
            }
          }
        }
        closedir(dir);
        if (uimFileName != NULL) {
          printf("Found a sound module file, name is: %s\n", uimFileName);
        }else {
          printf("WARNING:: Can not found the sound module file!!!\n");
          return 0;
        }

        char svaCommand[180];
        if (recordingSource == REC_SOURCE_SVA_LAB_DATA) {
          sprintf(svaCommand, "sva -ns 1 -id 1 -sm %s%s -nk %d -nu %d -lab true -lab_duration 15 -kb true -kb_duration 2", APP_ROOT_FOLDER.c_str(), uimFileName, svaNumberOfKeywords, svaNumberOfUsersPerKeyword);
        } else {
          sprintf(svaCommand, "sva -ns 1 -id 1 -sm %s%s -nk %d -nu %d", APP_ROOT_FOLDER.c_str(), uimFileName, svaNumberOfKeywords, svaNumberOfUsersPerKeyword);
        }

        printf("svaCommand: %s\n", svaCommand);
        std::vector<char*> argv;
        char *p = strtok(svaCommand," ");
        while(p != NULL){
            std::cout << "content: " << p << std::endl;
            argv.push_back(p);
            p = strtok(NULL," ");
        }
        int argc = argv.size();

        if (argc < 3) {
            printf(SOUNDTRIGGER_TEST_USAGE);
            return 0;
        }

        /* arguments to start with number of sessions always */
        if ((strcmp(argv[1], "-ns") == 0)) {
             num_sessions  = atoi(argv[2]);
        }

        if ((num_sessions <= 0) || (num_sessions > MAX_SOUND_TRIGGER_SESSIONS)) {
            printf(SOUNDTRIGGER_TEST_USAGE);
            return 0;
        }

        init_sm_session_data();
        int params = 0, count = 0;
        i = 3;

        while (count < num_sessions) {
            if ((i < argc) && (strcmp(argv[i], "-id") == 0) && ((i+1) < argc)) {
                index++;
                sound_trigger_info[index].session_id  = atoi(argv[i+1]);
                params = 1;
            }
            if (((i+2) < argc) && (strcmp(argv[i+2], "-sm") == 0)) {
                if (((i+3) < argc) && (index >= 0)) {
                    /*strlcpy(sound_trigger_info[index].sm_file_path, argv[i+3],
                           sizeof(sound_trigger_info[index].sm_file_path));*/ // King updated
                   strcpy(sound_trigger_info[index].sm_file_path, argv[i+3]);
                    params++;
                }
            }
            if (((i+4) < argc) && (strcmp(argv[i+4], "-nk") == 0)) {
                if (((i+5) < argc) && (index >= 0)) {
                    sound_trigger_info[index].num_kws = atoi(argv[i+5]);
                    params++;
                }
            }
            if (((i+6) < argc) && (strcmp(argv[i+6], "-nu") == 0)) {
                if (((i+7) < argc) && (index >= 0)) {
                    sound_trigger_info[index].num_users = atoi(argv[i+7]);
                    params++;
                }
            }
            /* If vendor uuid entered without entering number of users*/
            if (((i+6) < argc) && (strcmp(argv[i+6], "-vendor_uuid") == 0)) {
                if (((i+7) < argc) && (index >= 0)) {
                    string_to_uuid(argv[i+7],
                                   &sound_trigger_info[index].vendor_uuid);
                    params++;
                }
            }
            if (((i+8) < argc) && (strcmp(argv[i+8], "-vendor_uuid") == 0)) {
                if (((i+9) < argc) && (index >= 0)) {
                    string_to_uuid(argv[i+9],
                                   &sound_trigger_info[index].vendor_uuid);
                    params++;
                }
            }
            if (params < MIN_REQ_PARAMS_PER_SESSION ) {
                printf("Insufficient data entered for session %d\n", count);
                printf(SOUNDTRIGGER_TEST_USAGE);
                return 0;
            }
            count++;
            i += (params * 2);
        }

        if (++index != num_sessions) {
            printf("Insufficient data entered for sessions\n");
            printf(SOUNDTRIGGER_TEST_USAGE);
            return 0;
        }

        while (i < argc) {
            if ((strcmp(argv[i], "-user") == 0) && ((i+1) < argc)) {
                user_verification =
                      (0 == strncasecmp(argv[i+1], "true", 4))? true:false;
            } else if ((strcmp(argv[i], "-lab") == 0) && ((i+1) < argc)) {
                lookahead_buffer =
                      (0 == strncasecmp(argv[i+1], "true", 4))? true:false;
                usr_req_lookahead_buffer = true;
                isCaptureLABData = true;
            } else if ((strcmp(argv[i], "-lab_duration") == 0) && ((i+1) < argc)) {
                lab_duration = atoi(argv[i+1]);
            } else if ((strcmp(argv[i], "-kb") == 0) && ((i+1) < argc)) {
                keyword_buffer =
                      (0 == strncasecmp(argv[i+1], "true", 4))? true:false;
            } else if ((strcmp(argv[i], "-kb_duration") == 0) && ((i+1) < argc)) {
                kb_duration_ms = atoi(argv[i+1]);
            } else if ((strcmp(argv[i], "-kwcnf") == 0) && ((i+1) < argc)) {
                kw_conf = atoi(argv[i+1]);
            } else if ((strcmp(argv[i], "-usrcnf") == 0) && ((i+1) < argc)) {
                user_conf = atoi(argv[i+1]);
            } else {
                printf("Invalid syntax\n");
                printf(SOUNDTRIGGER_TEST_USAGE);
                exit (0);
            }
            i += 2;
        }

        kw_conf = svaKeywordConfidenceLevel;  // King add

        int status = 0;
        if (usr_req_lookahead_buffer) {
            if ((lookahead_buffer == false) && (keyword_buffer == true)) {
                printf("Invalid usecase: lab can't be false when keyword buffer is true ");
                status = -EINVAL;
                return 0;
            }
        }
        printf("keyword buffer %d",keyword_buffer);
        capture_requested = (lookahead_buffer || keyword_buffer) ? true : false;
        total_duration_ms = (lookahead_buffer ? lab_duration * 1000 : 0) + (keyword_buffer ? kb_duration_ms : 0);

        char command[128];

        st_mod_handle = qsthw_load_module(QSTHW_MODULE_ID_PRIMARY);
        if (NULL == st_mod_handle) {
            printf("qsthw_load_module() failed\n");
            status = -EINVAL;
            return 0;
        }

        // King debug
        qahw_module_handle_t *qa_mod_handle = qahw_load_module(QAHW_MODULE_ID_PRIMARY);
        if (isForVickLogEnable) {
          printf("-----> For Vick: SVA qahw_load_module...\n");
        }
        if (NULL == qa_mod_handle) {
            printf("qahw_load_module() failed\n");
            status = -EINVAL;
            return 0;
        }

        FILE *fp = NULL;
        for (k = 0; k < num_sessions; k++) {
            int num_kws = sound_trigger_info[k].num_kws;
            unsigned int num_users =
                     user_verification ? sound_trigger_info[k].num_users : 0;
            sound_model_handle_t sm_handle = 0;

            if (fp)
                fclose(fp);
            fp = fopen(sound_trigger_info[k].sm_file_path, "rb");
            if (fp == NULL) {
                printf("Could not open sound model file : %s\n",
                                       sound_trigger_info[k].sm_file_path);
                isAVSFinish = true;
                goto error;
            }

            /* Get the sound mode size i.e. file size */
            fseek( fp, 0, SEEK_END);
            sm_data_size  = ftell(fp);
            fseek( fp, 0, SEEK_SET);

            sound_model_size = sizeof(struct sound_trigger_phrase_sound_model) + sm_data_size;
            if (sound_model)
                free(sound_model);

            sound_model = (struct sound_trigger_phrase_sound_model *)calloc(1, sound_model_size);
            if (sound_model == NULL) {
                printf("Could not allocate memory for sound model");
               isAVSFinish = true;
               goto error;
            }

            sound_model->common.type = SOUND_MODEL_TYPE_KEYPHRASE;
            sound_model->common.data_size = sm_data_size;
            sound_model->common.data_offset = sizeof(*sound_model);
            sound_model->num_phrases = num_kws;
            for (i = 0; i < num_kws; i++) {
                sound_model->phrases[i].num_users = num_users;
                if (user_verification)
                    sound_model->phrases[i].recognition_mode = RECOGNITION_MODE_VOICE_TRIGGER |
                                         RECOGNITION_MODE_USER_IDENTIFICATION;
                else
                    sound_model->phrases[i].recognition_mode = RECOGNITION_MODE_VOICE_TRIGGER;
            }
            int bytes_read = fread((char*)sound_model+sound_model->common.data_offset , 1, sm_data_size , fp);
            printf("bytes from the file %d\n", bytes_read);
            if (bytes_read != sm_data_size) {
                printf("Something wrong while reading data from file: bytes_read %d file_size %d", bytes_read, sm_data_size);
                isAVSFinish = true;
                goto error;
            }

            memcpy(&sound_model->common.vendor_uuid, &sound_trigger_info[k].vendor_uuid,
                   sizeof(sound_trigger_uuid_t));
            printf("sound model data_size %d data_offset %d\n", sm_data_size, sound_model->common.data_offset);
            status = qsthw_load_sound_model(st_mod_handle, &sound_model->common, NULL, NULL, &sm_handle);
            if (SVA_STATUS_OK != status) {
                printf("load_sound_model failed\n");
                printf("\t>>>SVA: load_sound_model failed... will try again!\n");
                isAVSFinish = true;
                goto error;
            }
            sound_trigger_info[k].loaded = true;

            if (rc_config)
                free(rc_config);
            if (keyword_buffer)
                opaque_data_size = sizeof(struct keyword_buffer_config);

            rc_config_size = sizeof(struct sound_trigger_recognition_config) + opaque_data_size;
            rc_config = (struct sound_trigger_recognition_config *)calloc(1, rc_config_size);
            if (rc_config == NULL) {
                printf("Could not allocate memory for recognition config");
                isAVSFinish = true;
                goto error;
            }
            rc_config->capture_handle = AUDIO_IO_HANDLE_NONE;
            rc_config->capture_device = AUDIO_DEVICE_NONE;
            rc_config->capture_requested = capture_requested;
            rc_config->num_phrases = num_kws;

            if (keyword_buffer) {
                kb_config.version = SM_MINOR_VERSION;
                kb_config.kb_duration = kb_duration_ms;
                memcpy((char *) rc_config + rc_config->data_offset, &kb_config, sizeof(struct keyword_buffer_config));
                rc_config->data_size = sizeof(struct keyword_buffer_config);
                rc_config->data_offset = sizeof(struct sound_trigger_recognition_config);
            }
            int user_id = num_kws; //user_id should start from num_kws
            for (i = 0; i < num_kws; i++) {
                rc_config->phrases[i].id = i;
                rc_config->phrases[i].confidence_level = kw_conf;
                rc_config->phrases[i].num_levels = num_users;
                for (j = 0; j < num_users; j++) {
                    rc_config->phrases[i].levels[j].level = user_conf;
                    rc_config->phrases[i].levels[j].user_id = user_id++;
                }
                if (user_verification)
                    rc_config->phrases[i].recognition_modes = RECOGNITION_MODE_VOICE_TRIGGER |
                                 RECOGNITION_MODE_USER_IDENTIFICATION;
                else
                    rc_config->phrases[i].recognition_modes = RECOGNITION_MODE_VOICE_TRIGGER;
            }

            sound_trigger_info[k].sm_handle = sm_handle;
            memcpy(&sound_trigger_info[k].rc_config, rc_config, sizeof(*rc_config));
            printf("[%d]session params %p, %p, %d\n", k, &sound_trigger_info[k], rc_config, sm_handle);
        }

        // do {
        printf("King--> start test \n");
        while(!exitSva) {
            /*printf("Enter command in following format:\n");
            printf("\t<session_id:start/stop/set_param> (ex - 1:start)\n");
            printf("\t<global_set_param/exit>\n");
            printf(">> ");*/
            if(kingFlag == 0){
              strcpy(command, "1:start");
              printf("King--> Auto start\n");
              kingFlag = 1;

            /* Process session independent commands */
            /*if(!strncmp(command, "exit", 4)){
                printf("exiting the loop ..\n");
                exit_loop = true;
                continue;
            }

            if(!strncmp(command, "global_set_param", 16)) {
                printf("in global set parameters\n");
                const char *param = handle_set_parameters();
                if (param == NULL)
                    continue;
                printf("global param to set %s\n", param);
                sound_model_handle_t global_sm_handle = 0;
                status = qsthw_set_parameters(st_mod_handle, global_sm_handle, param);
                if (param)
                    free(param);
                if (SVA_STATUS_OK != status) {
                    printf("Global set parameters failed\n");
                    exit_loop = true;
                }
                continue;
            }*/

            /* Process session specific commands */
            char *token = strtok(command, ":");
            if (token == NULL) {
                printf("Error: Invalid command entered %s\n", token);
                continue;
            }
            int ses_id = atoi(token);
            if (ses_id == 0 && strncmp(token, "0", 1)) {
                printf("Enter valid command %s\n", token);
                continue;
            }
            struct sm_session_data *ses_data = get_sm_session_data(ses_id);
            if (ses_data == NULL) {
                printf("Error: Invalid sound model handle %d\n", ses_id);
                continue;
            }

            token = strtok(NULL, "\0");
            if (token == NULL) {
                printf("Invalid cmd following session id %d:%s\n",ses_id, token);
                continue;
            }
            printf("[%d][%p] command entered %s\n", ses_id, ses_data, token);
            rc_config = &ses_data->rc_config;
            sm_handle = ses_data->sm_handle;

            if(!strncmp(token, "start", 5)) {
                /*printf("[%d] in start, ses state %d\n", ses_id, ses_data->started);
                if (ses_data->started)
                    continue;
                status = qsthw_start_recognition(st_mod_handle, sm_handle,
                                                rc_config, eventCallback, NULL);
                if (SVA_STATUS_OK != status) {
                    printf("start_recognition failed\n");
                    //exit_loop = true;
                    exitSva = true;
                }
                ses_data->started = true;
                ses_data->counter = 0;*/
            } else if(!strncmp(token, "stop", 4)) {
                printf("[%d] in stop, ses state %d\n", ses_id, ses_data->started);
                if (!ses_data->started)
                    continue;
                status = qsthw_stop_recognition(st_mod_handle, sm_handle);
                if (SVA_STATUS_OK != status) {
                    printf("stop_recognition failed\n");
                    //exit_loop = true;
                }
                ses_data->started = false;
            } else if(!strncmp(token, "set_param", 9)) {
                printf("[%d] in set parameters %d\n", ses_id, ses_data->started);
                const char *param = handle_set_parameters();
                if (param == NULL)
                    continue;
                printf("[%d] param to set %s\n", ses_id, param);
                status = qsthw_set_parameters(st_mod_handle, sm_handle, param);
                if (param)
                    free((void*)param);
                if (SVA_STATUS_OK != status) {
                    printf("set parameters failed\n");
                    //exit_loop = true;
                }
            }

          }else{
            //fgets(command, 128, stdin);
            if (isAVSFinish && isSVAReady && isAVSReady) {
              strcpy(command, "King_wait");
              printf("\t>>>SVA: Waiting for a keyword, please say 'Alexa'......\n");
            }
            if (!isSVAReady) {
              isSVAReady = true;
              mWisCommon.sysLocalTime("\n\n\n>>>>>>>>>  SVA: SVA is ready......\n\n\n");
              if (isAVSReady && isSVAReady) {
                if (isPlayEnabled) {
                  std::string LOCAL_AUDIO_FILE_PATH = APP_ROOT_FOLDER + "iamready.mp3";
                  std::cout<<"LOCAL_AUDIO_FILE_PATH=" << LOCAL_AUDIO_FILE_PATH <<std::endl;
                  m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);

                  if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
                    if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
                      while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
                        ;
                      }
                      m_localMediaPlayer.m_mediaPlayer->stop();
                    }
                  }
                }
                startSVAFirstRecognize();
              }
            }
          }
        // King debug
        sleep(1);
        }
        // } while(!exit_loop);

        error:
        printf("\t>>>SVA: stop recognition and unload sound model ...\n");
        exitAVS = true;  // King add
        m_wisStateTrigger.notify_all();
        for (i = 0; i < num_sessions; i++) {
            sound_model_handle_t sm_handle = sound_trigger_info[i].sm_handle;
            if (sound_trigger_info[i].started) {
                status = qsthw_stop_recognition(st_mod_handle, sm_handle);
                if (SVA_STATUS_OK != status)
                    printf("stop_recognition failed\n");
                sound_trigger_info[i].started = false;
            }
            if (sound_trigger_info[i].loaded) {
                status = qsthw_unload_sound_model(st_mod_handle, sm_handle);
                if (SVA_STATUS_OK != status)
                    printf("unload_sound_model failed\n");
                sound_trigger_info[i].loaded = false;
            }
        }
        /*if (rc_config){
          printf("\t>>>SVA: free rc_config ...\n");
          free(rc_config);
        }*/

        status = qsthw_unload_module(st_mod_handle);
        if (SVA_STATUS_OK != status) {
           printf("qsthw_unload_module failed, status %d\n", status);
        }

        // King debug
        status = qahw_unload_module(qa_mod_handle);
        if (isForVickLogEnable) {
          printf("-----> For Vick: SVA qahw_unload_module...\n");
        }
        if (SVA_STATUS_OK != status) {
           printf("qahw_unload_module failed, status %d\n", status);
        }

        if (sound_model){
          printf("\t>>>SVA: free sound_model ...\n");
          free(sound_model);
        }
        if (fp){
          printf("\t>>>SVA: fclose fp ...\n");
          fclose(fp);
        }

        // King debug
        while (!isAVSFinish) {
          printf("\t>>>SVA: Waiting for AVS finish ...\n");
          sleep(1);
        }
        isSVAFinish = true;
        m_wisStateTrigger.notify_all();
        return 0;
    }

    void launchSVA(){
      pthread_t svaAction;
      int svaActionError = pthread_create(&svaAction, NULL, openSVA,NULL);
      printf("\tNut2AVSTest: svaActionError = %d\n", svaActionError);
    }
    // King add end: SVA

    TEST_F(Nut2AVSTest, avsFunctionTest) {
        //Bob debug for launch AVS while press key 20170830
        pthread_t  launchAVSKeyDetectThread;
        int launchAvsKeyDetectError = pthread_create(&launchAVSKeyDetectThread, NULL, launchAVSPressKey,NULL);
        printf("\tDB20_demo_tool: volUpKeyDetectError = %d\n", launchAvsKeyDetectError);
        //Bob end 20170830

        std::string fileName = configPath;
        std::ifstream inputFile(fileName.c_str());
        std::cout<<"file name: " << fileName << std::endl;
        if(!inputFile.good()){
          isAVSReady = false;
          std::cout << "Couldn't open refreshToken.json file, Please log in to your account first!" << '\n';
          inputFile.close();
        }else {
          inputFile.close();
          std::cout << "start AVS test" << std::endl;
          isAVSReady = true;
          mWisCommon.sysLocalTime("\n\n\n>>>>>>>>>  AVS: AVS is ready......\n\n\n");
          if (isAVSReady) {
            restoreDefaultLED();
          }

          if (wakeWordDetector == WAKE_WORD_DETECTOR_SVA) {
            if (isAVSReady && isSVAReady) {
              if (isPlayEnabled) {
                std::string LOCAL_AUDIO_FILE_PATH = APP_ROOT_FOLDER + "iamready.mp3";
                std::cout<<"LOCAL_AUDIO_FILE_PATH=" << LOCAL_AUDIO_FILE_PATH <<std::endl;
                m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);
                std::cout<<" TEST_F setMediaPlayerSource"<<std::endl;

                if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
                  if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
                    while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
                      ;
                    }
                    m_localMediaPlayer.m_mediaPlayer->stop();
                  }
                }
              }
              startSVAFirstRecognize();
            }

            while (!exitAVS) {
                std::unique_lock<std::mutex> lock(m_wisMutex);
                while (!isExpectDirectStatus
                        && !m_wisStateTrigger.wait_for(lock, WAIT_FOR_TIMEOUT_DURATION, [this]() { return isExpectDirectStatus || isSVADetected || exitAVS;})) {
                    std::cout << "TEST_F::avsFunctionTest  ------" << std::endl;
                }
                lock.unlock();

                std::cout << "TEST_F::isExpectDirectStatus  ------" << isExpectDirectStatus << std::endl;
                if (isSVADetected || isExpectDirectStatus) {
                  isSVADetected = false;

                  handleOneSpeech();
                  std::cout << "TEST_F::handleOneSpeech end  ------" << std::endl;

                  isAVSFinish = true;
                }
            }
          } else if (wakeWordDetector == WAKE_WORD_DETECTOR_SENSORY) {
            if (isAVSReady) {
              if (isPlayEnabled) {
                std::string LOCAL_AUDIO_FILE_PATH = APP_ROOT_FOLDER + "iamready.mp3";
                std::cout<<"LOCAL_AUDIO_FILE_PATH=" << LOCAL_AUDIO_FILE_PATH <<std::endl;
                m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);
                std::cout<<" TEST_F setMediaPlayerSource"<<std::endl;

                if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
                  if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
                    while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
                      ;
                    }
                    m_localMediaPlayer.m_mediaPlayer->stop();
                  }
                }
              }
            }
            while (!exitAVS) {
                handleOneSpeechBySensory();
                usleep(50*1000);
            }
          }
        }
    }
    // ********************************************** King end

} // namespace test
} // namespace integration
} // namespace alexaClientSDK

bool isVolUpLongPressed = false;
bool isVolDownLongPressed = false;

// UDP multicast
bool isMulticastEnabled = false;
bool isNetworkChecking = false;
void *findMe(void *argv);
void *executeAvsCreateProductData(void *argv);

void playWarnSound(){
  stopMediaPlayer();

  if (mWisCommon.getCurrentSKU() == SKU_WCD) {
    if (wcdPlaySoundCueType == WCD_PLAY_SOUND_CUE_TYPE_MEDIA_PLAYER) {
      //system("gst-play-1.0 " + APP_ROOT_FOLDER + "dong.mp3");
      std::string LOCAL_AUDIO_FILE_PATH = APP_ROOT_FOLDER + "dong.mp3";
      std::cout<<"LOCAL_AUDIO_FILE_PATH= "<< LOCAL_AUDIO_FILE_PATH<<std::endl;
      m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);

      if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
        if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
          while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
            ;
          }
          m_localMediaPlayer.m_mediaPlayer->stop();
        }
      }
    }else{
      system((APP_ROOT_FOLDER + SHELL_SCRIPT_WCD_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_DONG).c_str());
    }
  } else {
    system((APP_ROOT_FOLDER + SHELL_SCRIPT_CX_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_DONG).c_str());
  }
}

void *playMusic(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("playMusic thread: detachCode = %d\n", detachCode);

  sleep(1);
  std::string LOCAL_AUDIO_FILE_PATH = m_LocalMusicPath;
  std::cout<<"LOCAL_AUDIO_FILE_PATH = "<< LOCAL_AUDIO_FILE_PATH <<std::endl;
  m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);

  if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
    if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
      while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
        ;
      }
      m_localMediaPlayer.m_mediaPlayer->stop();
    }
  }

  pthread_exit(0);
  return 0;
}
//Damon add
void scanning_wifi() {
  std::string wifiTempData = STORE_ROOT_FOLDER + "temp_data";
  std::string wifiTempWifiInfo = STORE_ROOT_FOLDER + "temp_wifi_info";
  system(("iwlist wlan0 scanning > " + wifiTempData).c_str());
	system(("grep -e \"Encryption\" -e \"Signal level\" -e \"ESSID\" -e \"Scan completed\" " + wifiTempData + " > " + wifiTempWifiInfo).c_str());

	fstream fin(wifiTempWifiInfo.c_str());
	string content = "wifi_setting:scan_result:";
	string one_wifi_info = "";
	string line="";
	int count = -1;
	while(getline(fin,line)){
		count++;
		switch(count){
			case 1:
				{
				char *essid = strtok((char*)line.c_str(),"\"");
				essid = strtok(NULL,"\"");
				if(essid == NULL||strlen(essid)<=0)one_wifi_info += "ssid=,";
				else one_wifi_info += "ssid="+(string) essid + ",";
				}

				break;
			case 2:
				{
				char* encryption = strtok((char*) line.c_str(),":");
				encryption = strtok(NULL,":");
				encryption = strtok(encryption," ");
				if(strcmp(encryption,"on")==0) one_wifi_info +="encryption=1,";
				else one_wifi_info +="encryption=0,";
				}
				break;

			case 3:
				{
				if(line.find("=")==string::npos && line.find(":")==string::npos)break;
        if (line.find("=")!=string::npos) {
          char *level = strtok((char*)line.c_str(),"=");
  				level = strtok(NULL,"=");
  				level = strtok(NULL,"=");
  				level = strtok(level," ");
  				one_wifi_info += "level="+(string) level+",";
        }else if (line.find(":")!=string::npos) {
          char *level = strtok((char*)line.c_str(),":");
  				level = strtok(NULL,":");
  				level = strtok(NULL,":");
  				level = strtok(level," ");
  				one_wifi_info += "level="+(string) level+",";
        }
				}
				break;
		}

		if(count == 3){
			if(content.size()+one_wifi_info.size()>=1024){
				content.replace(content.size()-1,1,"");
				//send content.
				mWisCommon.sendUDPDataToSender(mSenderIPAddress,content);
				content = "wifi_setting:scan_result:";
				content += one_wifi_info;
				one_wifi_info = "";

			}else{
				content += one_wifi_info;
				one_wifi_info = "";

			}
      content.replace(content.size()-1,1,";");
			count = 0;
		}
	}
	fin.close();
	//system(("rm " + wifiTempData).c_str());
	//system(("rm " + wifiTempWifiInfo).c_str());
	content.replace(content.size()-1,1,"");
	if(content.size()>=26){
		mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
		isScanningWifiFinish = true;
	}
}
//Damon end
void *wifiSetting(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("wifiSetting thread: detachCode = %d\n", detachCode);

  //std::string wifiInfo = (char *)argv;
  printf("wifiSetting data: %s\n", wifiInfoData.c_str());
  char *p = strtok((char *)wifiInfoData.c_str(),",");

  char ssid[40], password[30];
  int encryption = -1;
  memset(ssid,0x00,40);
  memset(password,0x00,30);

  while(p != NULL){
    printf("temp p = %s\n", p);
    char key[30];
    char value[30];
    memset(key,0x00,30);
    memset(value,0x00,30);
    mWisCommon.subString(p, key, value);

    if (strcmp(key,"ssid") == 0) {
      strcpy(ssid, value);
    }else if (strcmp(key,"encryption") == 0) {
      encryption = atoi(value);
    }else if (strcmp(key, "password") == 0) {
      strcpy(password, value);
    }

    p=strtok(NULL, ",");
  }
  printf("ssid = %s, encryption = %d, password = %s\n", ssid, encryption, password);

  FILE *fInfo = fopen("/data/misc/wifi/wpa_supplicant.conf","w");
  fputs("# Only WPA-PSK is used. Any valid cipher combination is accepted.\n", fInfo);
  fputs("ctrl_interface=/var/run/wpa_supplicant\n", fInfo);
  fputs("\n", fInfo);
  fputs("network={\n",fInfo);
  switch (encryption) {
    case 0:  // None
       fputs("\tssid=\"",fInfo);
       fputs(ssid,fInfo);
       fputs("\"\n",fInfo);
       fputs("\tscan_ssid=1\n",fInfo);  // for hidden SSID
       fputs("\tkey_mgmt=NONE\n",fInfo);
     break;
    case 1:  // WPA/WPA2 PSK
       fputs("\tssid=\"",fInfo);
       fputs(ssid,fInfo);
       fputs("\"\n",fInfo);

       fputs("\tscan_ssid=1\n",fInfo);  // for hidden SSID

       fputs("\tpsk=\"",fInfo);
       fputs(password,fInfo);
       fputs("\"\n",fInfo);
     break;
    case 2:  // WEP: reserved
     break;
  }
  fputs("}\n",fInfo);
  fclose(fInfo);
  //system("reboot");

  if (mWisCommon.isWpaSupplicantProcessExist()) {
    system((APP_ROOT_FOLDER + SHELL_SCRIPT_WIFI_RECONFIGURE).c_str());
  } else {
    system("wpa_supplicant -B -iwlan0 -c /data/misc/wifi/wpa_supplicant.conf");
  }

  if (!isNetworkChecking) {
    isNetworkChecking = true;
    mWisCommon.lightLED(LED_TYPE_SYSTEM_ERROR);

    mWisCommon.checkIPAddress();
    mWisCommon.turnOffSoftAPMode();
    restoreDefaultLED();
    isSoftAPModeOn = false;

    // Ping a address
    mWisCommon.sysLocalTime(">>> Start to ping");
    mWisCommon.pingAddress(pingAddress);
    mWisCommon.sysLocalTime("*** Ping operation is done");

    // Check NTP server
    if (!isNtpSynced) {
      mWisCommon.sysLocalTime(">>> Start to sync NTP server");
      mWisCommon.syncNtpServer(syncNTPServer);
      mWisCommon.sysLocalTime("*** Sync NTP server done");
      isNtpSynced = true;
    }

    if (isAVSReady) {
      restoreDefaultLED();
    }

    if (!isMulticastEnabled) {
      pthread_t findMeThread;
      int findMeError = pthread_create(&findMeThread, NULL, findMe, NULL);
      printf("\t%s: findMeError = %d\n", APP_TAG.c_str(), findMeError);
    }
    isNetworkChecking = false;
  }

  pthread_exit(0);
  return 0;
}

void openSoftAPMode(){
  struct ifaddrs *ifap, *ifa;
  int family;
  getifaddrs (&ifap);
  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
      family = ifa->ifa_addr->sa_family;
      if (family == AF_PACKET) {
          //printf("interface name is: %s, running: %d\n", ifa->ifa_name, ifa->ifa_flags);
          if (strcmp(ifa->ifa_name, "softap.0") == 0) {
            isSoftAPModeOn = true;
            break;
          }
      }
  }
  freeifaddrs(ifap);

  if (isSoftAPModeOn) {
      printf("---> openSoftAPMode:: Soft AP mode is on, to turn off it!\n");
      mWisCommon.turnOffSoftAPMode();
      restoreDefaultLED();
      isSoftAPModeOn = false;
  } else {
    printf("---> openSoftAPMode:: Soft AP mode is off, to turn on it!\n");
    printf("\t%s: Reset wifi to soft AP mode...\n", APP_TAG.c_str());
    system((APP_ROOT_FOLDER + SHELL_SCRIPT_START_SOFT_AP_MODE).c_str());
    //usleep(4*1000*1000);
    //system("hostapd_cli wps_pbc"); // WPS active
    printf("\n\n\n\t***** %s: Success to open Soft AP mode, please select \"nut2_XXXXXXXX\" in your wifi list. *****\n\n\n", APP_TAG.c_str());
    mWisCommon.lightLED(LED_TYPE_HOTSPOT_MODE);
    isSoftAPModeOn = true;
  }
}

void readProductFile(){
    //execute avs make product data, if the product data has, not create
    std::ifstream inputFile(STORE_PRODUCT_DATA_FILE_PATH.c_str());
    if (!inputFile.good()) {
        std::cout << "Couldn't open product_data file! create it." << std::endl;
        mWisAvs.createVerifier();
    } else {
        std::cout << "the product data file exists,do not need to create new one!" << std::endl;
    }
    inputFile.close();

    std::string productId = mWisCommon.getValueWithKey(STORE_PRODUCT_DATA_FILE_PATH, CMD_AVS_SETTING_TAG_PRODUCT_ID);
    std::string productDSN = mWisCommon.getValueWithKey(STORE_PRODUCT_DATA_FILE_PATH, CMD_AVS_SETTING_TAG_PRODUCT_DSN);
    std::string codeVerifier = mWisCommon.getValueWithKey(STORE_PRODUCT_DATA_FILE_PATH, CMD_AVS_SETTING_TAG_CODE_VERIFIER);
    std::string codeChallenge = mWisCommon.getValueWithKey(STORE_PRODUCT_DATA_FILE_PATH, CMD_AVS_SETTING_TAG_CODE_CHALLENGE);
    std::string deviceOnline = isAVSReady ? std::to_string(1) : std::to_string(0);
    std::string userName = mWisCommon.getValueWithKey(STORE_AVS_ACCOUNT_FILE_PATH, CMD_AVS_SETTING_TAG_USER_NAME);
    std::string userEmail = mWisCommon.getValueWithKey(STORE_AVS_ACCOUNT_FILE_PATH, CMD_AVS_SETTING_TAG_USER_EMAIL);
    if (userName.length() <= 0) {
      userName = "Unknown";
    }
    if (userEmail.length() <= 0) {
      userEmail = "Unknown";
    }

    std::string sendContent = CMD_AVS_SETTING + ":" + CMD_AVS_SETTING_TAG_AVS_DATA + ":";
    sendContent += CMD_AVS_SETTING_TAG_PRODUCT_ID + "=" + productId;
    sendContent += ",";
    sendContent += CMD_AVS_SETTING_TAG_PRODUCT_DSN + "=" + productDSN;
    sendContent += ",";
    // sendContent += CMD_AVS_SETTING_TAG_CODE_VERIFIER + "=" + codeVerifier;
    sendContent += CMD_AVS_SETTING_TAG_CODE_CHALLENGE + "=" + codeChallenge;
    sendContent += ",";
    sendContent += CMD_AVS_SETTING_TAG_DEVICE_ONLINE + "=" + deviceOnline;
    sendContent += ",";
    sendContent += CMD_AVS_SETTING_TAG_USER_NAME + "=" + userName;
    sendContent += ",";
    sendContent += CMD_AVS_SETTING_TAG_USER_EMAIL + "=" + userEmail;
    mWisCommon.sendUDPDataToSender(mSenderIPAddress, sendContent);
}

void deleteAVSAccountInfo(){
  printf("\t\t\t\t start delete auth code and refresh token file\n");
  int result = system(("rm " + STORE_AUTH_CODE_FILE_PATH).c_str());
  printf("delete %s file %d\n", STORE_AUTH_CODE_FILE_PATH.c_str(), result);
  result = system(("rm " + STORE_REFRESH_TOKEN_FILE_PATH).c_str());
  printf("delete %s file %d\n", STORE_REFRESH_TOKEN_FILE_PATH.c_str(), result);
  result = system(("rm " + STORE_PRODUCT_DATA_FILE_PATH).c_str());
  printf("delete %s file %d\n", STORE_PRODUCT_DATA_FILE_PATH.c_str(), result);
  result = system(("rm " + STORE_AVS_ACCOUNT_FILE_PATH).c_str());
  printf("delete %s file %d\n", STORE_AVS_ACCOUNT_FILE_PATH.c_str(), result);
}

void sendAvsLogout(){
    setvbuf(stdout, NULL, _IONBF, 0);
    fflush(stdout);

    mWisCommon.sendUDPDataToSender(mSenderIPAddress, CMD_AVS_SETTING + ":" + CMD_AVS_SETTING_ACTION_LOGOUT_DONE);

    exitSva = true;
    exitAVS = true;
    m_wisStateTrigger.notify_all();
    printf("*******************>>>>>  Please login your amazon account!!!  <<<<<*******************\n");

    isAVSReady = false;
}

void launchSVAandAVS(){
  if (access(configPath.c_str(),F_OK) != -1) { // File exist
    exitSva = false;
    exitAVS = false;
    isSVAReady = false;
    isAVSReady = false;
    isSVAFinish = true;
    isAVSFinish = true;
    if (wakeWordDetector == WAKE_WORD_DETECTOR_SVA) {
      alexaClientSDK::integration::test::launchSVA();
    }
    int argc = 1;
    char *param[] = {(char *)"Nut2AVSTest"};
    ::testing::InitGoogleTest(&argc, param);
    mWisCommon.sysLocalTime("**********  AVS_SDK\tStart test AVS  **********");  // King add
    RUN_ALL_TESTS();
    if (wakeWordDetector == WAKE_WORD_DETECTOR_SVA) {
      printf("*******************>>>>>  SVA & AVS Has done!!!  <<<<<*******************\n");
    } else if (wakeWordDetector == WAKE_WORD_DETECTOR_SENSORY) {
      printf("*******************>>>>>  AVS Has done!!!  <<<<<*******************\n");
    }

    sleep(1);
    launchSVAandAVS();
  } else {
    printf("*******************>>>>>  Please login your amazon account first!!!  <<<<<*******************\n");
  }
}

void *executeAvsCreateProductData(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("executeAvsCreateProductData thread: detachCode = %d\n", detachCode);

  launchSVAandAVS();
  pthread_exit(0);
  return 0;
}

void *executeAvsGetAccessToken(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("executeAvsGetAccessToken thread: detachCode = %d\n", detachCode);

  sleep(1);
  printf("AVS get access token: start read auth code \n");
  mWisAvs.readAuthCodeFromFile();

  if (access(configPath.c_str(),F_OK)!=-1) {  // File exist
    pthread_t svaAndavsAction;
    int svaAndavsActionResult = pthread_create(&svaAndavsAction, NULL, executeAvsCreateProductData,NULL);
    printf("\t%s: start SVA and AVS app. errorCode = %d\n", APP_TAG.c_str(), svaAndavsActionResult);
  }

  pthread_exit(0);
  return 0;
}

void *volUpKeyDetect(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("volUpKeyDetect thread: detachCode = %d\n", detachCode);

  int keys_fd;
  struct input_event t;

  struct timeval keyPressedTime;
  struct timeval keyReleasedTime;

  keys_fd = open ("/dev/input/event1", O_RDONLY);  // event0 --> 114 --> vol down; event1 --> (766, 528, 115)  --> up to down
  if (keys_fd <= 0) {
    printf ("------------> open /dev/input/event1 device error!\n");
    return 0;
  }

  while (1) {
    if (read (keys_fd, &t, sizeof (t)) == sizeof (t)) {
      if (t.type == EV_KEY && t.code == 115) {  // Volume UP key
        if (t.value == 0 || t.value == 1) {
          printf ("------------> key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
          if (t.value) {  // Pressed
            isVolUpLongPressed = false;
            isVolDownLongPressed = false;
            gettimeofday(&keyPressedTime,NULL);
          } else {  // Released
            gettimeofday(&keyReleasedTime,NULL);
            printf("------------> pressed time is %ld seconds\n", (keyReleasedTime.tv_sec-keyPressedTime.tv_sec));
            printf("event1 keycode = %d\n", t.code );
            if ((keyReleasedTime.tv_sec - keyPressedTime.tv_sec) >= 3) {
              if(t.code==115){
                isVolUpLongPressed = true;
                if (isVolDownLongPressed) {
                  openSoftAPMode();
                }
                // King debug
                //break;
              }
            }
            usleep(400*1000);  // micro_seconds
            sendCurrentMediaVolume();
          }
        }
      }
    }
  }
  close (keys_fd);

  pthread_exit(0);
  return 0;
}

void *volDownKeyDetect(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("volDownKeyDetect thread: detachCode = %d\n", detachCode);

  int keys_fd;
  struct input_event t;

  struct timeval keyPressedTime;
  struct timeval keyReleasedTime;

  keys_fd = open ("/dev/input/event0", O_RDONLY);  // event0 --> 114 --> vol down; event1 --> (766, 528, 115)  --> up to down
  if (keys_fd <= 0) {
    printf ("------------> open /dev/input/event0 device error!\n");
    return 0;
  }

  while (1) {
    if (read(keys_fd, &t, sizeof (t)) == sizeof (t)) {
      if (t.type == EV_KEY && t.code == 114) {  // Volume down key
        if (t.value == 0 || t.value == 1) {
          printf ("------------> key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
          if (t.value) {  // Pressed
            isVolUpLongPressed = false;
            isVolDownLongPressed = false;
            gettimeofday(&keyPressedTime,NULL);
          } else {  // Released
            gettimeofday(&keyReleasedTime,NULL);
            printf("------------> pressed time is %ld seconds\n", (keyReleasedTime.tv_sec-keyPressedTime.tv_sec));
            printf("event0 keycode = %d\n", t.code );
            if ((keyReleasedTime.tv_sec - keyPressedTime.tv_sec) >= 3) {
              if(t.code==114){
                isVolDownLongPressed = true;
                if (isVolUpLongPressed) {
                  openSoftAPMode();
                }
                // King debug
                //break;
              }
            }
            usleep(400*1000);  // micro_seconds
            sendCurrentMediaVolume();
          }
        }
      }
    }
  }
  close (keys_fd);

  pthread_exit(0);
  return 0;
}

void *micMuteKeyDetect(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("micMuteKeyDetect thread: detachCode = %d\n", detachCode);

  int keys_fd;
  struct input_event t;

  keys_fd = open ("/dev/input/event1", O_RDONLY);  // event0 --> 114 --> vol down; event1 --> (766, 528, 115)  --> up to down
  if (keys_fd <= 0) {
    printf ("------------> open /dev/input/event1 device error!\n");
    return 0;
  }

  while (1) {
    if (read (keys_fd, &t, sizeof (t)) == sizeof (t)) {
      if (t.type == EV_KEY && t.code == 194) {  // Mute key
        printf ("------------> key %d, value = %d\n", t.code, t.value);
        if (isPlayEnabled) {
          if (t.value == 1) { // Mute
            if (mWisCommon.getCurrentSKU() == SKU_WCD) {
              if (wcdPlaySoundCueType == WCD_PLAY_SOUND_CUE_TYPE_MEDIA_PLAYER) {
                std::string LOCAL_AUDIO_FILE_PATH;
                LOCAL_AUDIO_FILE_PATH = APP_ROOT_FOLDER + "med_state_privacy_mode_on_to_off.wav";
                std::cout<<"LOCAL_AUDIO_FILE_PATH=" << LOCAL_AUDIO_FILE_PATH <<std::endl;
                m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);

                if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
                  if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
                    while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
                      ;
                    }
                    m_localMediaPlayer.m_mediaPlayer->stop();
                  }
                }
              }else{
                system((APP_ROOT_FOLDER + SHELL_SCRIPT_WCD_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_MIC_OFF).c_str());
              }
            } else {
              system((APP_ROOT_FOLDER + SHELL_SCRIPT_CX_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_MIC_OFF).c_str());
            }
          } else { // unMute
            if (mWisCommon.getCurrentSKU() == SKU_WCD) {
              if (wcdPlaySoundCueType == WCD_PLAY_SOUND_CUE_TYPE_MEDIA_PLAYER) {
                std::string LOCAL_AUDIO_FILE_PATH;
                LOCAL_AUDIO_FILE_PATH = APP_ROOT_FOLDER + "med_state_privacy_mode_off_to_on.wav";
                std::cout<<"LOCAL_AUDIO_FILE_PATH=" << LOCAL_AUDIO_FILE_PATH <<std::endl;
                m_localMediaPlayer.setMediaPlayerSource(LOCAL_AUDIO_FILE_PATH);

                if(MediaPlayerStatus::FAILURE != m_localMediaPlayer.m_mediaPlayer->play()){
                  if(m_localMediaPlayer.m_playerObserver->waitForPlaybackStarted()){
                    while(!m_localMediaPlayer.m_playerObserver->waitForPlaybackFinished()){
                      ;
                    }
                    m_localMediaPlayer.m_mediaPlayer->stop();
                  }
                }
              }else{
                system((APP_ROOT_FOLDER + SHELL_SCRIPT_WCD_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_MIC_ON).c_str());
              }
            } else {
              system((APP_ROOT_FOLDER + SHELL_SCRIPT_CX_PLAY_SOUND_CUE + " " + SOUND_CUE_TYPE_MIC_ON).c_str());
            }
          }
        }
      }
    }
  }
  close (keys_fd);

  pthread_exit(0);
  return 0;
}

void *checkNetworkState(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("checkNetworkState thread: detachCode = %d\n", detachCode);

  isNetworkChecking = true;
  mWisCommon.lightLED(LED_TYPE_SYSTEM_ERROR);

  // Check WLAN module
  mWisCommon.sysLocalTime(">>> Start to wait Wifi module ready");
  mWisCommon.waitWlanReady();
  mWisCommon.sysLocalTime("*** Wifi module is ready");

  // Check IP address
  mWisCommon.sysLocalTime(">>> Start to check IP address");
  mWisCommon.checkIPAddress();
  mWisCommon.sysLocalTime("*** Get IP address");

// King debug
  // Ping a address
  mWisCommon.sysLocalTime(">>> Start to ping");
  mWisCommon.pingAddress(pingAddress);
  mWisCommon.sysLocalTime("*** Ping operation is done");

  // Check NTP server
  if (!isNtpSynced) {
    mWisCommon.sysLocalTime(">>> Start to sync NTP server");
    mWisCommon.syncNtpServer(syncNTPServer);
    mWisCommon.sysLocalTime("*** Sync NTP server done");
  }

  mWisCommon.turnOffSoftAPMode();
  isSoftAPModeOn = false;

  if (isAVSReady) {
    restoreDefaultLED();
  }

  if (!isMulticastEnabled) {
    pthread_t findMeThread;
    int findMeError = pthread_create(&findMeThread, NULL, findMe, NULL);
    printf("\t%s: findMeError = %d\n", APP_TAG.c_str(), findMeError);
  }

  pthread_t avsProductDataThread;
  int avsProductDataThreadError = pthread_create(&avsProductDataThread, NULL, executeAvsCreateProductData,NULL);
  printf("\t%s: avsProductDataThreadError = %d\n", APP_TAG.c_str(), avsProductDataThreadError);
  isNetworkChecking = false;
  isWifiConnected = true;

  pthread_exit(0);
  return 0;
}

void *lightLEDWhenReceivedPhoneCmd(void *argv){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("lightLEDWhenReceivedPhoneCmd thread: detachCode = %d\n", detachCode);

  system("echo 255 > /sys/devices/soc/78b5000.i2c/i2c-1/1-0032/leds/LED2:B/brightness");
  sleep(1);
  system("echo 0 > /sys/devices/soc/78b5000.i2c/i2c-1/1-0032/leds/LED2:B/brightness");

  pthread_exit(0);
  return 0;
}

Aws::Transfer::DownloadProgressCallback progressCallBack = [](const TransferManager*, const std::shared_ptr<const TransferHandle>& handle)

	{
     int progress = static_cast<int>(handle->GetBytesTransferred() * 100.0 / handle->GetBytesTotalSize());
 	   if(progress > downloadProgress){
       downloadProgress = progress;
       std::cout << "\r" << "<AWS DOWNLOAD> Download Progress: " << progress << " Percent " << handle->GetBytesTransferred() << " bytes\n";
       printf("---> update download progress to: %d \n", progress);
       std::string content;
       content = "ota:upgrade_progress="+std::to_string(progress);
       mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
     }
	};

void *checkOTAUpdate(void *argv) {
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("checkPlayingEventTimeout thread: detachCode = %d\n", detachCode);

  std::string currentBuildVersion = mWisCommon.getCurrentImageBuildVersion();
  std::string currentBuildDate = mWisCommon.getCurrentImageBuildDate();
  std::string deviceId = mWisCommon.getDeviceName();
  std::string newBuildVersion, newBuildDate;
  Check_Result_Code code = mWisAws.sendCheckOTAUpgrade(currentBuildVersion,deviceId);
  if(code == NEED_UPDATE){
    newBuildVersion = mWisAws.getNewVersion();
    newBuildDate = mWisAws.getNewBuildDate();
  }else if(code == NO_NEED_UPDATE){
    std::cout << "check success! current version is the latest version" << '\n';
    newBuildDate = currentBuildDate;
    newBuildVersion = currentBuildVersion;
  }else if(code == CHECK_FAILED){
    std::cout << "check failed" << '\n';
    newBuildDate = currentBuildDate;
    newBuildVersion = currentBuildVersion;
  }
  std::string content;
  content = "ota:cur_ver=" + currentBuildDate + ":" + currentBuildVersion + ",new_ver=" + newBuildDate + ":" + newBuildVersion;
  mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);

  pthread_exit(0);
  return 0;
}

void *OTAUpgrade(void *argv) {
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("checkPlayingEventTimeout thread: detachCode = %d\n", detachCode);

  Download_Result_Code downloadResult = DOWNLOAD_FAIL;
  downloadProgress = -1;
  if(mWisAws.isDownloadSuccess()){
    std::cout << "Latest image has been downloaded" << '\n';
    downloadResult = DOWNLOAD_SUCCESS;
    //std::string content;
    //content = "ota:upgrade_progress=100";
    //mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
  }else{
    downloadResult = mWisAws.downloadImage(OTA_LOCAL_IMAGE_FILE,progressCallBack);
  }

  if (downloadResult == DOWNLOAD_SUCCESS) {
    bool allowUpGrade = false;
    string batteryStatus = mWisCommon.getBatteryStatus();
    string batteryCapacity = mWisCommon.getBatteryCapacity();

    if(batteryStatus == "Full"||batteryStatus == "Charging")
      allowUpGrade = true;
    else{
      if(atoi((const char*)batteryCapacity.c_str())>50) allowUpGrade = true;
    }

    if(allowUpGrade){
      mWisAws.addLogMessage("upgrade successfully!");
      mWisAws.sendLog(WisAWS::LOG_INFO);
      string content = "ota:start_upgrade";
      mWisCommon.sendUDPDataToSender(mSenderIPAddress,content);
      system((APP_ROOT_FOLDER + SHELL_SCRIPT_OTA_UPGRADE).c_str());
    }else{
      string content = "ota:low_battery";
      mWisCommon.sendUDPDataToSender(mSenderIPAddress,content);
    }
  }else if(downloadResult == DOWNLOAD_CANCEL){
      //TO_DO
      //std::string content;
      //content = "ota:upgrade_progress=0";
      //mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
  }else{
      //可能网络环境比较差导致没有连接到s3服务器.
    string content = "ota:downloadfail";
    mWisCommon.sendUDPDataToSender(mSenderIPAddress,content);
  }
  downloadProgress = -1;
  pthread_exit(0);
  return 0;
}

void *OTACancel(void * args){
  int detachCode = pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放
  printf("checkPlayingEventTimeout thread: detachCode = %d\n", detachCode);

  if(mWisAws.isDownloading()){
    mWisAws.cancelDownload();
  }else{
    std::cout << "User Canceled!" << '\n';
    mWisAws.addLogMessage("User Canceled!");
    mWisAws.sendLog(WisAWS::LOG_INFO);
  }

  pthread_exit(0);
  return 0;
}

void getTestParamsFromSettings(){
  if (access(COMPANION_APP_SETTINGS_FILE_PATH.c_str(),F_OK) != -1) {  // File exist
    mWisCommon.getSettingParametersFromCompanionConfig(wakeWordDetector, recordingSource, enablePreRecSoundPrompt, enablePostRecSoundPrompt,
                  enableRecLEDPrompt, svaKeywordConfidenceLevel, svaNumberOfKeywords, svaNumberOfUsersPerKeyword, recordingTimeout, waitDirectiveTimeout, waitForResponseTimeout,
                  syncNTPServer, pingAddress, enableSoundWhenReceivedPhoneCmd,
                  otaFTPServer, otaFTPUserName, otaFTPPassword, otaFTPRemoteVersionFilePath);
  }else{
    mWisCommon.getSettingParametersFromDefaultConfig(wakeWordDetector, recordingSource, enablePreRecSoundPrompt, enablePostRecSoundPrompt,
                  enableRecLEDPrompt, svaKeywordConfidenceLevel, svaNumberOfKeywords, svaNumberOfUsersPerKeyword, recordingTimeout, waitDirectiveTimeout, waitForResponseTimeout,
                  syncNTPServer, pingAddress, enableSoundWhenReceivedPhoneCmd,
                  otaFTPServer, otaFTPUserName, otaFTPPassword, otaFTPRemoteVersionFilePath);
  }
}

void sendSettingsUpdatedEvent(char *locale){
  //  mWisAvs.sendSettingsUpdatedEvent(locale);
  mWisAvs.m_settingsObject->changeSetting("locale",locale);
}

void sendPlaybackControllerEvent(std::string playbackAction){
  if (playbackAction.compare("play") == 0) {  // equal to
      // sendDataEvent = avsWisCommon.replace_all(playbackControllerEvent,"playbackAction", "PlayCommandIssued");
      mWisAvs.m_playbackController->playButtonPressed();
  } else if (playbackAction.compare("pause") == 0) {
      // sendDataEvent = avsWisCommon.replace_all(playbackControllerEvent,"playbackAction", "PauseCommandIssued");
      mWisAvs.m_playbackController->pauseButtonPressed();
  } else if (playbackAction.compare("previous") == 0) {
      // sendDataEvent = avsWisCommon.replace_all(playbackControllerEvent,"playbackAction", "PreviousCommandIssued");
      mWisAvs.m_playbackController->previousButtonPressed();
  } else if (playbackAction.compare("next") == 0) {
      // sendDataEvent = avsWisCommon.replace_all(playbackControllerEvent,"playbackAction", "NextCommandIssued");
      mWisAvs.m_playbackController->nextButtonPressed();
  }
}

//Bob add for fm 20170821
void main_ps_name_callback(int ps_name_len, char *ps_name){
  char g_ps_name[96] = "\0";
	memcpy(g_ps_name, ps_name, ps_name_len);
	printf("main: station name: %s\n", g_ps_name);
}

void main_searchex_callback(int search_freq){
	if (search_freq != 0) {
    fm_searchStations.push_back(search_freq);
		printf("main: searching station %d in freq %d. \n", fm_searchStations.size()-1 , search_freq);
	} else {
		printf("main: search complete and get %d stations. \n", fm_searchStations.size());
    int current_volume = mWisFM.getVolume();
    std::string content;
    content = "fm:volume="+std::to_string(current_volume);

    mWisFM.mLastFMFrequency=fm_searchStations[fm_searchStations.size()-1];

    content = content + ",searched_stations=";
    std::sort(fm_searchStations.begin(),fm_searchStations.end(),less<int>()); // Sort vector
    size_t m_stationsSize = fm_searchStations.size();
    for (size_t i =0; i < m_stationsSize; i ++) {
        int station = fm_searchStations[i];
        if (i==0) {
          content = content + to_string(station);
        }else{
          content = content + "," + to_string(station);
        }
    }
    mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
	}
}
//Bob end 20170821

void *findMe(void *argv) {
    struct sockaddr_in peeraddr;
    struct in_addr ia;
    int sockfd;
    unsigned int socklen;
    struct hostent *group;
    struct ip_mreq mreq;

    /* 创建 socket 用于UDP通讯 */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
      perror("socket\n");
      exit(errno);
    }
    printf("socket created success!!!\n");
    /* 设置要加入组播的地址 */
    bzero(&mreq, sizeof(struct ip_mreq));
    //you should give me a group address, 224.0.0.0-239.255.255.255
    if ((group = gethostbyname(UDP_MULTICAST_IP_ADDRESS.c_str())) == (struct hostent *) 0) {
        perror("gethostbyname");
        exit(errno);
    }

    bcopy((void *) group->h_addr, (void *) &ia, group->h_length);
    printf("Muticast address is: %s\n",inet_ntoa(ia));
    /* 设置组地址 */
    bcopy(&ia, &mreq.imr_multiaddr.s_addr, sizeof(struct in_addr));

    /* 设置发送组播消息的源主机的地址信息 */
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    // reuse socket port
    int on=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int))<0) {
      perror("findMe:: setsockopt");
    }

    /* 把本机加入组播地址，即本机网卡作为组播成员，只有加入组才能收到组播消息 */
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,sizeof(struct ip_mreq)) ==-1) {
      perror("findMe:: setsockopt");
      exit(errno);
    }

    socklen = sizeof(struct sockaddr_in);
    memset(&peeraddr, 0, socklen);
    peeraddr.sin_family = AF_INET;
    peeraddr.sin_port = htons(UDP_RECEIVE_MULTICAST_PORT);
    if (inet_pton(AF_INET, UDP_MULTICAST_IP_ADDRESS.c_str(), &peeraddr.sin_addr) <= 0) {
        printf("Wrong dest IP address!\n");
        exit(0);
    }

    /* 绑定组播地址的端口和IP信息到socket上 */
    if (bind(sockfd, (struct sockaddr *) &peeraddr,sizeof(struct sockaddr_in)) == -1) {
      printf("Binded failure\n");
      exit(0);
    }else
      printf("binded success!!!\n");

    /* 循环接收网络上来的组播消息 */
    isMulticastEnabled = true;
    char receivedCmd[1024] = {0};
    while(1) {
        printf("\t%s: findMe:: waiting for new command...\n", APP_TAG.c_str());
        memset(receivedCmd,0x00,1024);
        //从广播地址接受消息
        int ret=recvfrom(sockfd, receivedCmd, 1024, 0, (struct sockaddr *) &peeraddr, &socklen);
        mSenderIPAddress = inet_ntoa(peeraddr.sin_addr);
        printf("findMe:: receive from %s\n",mSenderIPAddress.c_str());
        if(ret<=0) {
            printf("\tfindMe:: %s: read error...\n", APP_TAG.c_str());
            continue;
        } else {
            if (strlen(receivedCmd) <= 0) {
                continue;
            }

            if (enableSoundWhenReceivedPhoneCmd) {
               playWarnSound();
            } else {
               pthread_t lightLEDThread;
               pthread_create(&lightLEDThread, NULL, lightLEDWhenReceivedPhoneCmd,NULL);
            }
            printf("\t%s: findMe:: Run action with command...%s\n", APP_TAG.c_str(), receivedCmd);

            char *p = strtok(receivedCmd, ":");
            if (strcmp(p, CMD_FIND_DEVICES) == 0) {
              printf("\t%s: findMe:: find devices...\n", APP_TAG.c_str());
              // get Nut2 name
              std::string name=mWisCommon.getDeviceName();

              // get Nut2 IP
              std::string ip=mWisCommon.getIPAddress();

              // Send data:  device:name=NUT2_xxxxxxxx,ip=yyy.yyy.yyy.yyy
              std::string content;
              content = "device:name="+name+",ip="+ip;
              mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
            }
        }
    }
    close(sockfd);
}

void sig_handler(int signo) {
  printf("isNetworkChecking = %d\n", isNetworkChecking);
  if (!isNetworkChecking) {
    if (signo == SIGUSR1) {
      printf("received SIGUSR1\n");
      bool connected = false;
      // get Nut2 IP
      std::string ip=mWisCommon.getIPAddress();
      if (ip.empty()) {
        printf("Wifi disconnected...\n");
        connected = false;
      } else {
        printf("Wifi connected...\n");
        connected = true;
      }
      if (isWifiConnected != connected) {
        isWifiConnected = connected;
        if (isWifiConnected) {
          if (isAVSReady) {
            restoreDefaultLED();
          }
        } else {
          mWisCommon.lightLED(LED_TYPE_SYSTEM_ERROR);
        }
      }
    }
  }
}

int main(int argc, char **argv) {
  const std::string versionID = "v3.3.5";
  const std::string buildDate = "20171125";
  const std::string avsSDKVersion = "1.1.0";
  printf("************** %s version is: %s build on %s, AVS sdk version: %s **************\n", APP_TAG.c_str(), versionID.c_str(), buildDate.c_str(), avsSDKVersion.c_str());
  if (argc > 1) {
    if(strcmp(argv[1], "-version") == 0
        || strcmp(argv[1], "-v") == 0
        || strcmp(argv[1], "version") == 0
        || strcmp(argv[1], "--version") == 0) {
      printf("%s version is: %s build on %s, AVS sdk version: %s\n\n", APP_TAG.c_str(), versionID.c_str(), buildDate.c_str(), avsSDKVersion.c_str());
      return 0;
    }
  }

  // enable Wifi connection signal
  if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
    printf("\ncan't catch Wifi connection SIGUSR1\n");
  } else {
    printf("\n\n\t Registered Wifi signal!\n\n");
  }

  system("mount -o remount,rw /");
  system(("mkdir "+STORE_ROOT_FOLDER).c_str());
  mWisVolumeControl.deleteVolumeBackupFile();
  getTestParamsFromSettings();

  if (access("/data/misc/wifi/wpa_supplicant.conf",F_OK)!=-1) {  // File exist
    printf("\t%s: Wifi configure file is exist, so skip Soft AP mode......\n", APP_TAG.c_str());

    pthread_t checkNetworkStateThread;
    int checkNetworkStateThreadError = pthread_create(&checkNetworkStateThread, NULL, checkNetworkState,NULL);
    printf("\t%s: checkNetworkStateThreadError = %d\n", APP_TAG.c_str(), checkNetworkStateThreadError);
  } else {  // File not exist
    // Check WLAN module
    mWisCommon.sysLocalTime(">>> Start to wait Wifi module ready");
    mWisCommon.waitWlanReady();
    mWisCommon.sysLocalTime("*** Wifi module is ready");

    printf("\t%s: Wifi configure file is not exist, so open Soft AP mode......\n", APP_TAG.c_str());
    openSoftAPMode();
  }

  // init
  m_localMediaPlayer.SetUp();

  // Key detect
  printf("------------> Please press and hold VolUp and VolDown at the same time for more than 3 seconds to reboot device to Wifi AP mode...\n");
  pthread_t volUpKeyDetectThread, volDownKeyDetectThread, micMuteKeyDetectThread;
  int volUpKeyDetectError = pthread_create(&volUpKeyDetectThread, NULL, volUpKeyDetect,NULL);
  int volDownKeyDetectError = pthread_create(&volDownKeyDetectThread, NULL, volDownKeyDetect,NULL);
  int micMuteKeyDetectError = pthread_create(&micMuteKeyDetectThread, NULL, micMuteKeyDetect, NULL);
  printf("\t%s: volUpKeyDetectError = %d, volDownKeyDetectError = %d, micMuteKeyDetectError = %d\n", APP_TAG.c_str(), volUpKeyDetectError, volDownKeyDetectError, micMuteKeyDetectError);

  // get AVS request count
  mWisCommon.getAVSRequestResult(totalAVSRequestNum, failAVSRequestNum);

  // Receive command from Phone
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(UDP_RECEIVE_IP_PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int sock;
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("UDP IP:: socket fail");
    exit(1);
  }
  // reuse socket port
  int on=1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int))<0) {
    perror("UDP IP:: setsockopt");
  }
  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("UDP IP:: bind fail");
    exit(1);
  }
  char receivedCmd[1024] = {0};
  struct sockaddr_in clientAddr;
  socklen_t len = sizeof(clientAddr);
  char *p;
  while (1) {
    printf("\t%s: UDP IP:: waiting for new command...\n", APP_TAG.c_str());
    memset(receivedCmd,0x00,1024);
    int ret=recvfrom(sock, receivedCmd, 1024, 0, (struct sockaddr*)&clientAddr, &len);
    mSenderIPAddress = inet_ntoa(clientAddr.sin_addr);
    mWisCommon.sysLocalTime(("UDP IP::receive from " + mSenderIPAddress).c_str());
    if(ret<=0) {
        printf("\t%s: UDP IP:: read error...\n", APP_TAG.c_str());
        continue;
    } else {
        if (strlen(receivedCmd) <= 0) {
            continue;
        }

        if (enableSoundWhenReceivedPhoneCmd) {
           playWarnSound();
        } else {
           pthread_t lightLEDThread;
           pthread_create(&lightLEDThread, NULL, lightLEDWhenReceivedPhoneCmd,NULL);
        }
        printf("\t%s: UDP IP:: Run action with command...%s\n", APP_TAG.c_str(), receivedCmd);

        p = strtok(receivedCmd, ":");
        if (strcmp(p, CMD_PLAY_LOCAL_MUSIC) == 0) {
          if (m_localMediaPlayer.m_playerObserver->isMediaPlayerPlaying()) {
            printf("\t%s: stop music...\n", APP_TAG.c_str());
            if (!enableSoundWhenReceivedPhoneCmd) {
                m_localMediaPlayer.m_mediaPlayer->stop();
                printf("\t%s: stop music done...\n", APP_TAG.c_str());
            }
          }else{
            printf("\t%s: play music start...\n", APP_TAG.c_str());
            pthread_t playMusicThread;
            int playMusicThreadError = pthread_create(&playMusicThread, NULL, playMusic,NULL);
            printf("\t%s: playMusicThreadError = %d\n", APP_TAG.c_str(), playMusicThreadError);
          }
        } else if(strcmp(p, CMD_PLAY_MUSIC) == 0) { // Play music
            printf("\t%s: Play music control...\n", APP_TAG.c_str());
            p = strtok(NULL, ":");
            std::string playbackAction = p;
            if (playbackAction.find("get_current_status", 0) == 0) {
              // get AudioPlayer play status
              printf("AudioPlayer isAudioPlayerPlaying: %d\n", isAudioPlayerPlaying);

              // get music volume
              int current_volume = getCurrentMediaVolume();

              // Send data:  play_music:status=0,volume=2
              std::string content;
              content = "play_music:status="+(isAudioPlayerPlaying ? std::to_string(1) : std::to_string(0))+",volume="+std::to_string(current_volume);
              mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
            } else if (playbackAction.find("play", 0) == 0
                        || playbackAction.find("pause", 0) == 0
                        || playbackAction.find("previous", 0) == 0
                        || playbackAction.find("next", 0) == 0) {
              printf("To execute play action : %s\n", playbackAction.c_str());
              // mWisCommon.sysLocalTime("get context start!");
              // mWisAvs.m_contextManager->getContext(m_contextRequester);
              // m_contextRequester->waitForContext(DEFAULT_TIMEOUT);
              // std::string context = m_contextRequester->getContextString();
              // mWisCommon.sysLocalTime("get context done!");
              // printf("context content is : %s\n\n", context.c_str());

              // mWisAvs.sendPlaybackControllerEvent(context, playbackAction);
              sendPlaybackControllerEvent(playbackAction);
            } else if (playbackAction.find("set_volume", 0) == 0) {
              printf("To execute play action  : %s\n", playbackAction.c_str());
              int startIndex = playbackAction.find("=") + 1;
              std::string volume = playbackAction.substr(startIndex, strlen(playbackAction.c_str()) - startIndex);
              std::string command = "volume_control set "+volume;
              std::cout << "set mediaPlayer volume command: " << command << std::endl;
              system(command.c_str());
              getCurrentMediaVolume();
            } else if (playbackAction.find("get_volume") == 0) {
              printf("To execute play action   : %s\n", playbackAction.c_str());
              sendCurrentMediaVolume();
            }
        } else if(strcmp(p, CMD_WIFI_SETTING.c_str()) == 0) { // Wifi setting
            printf("\t%s: get Wifi setting command...\n", APP_TAG.c_str());
            p = strtok(NULL, ":");
            printf("Wifi configuration...\n");
            // King
            std::string wifiSettingAction = p;
            if (wifiSettingAction.find(CMD_WIFI_SETTING_ACTION_SCAN.c_str(), 0) == 0) {  // scan
              printf("\t%s: get scanning Wifi command...\n",APP_TAG.c_str());
              while(!isScanningWifiFinish) {
                scanning_wifi();
              }
              isScanningWifiFinish = false;//Damon end
            } else if (wifiSettingAction.find(CMD_WIFI_SETTING_TAG_SSID.c_str(), 0) == 0) { // ssid=
              wifiInfoData = p;
              printf("wifi setting send data to thread: %s\n", wifiInfoData.c_str());

              pthread_t wifiSettingThread;
              //int wifiSettingThreadError = pthread_create(&wifiSettingThread, NULL, wifiSetting, (void *)wifiInfoData.c_str());
              int wifiSettingThreadError = pthread_create(&wifiSettingThread, NULL, wifiSetting, NULL);
              printf("\t%s: wifiSettingThreadError = %d\n", APP_TAG.c_str(), wifiSettingThreadError);
            }
        } else if(strcmp(p, CMD_SVA_AVS) == 0) { // SVA AVS:  reserved
            //
        } else if (strcmp(p, CMD_RESET_WIFI) == 0) {  // Reset wifi: reserved
            //openSoftAPMode();
        } else if (strcmp(p, CMD_AVS_SETTING.c_str()) == 0) { // AVS setting
          printf("\t%s: get AVS setting command...\n", APP_TAG.c_str());
          p = strtok(NULL, ":");
          std::string avsAction = p;
          if (avsAction.find(CMD_AVS_SETTING_ACTION_GET_DATA.c_str(), 0) == 0) {  // get_data
            printf("\t\t\t\tstart read product data send it\n");
            readProductFile();
          } else if (avsAction.find(CMD_AVS_SETTING_ACTION_LOGIN_INFO.c_str(), 0) == 0) { // login_info
            /*if (!exitAVS) {
              deleteAVSAccountInfo();
              exitSva = true;
              exitAVS = true;
              m_wisStateTrigger.notify_all();
              sleep(3);
            }*/

            string login_info;
            p = strtok(NULL, ":");
            while (p != NULL) {
              login_info += p;
              p=strtok(NULL, ":");
              if (p != NULL) {
                login_info += ":";
              }
            }

            printf("\t\t\t\treceive auth code data and save it to file: %s\n", login_info.c_str());
            std::string clientID, redirectUri, authCode, userName, userEmail;
            p = strtok((char *)login_info.c_str(), ",");
            while(p != NULL){
              std::string key = p;
              if (key.find(CMD_AVS_SETTING_TAG_CLIENT_ID.c_str(), 0) == 0) {
                clientID = key.substr(key.find('=') + 1);
              } else if (key.find(CMD_AVS_SETTING_TAG_REDIRECT_URI.c_str(), 0) == 0) {
                redirectUri = key.substr(key.find('=') + 1);
              } else if (key.find(CMD_AVS_SETTING_TAG_AUTH_CODE.c_str(), 0) == 0) {
                authCode = key.substr(key.find('=') + 1);
              } else if (key.find(CMD_AVS_SETTING_TAG_USER_NAME.c_str(), 0) == 0) {
                userName = key.substr(key.find('=') + 1);
              } else if (key.find(CMD_AVS_SETTING_TAG_USER_EMAIL.c_str(), 0) == 0) {
                userEmail = key.substr(key.find('=') + 1);
              }

              p=strtok(NULL, ",");
            }

            printf("clientID = %s, redirectUri = %s, authCode = %s, userName = %s, userEmail =%s\n",
                      clientID.c_str(), redirectUri.c_str(), authCode.c_str(), userName.c_str(), userEmail.c_str());

            std::ofstream authCodeOut(STORE_AUTH_CODE_FILE_PATH.c_str());
            authCodeOut<< CMD_AVS_SETTING_TAG_CLIENT_ID << "=" << clientID << "\n";
            authCodeOut<< CMD_AVS_SETTING_TAG_REDIRECT_URI << "=" << redirectUri << "\n";
            authCodeOut<< CMD_AVS_SETTING_TAG_AUTH_CODE << "=" << authCode << "\n";
            authCodeOut.close();

            std::ofstream accountFileOut(STORE_AVS_ACCOUNT_FILE_PATH.c_str());
            accountFileOut<< CMD_AVS_SETTING_TAG_USER_NAME << "=" << userName << "\n";
            accountFileOut<< CMD_AVS_SETTING_TAG_USER_EMAIL << "=" << userEmail << "\n";
            accountFileOut.close();

            printf("\t\t\t\twrite auth code done\n");
            //receive authorize code and save it to $STORE_AUTH_CODE_FILE_PATH file
            //notification AVS use authorize code get access token and save it to refreshtoken.txt file
            pthread_t avsGetAccessTokenThread;
            int avsGetAccessTokenThreadError = pthread_create(&avsGetAccessTokenThread, NULL, executeAvsGetAccessToken,NULL);
            printf("\t%s: avsGetAccessTokenThreadError = %d\n", APP_TAG.c_str(), avsGetAccessTokenThreadError);
          } else if (avsAction.find(CMD_AVS_SETTING_ACTION_LOGOUT.c_str(), 0) == 0) {  // logout_out
            deleteAVSAccountInfo();
            exitSva = true;
            exitAVS = true;
            m_wisStateTrigger.notify_all();
            sendAvsLogout();
          }
        } else if(strcmp(p,CMD_FM_DATA.c_str()) == 0){ // fm data
          printf("receive fm data P:%s\n", p);
          p = strtok(NULL, ":");
          std::string fmAction = p;

          if(fmAction.find(CMD_FM_DATA_ENABLE.c_str(), 0) == 0) { //enable
            p = strtok(p, ",");
            char* tempCountry = strtok(NULL,",");
            std::cout << "country_code key-value: " << tempCountry << std::endl;
            char* tempFrequency = strtok(NULL,",");
            std::cout << "frequency key-value: " << tempFrequency << std::endl;

            char* countryCode = strtok(tempCountry,"=");
            countryCode = strtok(NULL,"=");
            std::cout<< "country_code value = " << countryCode << std::endl;
            mWisFM.setCountry(atoi(countryCode));

            char *frequency = strtok(tempFrequency,"=");
            frequency = strtok(NULL,"=");
            std::cout << "frequency value = " << frequency << std::endl;

            mWisFM.enable(main_ps_name_callback);
            mWisFM.setFrequency(atoi(frequency));

            int current_volume = mWisFM.getVolume();
            std::string content;
            content = "fm:volume="+std::to_string(current_volume);
            mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
          }else if(fmAction.find(CMD_FM_DATA_DISABLE.c_str(), 0) == 0){
            mWisFM.disable();
            mWisFM.mLastFMOpened=false;
          }else if(fmAction.find(CMD_FM_DATA_FREQUENCY.c_str(), 0) == 0){
            p = strtok(p, "=");
            mWisFM.enable(main_ps_name_callback);

            printf("%s\n", "FM set frequency");
            p = strtok(NULL,"=");
            std::cout << "set frequency to : " << p << std::endl;
            std::string frequency = p;
            mWisFM.setFrequency(atoi(frequency.c_str()));

            int current_volume = mWisFM.getVolume();
            std::string content;
            content = "fm:volume="+std::to_string(current_volume);
            mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
          }else if(fmAction.find(CMD_FM_DATA_GET_VOLUME.c_str(), 0) == 0){
            int current_volume = mWisFM.getVolume();
            std::string content;
            content = "fm:volume="+std::to_string(current_volume);
            mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
          }else if(fmAction.find(CMD_FM_DATA_SET_VOLUME.c_str(), 0) == 0){
            p = strtok(p, "=");
            p = strtok(NULL,"=");
            std::cout << "set volume to: " << p << std::endl;

            std::string volume = p;
            mWisFM.setVolume(volume);
          } else if (fmAction.find(CMD_FM_DATA_SEARCH.c_str(), 0) == 0) {
            p = strtok(p, ",");
            printf("fm search data P:%s\n", p);
            p = strtok(NULL,",");
            std::cout << "countryCode key-value: " << p << std::endl;
            char* countryCode = strtok(p,"=");
            countryCode = strtok(NULL,"=");
            std::cout << "countryCode value: " << countryCode << std::endl;
            mWisFM.setCountry(atoi(countryCode));

            mWisFM.enable(main_ps_name_callback);

            fm_searchStations.clear();
            mWisFM.search(main_searchex_callback);
          }
        } else if (strcmp(p,CMD_SET_LOCALE) == 0) { // Locale
          p = strtok(NULL, ":");
          printf("set locale to %s...\n", p);
          sendSettingsUpdatedEvent(p);
        } else if (strcmp(p,CMD_OTA_UPDATE.c_str()) == 0) { // OTA
          printf("\t%s: OTA update...\n", APP_TAG.c_str());
          p = strtok(NULL, ":");
          std::string otaAction = p;
          if (otaAction.find(CMD_OTA_UPDATE_ACTION_CHECK_CUR_VER.c_str(), 0) == 0) {
            std::string currentBuildVersion = mWisCommon.getCurrentImageBuildVersion();
            std::string currentBuildDate = mWisCommon.getCurrentImageBuildDate();
            std::string content;
            content = "ota:cur_ver=" + currentBuildDate + ":" + currentBuildVersion;
            mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
          } else if (otaAction.find(CMD_OTA_UPDATE_ACTION_CHECK_UPDATE.c_str(), 0) == 0) {
            pthread_t checkUpdateThread;
            pthread_create(&checkUpdateThread, NULL, checkOTAUpdate,NULL);
          } else if (otaAction.find(CMD_OTA_UPDATE_ACTION_UPGRADE, 0) == 0) {
            if(!mWisAws.isDownloading()){
              pthread_t upgradeThread;
              pthread_create(&upgradeThread, NULL, OTAUpgrade,NULL);
            }else{
              std::string content;
              content = "ota:upgrade_progress="+std::to_string(downloadProgress);
              mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
            }
          } else if(otaAction.find(CMD_OTA_UPDATE_ACTION_CANCEL,0) == 0){
            pthread_t cancelThread;
            pthread_create(&cancelThread, NULL, OTACancel,NULL);
          }
        } else if (strcmp(p, CMD_BATTERY_INFO) == 0) {
          // King add
          printf("\t%s: Battery info...\n", APP_TAG.c_str());
          p = strtok(NULL, ":");
          p = strtok(p,",");
          std::string content = "battery_info:";
          while(p != NULL){
            std::string key = p;
            if (strcmp(key.c_str(),BATTERY_INFO_KEY_CAPACITY.c_str()) == 0) {
              content += (key + "=" + mWisCommon.getBatteryCapacity());
            }

            p=strtok(NULL, ",");
            if (p != NULL) {
              content += ",";
            }
          }
          mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
        } else if (strcmp(p, CMD_ADVANCED_SETTING) == 0) { // Advanced settings
          printf("\t%s: Advanced settings...\n", APP_TAG.c_str());
          p = strtok(NULL, ":");
          std::string otaAction = p;
          if (otaAction.find("get_current_settings", 0) == 0) {
            // Send data:  advanced_settings:get_current_settings=enablePreRecSoundPrompt,enablePostRecSoundPrompt
            p = strtok(p,"=");  // get_current_settings
            p = strtok(NULL,"="); // enablePreRecSoundPrompt,enablePostRecSoundPrompt
            p = strtok(p,",");
            std::string content = "advanced_settings:";

            while(p != NULL){
              std::string key = p;
              if (strcmp(key.c_str(),CONFIG_KEY_WAKEWORD_DETECTOR.c_str()) == 0) {
                content += (key + "=" + std::to_string(wakeWordDetector));
              } else if (strcmp(key.c_str(),CONFIG_KEY_RECORDING_SOURCE.c_str()) == 0) {
                content += (key + "=" + std::to_string(recordingSource));
              } else if (strcmp(key.c_str(),CONFIG_KEY_ENABLE_PRE_REC_SOUND_PROMPT.c_str()) == 0) {
                content += (key + "=" + std::to_string(enablePreRecSoundPrompt));
              } else if (strcmp(key.c_str(),CONFIG_KEY_ENABLE_POST_REC_SOUND_PROMPT.c_str()) == 0) {
                content += (key + "=" + std::to_string(enablePostRecSoundPrompt));
              } else if (strcmp(key.c_str(),CONFIG_KEY_SVA_KEYWORD_CONFIDENCE_LEVEL.c_str()) == 0) {
                content += (key + "=" + std::to_string(svaKeywordConfidenceLevel));
              } else if (strcmp(key.c_str(),CONFIG_KEY_SVA_NUMBER_OF_KEYWORDS.c_str()) == 0) {
                content += (key + "=" + std::to_string(svaNumberOfKeywords));
              } else if (strcmp(key.c_str(),CONFIG_KEY_SVA_NUMBER_OF_USERS_PER_KEYWORD.c_str()) == 0) {
                content += (key + "=" + std::to_string(svaNumberOfUsersPerKeyword));
              } else if (strcmp(key.c_str(),CONFIG_KEY_RECORDING_TIMEOUT.c_str()) == 0) {
                content += (key + "=" + std::to_string(recordingTimeout));
              } else if (strcmp(key.c_str(),CONFIG_KEY_WAIT_DIRECTIVE_TIMEOUT.c_str()) == 0) {
                content += (key + "=" + std::to_string(waitDirectiveTimeout));
              } else if (strcmp(key.c_str(),CONFIG_KEY_WAIT_FOR_RESPONSE_TIMEOUT.c_str()) == 0) {
                content += (key + "=" + std::to_string(waitForResponseTimeout));
              } else if (strcmp(key.c_str(),CONFIG_KEY_SYNC_NTP_SERVER.c_str()) == 0) {
                content += (key + "=" + syncNTPServer);
              } else if (strcmp(key.c_str(),CONFIG_KEY_PING_ADDRESS.c_str()) == 0) {
                content += (key + "=" + pingAddress);
              } else if (strcmp(key.c_str(),CONFIG_KEY_ENABLE_SOUND_WHEN_RECEIVED_PHONE_CMD.c_str()) == 0) {
                content += (key + "=" + std::to_string(enableSoundWhenReceivedPhoneCmd));
              } else if (strcmp(key.c_str(),CONFIG_KEY_OTA_FTP_SERVER.c_str()) == 0) {
                content += (key + "=" + otaFTPServer);
              } else if (strcmp(key.c_str(),CONFIG_KEY_OTA_FTP_USER_NAME.c_str()) == 0) {
                content += (key + "=" + otaFTPUserName);
              } else if (strcmp(key.c_str(),CONFIG_KEY_OTA_FTP_PASSWORD.c_str()) == 0) {
                content += (key + "=" + otaFTPPassword);
              } else if (strcmp(key.c_str(),CONFIG_KEY_OTA_FTP_REMOTE_VERSION_FILE_PATH.c_str()) == 0) {
                content += (key + "=" + otaFTPRemoteVersionFilePath);
              }

              p=strtok(NULL, ",");
              if (p != NULL) {
                content += ",";
              }
            }
            mWisCommon.sendUDPDataToSender(mSenderIPAddress, content);
          } else if (otaAction.find(CMD_REBOOT_DEVICE, 0) == 0) {
            // Reboot device
            printf("Do reboot!!!\n");
            system("reboot");
          } else if(otaAction.find(CMD_FACTORY_RESET, 0) == 0) {
            // Factory reset
            printf("Do factory reset!!!\n");
            system((APP_ROOT_FOLDER + SHELL_SCRIPT_FACTORY_RESET).c_str());
          } else {
            p = strtok(p,",");

            char key[50];
            char value[150];
            while(p != NULL){
              printf("temp p = %s\n", p);
              memset(key,0x00,50);
              memset(value,0x00,150);
              mWisCommon.subString(p, key, value);

              if (strcmp(key,CONFIG_KEY_WAKEWORD_DETECTOR.c_str()) == 0) {
                wakeWordDetector = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_RECORDING_SOURCE.c_str()) == 0) {
                recordingSource = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_ENABLE_PRE_REC_SOUND_PROMPT.c_str()) == 0) {
                enablePreRecSoundPrompt = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_ENABLE_POST_REC_SOUND_PROMPT.c_str()) == 0) {
                enablePostRecSoundPrompt = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_SVA_KEYWORD_CONFIDENCE_LEVEL.c_str()) == 0) {
                svaKeywordConfidenceLevel = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_SVA_NUMBER_OF_KEYWORDS.c_str()) == 0) {
                svaNumberOfKeywords = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_SVA_NUMBER_OF_USERS_PER_KEYWORD.c_str()) == 0) {
                svaNumberOfUsersPerKeyword = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_RECORDING_TIMEOUT.c_str()) == 0) {
                recordingTimeout = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_WAIT_DIRECTIVE_TIMEOUT.c_str()) == 0) {
                waitDirectiveTimeout = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_WAIT_FOR_RESPONSE_TIMEOUT.c_str()) == 0) {
                waitForResponseTimeout = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_SYNC_NTP_SERVER.c_str()) == 0) {
                syncNTPServer = value;
              } else if (strcmp(key,CONFIG_KEY_PING_ADDRESS.c_str()) == 0) {
                pingAddress = value;
              } else if (strcmp(key,CONFIG_KEY_ENABLE_SOUND_WHEN_RECEIVED_PHONE_CMD.c_str()) == 0) {
                enableSoundWhenReceivedPhoneCmd = atoi(value);
              } else if (strcmp(key,CONFIG_KEY_OTA_FTP_SERVER.c_str()) == 0) {
                otaFTPServer = value;
              } else if (strcmp(key,CONFIG_KEY_OTA_FTP_USER_NAME.c_str()) == 0) {
                otaFTPUserName = value;
              } else if (strcmp(key,CONFIG_KEY_OTA_FTP_PASSWORD.c_str()) == 0) {
                otaFTPPassword = value;
              } else if (strcmp(key,CONFIG_KEY_OTA_FTP_REMOTE_VERSION_FILE_PATH.c_str()) == 0) {
                otaFTPRemoteVersionFilePath = value;
              }

              p=strtok(NULL, ",");
            }
            mWisCommon.saveCompanionSettings(wakeWordDetector, recordingSource, enablePreRecSoundPrompt, enablePostRecSoundPrompt,
                                              svaKeywordConfidenceLevel, svaNumberOfKeywords, svaNumberOfUsersPerKeyword, recordingTimeout,
                                              waitDirectiveTimeout, waitForResponseTimeout, syncNTPServer, pingAddress, enableSoundWhenReceivedPhoneCmd,
                                              otaFTPServer, otaFTPUserName, otaFTPPassword, otaFTPRemoteVersionFilePath);
          }
        }
      }
  }
}
//#endif /* WISTRON_WISDEFINERESOURCES_H */
