#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"

#include <iostream>

using namespace cv;
using namespace std;

void help()
{
 cout << "\nThis program demonstrates line finding with the Hough transform.\n"
         "Usage:\n"
         "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

int main(int argc, char** argv)
{
 const char* filename = argc >= 2 ? argv[1] : "5.jpg";

 Mat src = imread(filename, 0);
 if(src.empty())
 {
     help();
     cout << "can not open " << filename << endl;
     return -1;
 }
 
 Mat cropres ,hlres, res, src1, dst, cdst, cdst1;
 resize(src, src1, Size(), 0.5, 0.5, INTER_NEAREST);
 threshold(src1, res, 160, 255, THRESH_BINARY);
 inpaint(src1, res, hlres, 3, INPAINT_TELEA);
 
 Canny(hlres, cropres, 650, 700, 5);
 dst = cropres(Rect(0, cropres.rows/2, cropres.cols, cropres.rows/2));
 cvtColor(dst, cdst, CV_GRAY2BGR);
 cvtColor(dst, cdst1, CV_GRAY2BGR);

 #if 0
  vector<Vec2f> lines;
  HoughLines(dst, lines, 1, CV_PI/180, 50, 10, 10 );

  for( size_t i = 0; i < lines.size(); i++ )
  {
     float rho = lines[i][0], theta = lines[i][1];
     Point pt1, pt2;
     double a = cos(theta), b = sin(theta);
     double x0 = a*rho, y0 = b*rho;
     pt1.x = cvRound(x0 + 1000*(-b));
     pt1.y = cvRound(y0 + 1000*(a));
     pt2.x = cvRound(x0 - 1000*(-b));
     pt2.y = cvRound(y0 - 1000*(a));
     line( cdst, pt1, pt2, Scalar(255,0,0), 3, CV_AA);
  }
 #else
  vector<Vec4i> lines;
  HoughLinesP(dst, lines, 1, CV_PI/180, 70, 10, 20 );
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
    line( cdst1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,255), 3, CV_AA);
    line( src1, Point(l[0], l[1]+(cropres.rows/2)), Point(l[2], l[3]+(cropres.rows/2)), Scalar(255,255,255), 3, CV_AA);
  }
 #endif
 imshow("source", src1);
 imshow("highlights", hlres);
 imshow("detected lines", cdst);
 imshow("detected lines 1", cdst1);

 waitKey();

 return 0;
}
