#include <iostream>
#include <string>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/core/core.hpp"
#include <vector>
#include <fstream>
#include <time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include <cmath>
#include <dirent.h>


using namespace std;
struct rgbS staticAvRgb(cv::Mat img);
cv::Mat imgPaste(cv::Mat src, cv::Mat img, int *box);
cv::Mat resizeImg(cv::Mat srcImg, int width, bool isSque);

struct rgbS {
    int val[4];
};


string getTime()
{
    time_t timep;
    time (&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y%m%d%H%M%S",localtime(&timep) );
    return tmp;
}

int getRand(int v){
    srand((unsigned)time(NULL));
    return  rand()%v;
}

string intToString(int v) {
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%u", v);

    string str = buf;
    return str;
}


void getJustCurrentFile( string path, vector<string>& files)
{
    struct dirent *ptr;
    DIR *dir;
    dir=opendir(path.c_str());
    while((ptr=readdir(dir))!=NULL)
    {
        //跳过'.'和'..'两个目录
        if(ptr->d_name[0] == '.')
            continue;
        files.push_back(ptr->d_name);
    }
}
cv::Mat Cut_img(cv::Mat src_img, int *box) {

    int newW = box[2] - box[0];
    int newH = box[3] - box[1];


    cv::Mat roi_img;

    cv::Rect rect(box[0], box[1], newW, newH);

    src_img(rect).copyTo(roi_img);

    return roi_img;


}




















int frame_width;
int frame_height;

int frame_number;
int frame_width_new;
int frame_height_new;
float frame_fps;
string movie_ext;
int final_num = 0;
string randStr;
int subW = 8;
int mainW = 1080;
string zm_path = "/Users/xxxx/Desktop/moviebinary/zm/";
string outMovieBaseName = "/Users/xxxx/Desktop/moviebinary/movie_out/";


int main(int arc, char **argv)
{

    string video_name = "/Users/xiangju/xxxx/moviebinary/movie/10.mp4";



    srand((unsigned)time(NULL));
    randStr = getTime()+intToString(rand()%1000);






    movie_ext =  video_name.substr(video_name.find_last_of('.') + 1);//获取文件后缀

    string outName = outMovieBaseName+randStr+"."+movie_ext;



    cv::VideoCapture capture(video_name);
    if (!capture.isOpened())
    {
        bool a = capture.open(video_name);
        if(!a){
            cout << "open video error";
        }

        exit(-2);
        //
    }

    frame_width = (int)capture.get(CV_CAP_PROP_FRAME_WIDTH);
    frame_height = (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    frame_fps = capture.get(CV_CAP_PROP_FPS);
    frame_number = capture.get(CV_CAP_PROP_FRAME_COUNT);//总帧数

    frame_width_new = frame_width;
    frame_height_new = frame_height;


    int num = 0;//统计帧数
    cv::Mat img;
    cv::Mat mimage;


    int mxLen = frame_width_new / subW;
    int myLen = frame_height_new / subW;
    int my,mx,totalRgb,totalRgbJb;
    int box[4];
    int boxJb[4];
    cv::Mat tmpImg;
    cv::Mat tmpImgJb;
    rgbS tmpRgbJb;
    rgbS tmpRgb;

    rgbS tmpRgbMain;
    int avgRgbMain;
    cv::Mat zm[2];
    zm[0] = resizeImg(cv::imread(zm_path+"0.jpg"),subW, false);
    zm[1] = resizeImg(cv::imread(zm_path+"1.jpg"),subW, false);
    int randIndex;





    char image_name[255];
    string s_image_name;
    cv::VideoWriter writer;
    int isColor = 1;



    writer = cv::VideoWriter(outName, CV_FOURCC('X', 'V', 'I', 'D'),frame_fps,cv::Size(frame_width_new,frame_height_new),isColor);

    while (num<frame_number-1)
    {

        //从视频中读取一个帧
        bool bSuccess = capture.read(mimage);
        if (!bSuccess){ break;}

        //一帧图片frame

        cv::Mat image = cv::Mat::zeros(cv::Size(frame_width_new, frame_height_new), CV_8UC3);
        image.setTo(cv::Scalar(255, 255, 255));


        tmpRgbMain = staticAvRgb(mimage);
        avgRgbMain =  0.3*tmpRgbMain.val[0]+0.59*tmpRgbMain.val[1]+0.11*tmpRgbMain.val[2];

        for (my = 0; my < myLen; my++) {

            for (mx = 0; mx < mxLen; mx++) {
                box[0] = mx * subW;
                box[1] = my * subW;
                box[2] = (mx + 1) * subW;
                box[3] = (my + 1) * subW;
                tmpImg = Cut_img(mimage, box);
                tmpRgb = staticAvRgb(tmpImg);
                totalRgb = 0.3*tmpRgb.val[0]+0.59*tmpRgb.val[1]+0.11*tmpRgb.val[2];



                //判断周围一圈 rgb平均值


                boxJb[0] = (mx-1) * subW;
                boxJb[1] = (my-1) * subW;
                if(boxJb[0]<0){
                    boxJb[0] = 0;
                }
                if(boxJb[1]<0){
                    boxJb[1] = 0;
                }


                boxJb[2] = (mx + 2)>mxLen?  mxLen* subW:(mx + 2)*subW;
                boxJb[3] = (my + 2)>myLen?  myLen* subW:(my + 2)*subW;

                tmpImgJb = Cut_img(mimage, boxJb);
                tmpRgbJb = staticAvRgb(tmpImgJb);
                totalRgbJb = 0.3*tmpRgbJb.val[0]+0.59*tmpRgbJb.val[1]+0.11*tmpRgbJb.val[2];

                if(totalRgb<totalRgbJb){//替换
                    randIndex = rand()%2;
                    imgPaste(image,zm[randIndex],box);
                }
            }
        }



        writer.write(image);



    }





    capture.release();



    cout<<"success@@@@"<<outName<<"@@@@"<<endl;
    return 0;
}


struct rgbS staticAvRgb(cv::Mat img) {
    rgbS rgbArr;

    rgbArr.val[0] = rgbArr.val[1] = rgbArr.val[2] = 0;
    for (int i = 0; i < img.rows; i++) {
        cv::Vec3b *p = img.ptr<cv::Vec3b>(i);
        for (int j = 0; j < img.cols; j++) {

            //b g r
            rgbArr.val[0] += (int) p[j][2];
            rgbArr.val[1] += (int) p[j][1];
            rgbArr.val[2] += (int) p[j][0];
        }
    }

    int totalPix = img.rows*img.cols;

    rgbArr.val[0] = (int)rgbArr.val[0]/totalPix;
    rgbArr.val[1] = (int)rgbArr.val[1]/totalPix;
    rgbArr.val[2] = (int)rgbArr.val[2]/totalPix;

    return rgbArr;
}


cv::Mat imgPaste(cv::Mat src, cv::Mat img, int *box) {


    int newW = box[2] - box[0];
    int newH = box[3] - box[1];

    CvRect rect = CvRect(cv::Rect(box[0], box[1], newW, newH));
    img.copyTo(src(rect));
    return src;
}





cv::Mat resizeImg(cv::Mat srcImg, int width, bool isSque) {

    int srcWidth = srcImg.cols;
    int srcHeight = srcImg.rows;
    int newH, newW;
    int cutBox[4];
    cv::Mat dstImage;
    if (isSque) {

        if (srcWidth > srcHeight) {
            newH = width;
            newW = srcWidth * width / srcHeight;
            resize(srcImg, dstImage, cv::Size(newW, newH));
            cutBox[0] = (newW - width) / 2;
            cutBox[1] = 0;
            cutBox[2] = cutBox[0] + width;
            cutBox[3] = width;

        } else {

            newW = width;
            newH = srcHeight * width / srcWidth;
            resize(srcImg, dstImage, cv::Size(newW, newH));

            cutBox[1] = (newH - width) / 2;
            cutBox[0] = 0;
            cutBox[3] = cutBox[1] + width;
            cutBox[2] = width;
        }

        dstImage = Cut_img(dstImage, cutBox);

    } else {
        newW = width;
        newH = srcHeight * width / srcWidth;
        resize(srcImg, dstImage, cv::Size(newW, newH));
    }
    return dstImage;
}

