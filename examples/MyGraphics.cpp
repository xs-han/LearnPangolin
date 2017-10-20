#include <iostream>
#include <pangolin/pangolin.h>
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

void drawCamera(const float * Twc){
    //把下面的点都做一次旋转变换
    glPushMatrix();
    //col major
    glMultMatrixf(Twc);

    //直线的创建
    const float w = 2;
    const float h = w*0.75;
    const float z = w*0.6;
    glLineWidth(2);
    glColor3f(1.0,0,0);
    glBegin(GL_LINES);

    glVertex3f(0,0,0);
    glVertex3f(w,h,z);
    glVertex3f(0,0,0);
    glVertex3f(w,-h,z);
    glVertex3f(0,0,0);
    glVertex3f(-w,-h,z);
    glVertex3f(0,0,0);
    glVertex3f(-w,h,z);
    glVertex3f(w,h,z);
    glVertex3f(-w,h,z);
    glVertex3f(-w,h,z);
    glVertex3f(-w,-h,z);
    glVertex3f(-w,-h,z);
    glVertex3f(w,-h,z);
    glVertex3f(w,-h,z);
    glVertex3f(w,h,z);
    glEnd();

    glPopMatrix();
}

void drawPoint(float x, float y, float z){
    //点的创建
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(x,y,z);
    glEnd();
}

int main(int argc, char **argv)
{
    //创建一个窗口
    pangolin::CreateWindowAndBind("Main",640,480);
    //启动深度测试
    glEnable(GL_DEPTH_TEST);

    // Define Projection and initial ModelView matrix
    pangolin::OpenGlRenderState s_cam(
            pangolin::ProjectionMatrix(640,480,420,420,320,240,0.2,100),
            //对应的是gluLookAt,摄像机位置,参考点位置,up vector(上向量)
            pangolin::ModelViewLookAt(0,-10,0.1,0,0,0,pangolin::AxisNegY)
    );

    // Create Interactive View in window
    pangolin::Handler3D handler(s_cam);
    //setBounds 跟opengl的viewport 有关
    //看SimpleDisplay中边界的设置就知道
    pangolin::View &d_cam = pangolin::CreateDisplay().SetBounds(0.0,1.0,0.0,1.0,-640.0f/480.0f)
            .SetHandler(&handler);

    std::vector<float > Twc = {1,0,0,0, 0,1,0,0 , 0,0,1,0 ,0,0,5,1};
    float rvecArray[] = {0, 0, CV_PI / 6};
    float tvec[] = {0,0,5};
    float homo[] = {0,0,0,1};
    Mat rvec(3,1,CV_32F,rvecArray);
    Mat R(3,3,CV_32F), t(3,1,CV_32F,tvec), hmat(1,4,CV_32F,homo);
    Rodrigues(rvec, R);
    Mat TwcMat(4,4,CV_32F);
    R.convertTo(TwcMat(Range(0,3), Range(0,3)),CV_32F);
    t.convertTo(TwcMat(Range(0,3), Range(3,4)),CV_32F);
    hmat.convertTo(TwcMat(Range(3,4), Range(0,4)),CV_32F);

    vector<vector<float> > allCams;
    float x = 0, y = 0, z = 5;
    while(!pangolin::ShouldQuit())
    {
        // Clear screen and activate view to render into
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_cam.Activate(s_cam);

        // Render OpenGL Cube
        pangolin::glDrawColouredCube();\
        //坐标轴的创建
        pangolin::glDrawAxis(3);

        //z += 0.01;
        drawPoint(x,y,z);
//        Twc[14] += 0.01;
//        allCams.push_back(Twc);
//        for(vector<float> tmp: allCams){
//            drawCamera(tmp.data());
//        }
        Mat matT = TwcMat.t();
        drawCamera((float*)matT.data);

        // Swap frames and Process Events
        pangolin::FinishFrame();
    }

    return 0;

}