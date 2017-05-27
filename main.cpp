#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

#include <iostream>
#include <sstream>
#include <vector>

using namespace cv;
using namespace std;

const double PI = 3.14159265;

int main(int argc, char** argv)
{
 const char* filename = argc >= 2 ? argv[1] : "opencv/1.mov";
 VideoCapture cap(0);
 cap.open(filename);
 if (cap.isOpened() == 0) {
    cout << "cant open" << filename << endl;
    return -1;
 }
    Mat src;
    int num = 1;
    const string NAME = "opencv/output.mov";   // Form the new name with container
    int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form

    // Transform from int to char via Bitwise operators
    //char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};

    Size S = Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH), (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT)); // Acquire input size
    cout << "Processing..." << endl;
    VideoWriter outputVideo;
    outputVideo.open(NAME, cap.get(CV_CAP_PROP_FOURCC),cap.get(CV_CAP_PROP_FPS), S, true);

    if (!outputVideo.isOpened()) {
        cout << "could not open output video" << endl;
        return -1;
    }
    for (;;) {        //==================start for frames loop==================================
 Mat cropres ,hlres, res, src1, dst, cdst, cdst1;
 //resize(src, src1, Size(), 0.5, 0.5, INTER_NEAREST);
//for (int i = 0; i < 17; i++) {
  cap >> src;
  if(src.empty()){
      break;}
    cout << "\r" << num << "/" << cap.get(CAP_PROP_FRAME_COUNT) << flush;
 src.convertTo(src1, -1, 0.9, 0);
 blur(src1, res, Size(3,3), Point(-1,-1));
 //threshold(src1, res, 160, 255, THRESH_BINARY);
 //inpaint(src1, res, hlrehlreshlress, 3, INPAINT_TELEA);

 Canny(res, dst, 20, 50, 3);
 //dst = cropres(Rect(0, cropres.rows/2, cropres.cols, cropres.rows/2));
 cvtColor(dst, cdst, CV_GRAY2BGR);
 cvtColor(dst, cdst1, CV_GRAY2BGR);

  vector<Vec4i> lines;
  vector<Vec4i> right;
  vector<Vec4i>::iterator itr;
  vector<Vec4i> left;
  vector<Vec4i>::iterator itl;
  itr = right.begin();
  itl = left.begin();
  HoughLinesP(dst, lines, 1, CV_PI/180, 50, 70, 10 );
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
    //cout << i << ": " << l << ", angle = ";
    double angle = atan2(l[3]-l[1], l[2]-l[0]) * 180.0 / CV_PI;
    //cout << angle << endl;
    if ((angle < -5 && angle > -45 && l[0] < 15)) {
        left.push_back(l);
        //cout << "right" << endl;
        itr++;
    }
    if (angle > 5 && angle < 45 && l[2] > (dst.size().width - 15)) {
        right.push_back(l);
        //cout << "left" << endl;
        itl++;
    }
  }

  //drawing lines
    Vec4i rightmid, leftmid;
    double angleright, angleleft;
    int mid = dst.size().height;
    for (size_t i = 0; i < right.size(); i++) {
        Vec4i l = right[i];
        if (abs(l[3] - dst.size().height/2) < mid) {
            rightmid = l;
            mid = l[3] - dst.size().height/2;
        }
        line( cdst1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,255), 3, CV_AA);
        line( res, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,255), 3, CV_AA);
    }
    if (rightmid[0]) {
        line( res, Point(rightmid[0], rightmid[1]), Point(rightmid[2], rightmid[3]), Scalar(0,0,0), 3, CV_AA);
    }
    mid = dst.size().height;
    for (size_t i = 0; i < left.size(); i++) {
        Vec4i l = left[i];
        if (abs(l[1] - dst.size().height/2) < mid) {
            leftmid = l;
            mid = l[1] - dst.size().height/2;
        }
        line( cdst1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,255), 3, CV_AA);
        line( res, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,255), 3, CV_AA);
    }
    if (leftmid[0]) {
        line( res, Point(leftmid[0], leftmid[1]), Point(leftmid[2], leftmid[3]), Scalar(0,0,0), 3, CV_AA);
    }
    angleright = atan2(rightmid[3]-rightmid[1], rightmid[2]-rightmid[0]) * 180.0 / CV_PI;
    angleleft = atan2(leftmid[3]-leftmid[1], leftmid[2]-leftmid[0]) * 180.0 / CV_PI;
    double anglearrow = abs(angleright) - abs(angleleft);
    Vec4f arrow;
    if (anglearrow < 0)
        anglearrow += 360;
    anglearrow = anglearrow * (PI / 180);
    cout << "\n angle: " << anglearrow << endl;
    arrow[0] = res.size().width / 2;
    arrow[3] = res.size().height - 50;
    arrow[2] = res.size().width / 2;
    arrow[1] = res.size().height - 250;
    double temp1, temp2;
    temp1 = ((arrow[0] - arrow[0]) * cos(anglearrow)) - ((arrow[1] - (arrow[1]+100)) * sin(anglearrow)) + arrow[0];
    temp2 = ((arrow[1] - (arrow[1]+100)) * cos(anglearrow)) + ((arrow[0] - arrow[0]) * sin(anglearrow)) + (arrow[1]+100) ;
    arrow[0] = temp1;
    arrow[1] = temp2;
    temp1 = ((arrow[2] - arrow[2]) * cos(anglearrow)) - ((arrow[3] - (arrow[1]+100)) * sin(anglearrow)) + arrow[2];
    temp2 = ((arrow[3] - (arrow[1]+100)) * cos(anglearrow)) + ((arrow[2] - arrow[2]) * sin(anglearrow)) + (arrow[1]+100);
    arrow[2] = temp1;
    arrow[3] = temp2;
    arrowedLine(res, Point(arrow[2],arrow[3]), Point(arrow[0],arrow[1]), Scalar(0,0,0), 3, CV_AA);
    outputVideo << res;
    num++;
    imshow("source", res);
    //imshow("detected lines", cdst);
    //imshow("detected lines 1", cdst1);
}   //======================================end of frames loop======================================================


 waitKey();

 return 0;
}
