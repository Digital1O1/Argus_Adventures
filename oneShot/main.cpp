#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/*
* Angled vs Quotation marks
*   - Double quotes
*     - First searches directory that has the source file 
*     - If not found, it'll search the stadnard system include path
*     - Typically used for user-defined headers or private project files
*   - Angled brackets 
*     - Skips local source  directory 
*     - Seraches predefined system include paths 
*     - Typically used for standard library headers 
*/ 

#include <Argus/Argus.h>
#include <EGLStream/EGLStream.h>
#include "ArgusHelpers.h"
#include "CommonOptions.h"

// Error handling stuff 
/*
 * Purpose of continuation character \
 *  - Tells preprocessor : Treat the next line as a direct continuation of this line
 *  - The C preprocessors expect the #define statements to be just one line 
 *    - So as soon as it hits the new line character when you press ENTER, 
 *      it'll assume the macro definition is done
 *
 * When writing a multi-line macro 
 *  - You MUST put a backslash at the end of every line except the last one
 *
 * Example 
 * #define EXIT_IF_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "Error: Null pointer at %s:%d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
*/
#define EXIT_IF_NULL(val,msg) \
{if(!val){printf("EXITING PROGRAM NOW WITH ERROR : %s\n",msg);return EXIT_FAILURE;}}

#define EXIT_IF_NOT_OK(val,msg)\
{if (val!=Argus::STATUS_OK) {printf("%s\n",msg); return EXIT_FAILURE;}}

int main()
{
  printf("Running program...\r\n");

  // Variables 
  int cameraDevice = 0;
  int cameraSensorMode = 2;

  const uint64_t NANOSECOND_CONVERSION = 1000000000;
  const uint64_t SECONDS_SET           = 5;

  // Create CameraProvider
  Argus::UniqueObj<Argus::CameraProvider> cameraProvider(Argus::CameraProvider::create());
  Argus::ICameraProvider *iCameraProvider = Argus::interface_cast<Argus::ICameraProvider>(cameraProvider);
  EXIT_IF_NULL(iCameraProvider, "Cannot get core camera provider interface");
  printf("Argus Version: %s\n", iCameraProvider->getVersion().c_str());

  // Picking the physical camera
  // Right now it's hard coded to just pick camera0 
  // TODO:: Learn how to enumerate through the attached cameras down the road 
  
  Argus::CameraDevice *device =
    ArgusSamples::ArgusHelpers::getCameraDevice(
        cameraProvider.get(), cameraDevice);

  Argus::ICameraProperties *iCameraProperties = Argus::interface_cast<Argus::ICameraProperties>(device);

  if(!iCameraProperties)
  {
    REPORT_ERROR("Failed to get iCameraProperties interface");
    printf("Exiting now...\r\n");
    return EXIT_FAILURE;
  }


  // Picking the sensor mode
  // The original program used 
  Argus::SensorMode * sensorMode = ArgusSamples::ArgusHelpers::getSensorMode(device,cameraSensorMode);
  Argus::ISensorMode *iSensorMode = Argus::interface_cast<Argus::ISensorMode>(sensorMode);

  if(!iSensorMode)
  {
    REPORT_ERROR("Failed to get sensor mode interface");
    return EXIT_FAILURE;
  }
  std::cout<< iSensorMode->getResolution().width() << " X " << iSensorMode->getResolution().height();
  //std::cout<< iSensorMode->getResolution().width();


  // Creating CaptureSession
  // Represents active connection to one or more cameras 
  Argus::Status status;
  Argus::UniqueObj<Argus::CaptureSession> captureSession(iCameraProvider->createCaptureSession(device, &status));

  EXIT_IF_NOT_OK(status,"Failed to create capture session\r\n");

  Argus::ICaptureSession *iSession = Argus::interface_cast<Argus::ICaptureSession>(captureSession);
  EXIT_IF_NULL(iSession,"Can't capture Session Interface \r\n");

  // Create output stream with EGLStream
  Argus::UniqueObj<Argus::OutputStreamSettings> streamSetting
    (iSession->createOutputStreamSettings(Argus::STREAM_TYPE_EGL));

  Argus::IEGLOutputStreamSettings *iEGLStreamSettings = Argus::interface_cast<Argus::IEGLOutputStreamSettings>(streamSetting);
  EXIT_IF_NULL(iEGLStreamSettings,"Can't get IEGLOutputStreamSettings Interface");

  iEGLStreamSettings->setPixelFormat(Argus::PIXEL_FMT_YCbCr_420_888);
  iEGLStreamSettings->setResolution(iSensorMode->getResolution());
  iEGLStreamSettings->setMetadataEnable(true);

  // Once the stream is created from the setting object, any changes on the stream won't 
  // take any effect
  Argus::UniqueObj<Argus::OutputStream> stream(iSession->createOutputStream(streamSetting.get()));
  EXIT_IF_NULL(stream,"Failed to create EGLOutputstream");

  // Create FrameConsumer to read frames off stream 
  Argus::UniqueObj<EGLStream::FrameConsumer> consumer(
    EGLStream::FrameConsumer::create(stream.get()));

  EGLStream::IFrameConsumer *iFrameConsumer = 
    Argus::interface_cast<EGLStream::IFrameConsumer>(consumer);
  EXIT_IF_NULL(iFrameConsumer,"Failed to initialize Consumer");

  Argus::UniqueObj<Argus::Request> request(iSession->createRequest(Argus::CAPTURE_INTENT_STILL_CAPTURE));
  Argus::IRequest *iRequest = Argus::interface_cast<Argus::IRequest>(request);
  EXIT_IF_NULL(iRequest, "Fail to get capture request interface");
  // Build the capture request
  uint32_t requestId = iSession->capture(request.get());
  EXIT_IF_NULL(requestId,"Failed to submit cpature request");

  EGLStream::Image *image = iFrame->getImage();
  EXIT_IF_NULL(image,"Failed to get image from iFrame->getImage()");

  EGLStream::IImageJPEG *iImageJPEG = Argus::interface_cast<EGLStream::IImangeJPEG>(image);
  EXIT_IF_NULL(iImageJPEG,"Failed to get ImageJPEG Interface");

  status = iImageJPEG->writeJPEG(FILE_PREFIX "test.jpg");
  EXIT_IF_NOT_OK(status, "Fail to write JPEG");
  printf("Finish line...\r\n");
  return 0;
}
