#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <stdexcept>
#include <string>
#include <signal.h>
#include <libcam.h>
#include "timer.h"
#include "libcam/libcam.h"
using namespace std;
using namespace cv;

bool should_continue = true;

void capture_signal(int sig) {
  if (should_continue == true) {
    should_continue = false;
    cout << "Stopping"<< endl;
  }
}

int main(int argc, char** argv) {
  signal(SIGINT, capture_signal);
  signal(SIGTERM, capture_signal);

  if (argc < 3) {
    cout << "Parametros: device-camara-1 device-camara2" << endl;
    return 1;
  }

  CvSize size = cvSize(640, 480);
  int fps = 15;
  Camera cam1(argv[1], size.width, size.height, fps);
  Camera cam2(argv[2], size.width, size.height, fps);

  cam1.setExposure(400);
  cam2.setExposure(400);

  int video_fps = 30;
  CvVideoWriter* writer1 = cvCreateVideoWriter("cam1.avi", CV_FOURCC('I','4','2','0'), video_fps, size);
  CvVideoWriter* writer2 = cvCreateVideoWriter("cam2.avi", CV_FOURCC('I','4','2','0'), video_fps, size);

  cam1.Update();
  cam2.Update();

  IplImage* frame1 = cvCreateImage(size, IPL_DEPTH_8U, 3);
  IplImage* frame2 = cvCreateImage(size, IPL_DEPTH_8U, 3);

  cvNamedWindow("cam1");
  cvNamedWindow("cam2");
  cvStartWindowThread();

  Timer t, tsegundo;
  uint frames = 0;
  vector<cv::Mat> channels1, channels2;
  cv::Mat anaglyph;
  tsegundo.start();

  cout << "recording" << endl;
  while (should_continue) {
    frames++;
    t.start();
    cam1.Update((unsigned int)1);
    cam2.Update((unsigned int)1);

    cam1.toIplImage(frame1);
    cvWriteFrame(writer1, frame1);
    cv::imshow("cam1", frame1);
    cam2.toIplImage(frame2);
    cvWriteFrame(writer2, frame2);
    cv::imshow("cam2", frame2);

    //uint disp = 10;
    //cv::Mat new_frame2(frame2_mat.size(), frame2_mat.type());
    //cv::Mat sub_frame2(new_frame2, cv::Rect(disp,0,frame2_mat.cols - disp, frame2_mat.rows));
    //sub_frame2 = frame2;
    //cout << sub_frame2.cols << " " << sub_frame2.rows << endl;
    //cv::split(frame1_mat,channels1);
    //cv::split(sub_frame2,channels2);
    //cv::Mat output_channels[] = { channels1[0], channels1[1], channels2[2] };
    //cv::merge(output_channels, 3, anaglyph);
    //cv::imshow("anaglyph", anaglyph);
    //cv::waitKey(1);

    t.stop();
    tsegundo.pause();
    if (tsegundo.getSec() >= 1) {
      cout << "delta: " << t << "FPS: " << frames << endl;
      tsegundo.start();
      tsegundo.stop(); 
      frames = 0;
    }
    tsegundo.start();
  }

  cvReleaseImage(&frame1);
  cvReleaseImage(&frame2);

  cvReleaseVideoWriter(&writer1);
  cvReleaseVideoWriter(&writer2);
}

void toAnaglyph(const cv::Mat& im1, const cv::Mat& im2, cv::Mat& anaglyph)
{

}
