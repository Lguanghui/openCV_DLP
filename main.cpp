//
//  main.cpp
//  opencv
//
//  Created by 梁光辉 on 2020/4/22.
//  Copyright © 2020 梁光辉. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace cv;

float **Points=nullptr;//全局指针，存放坐标数组
int Points_count=0;

//读取txt文件
void readfile(string filename)
{
    ifstream file("/Users/yunyi/Desktop/Calibration_Points.txt");
    string Line;
    string buf;
    int i=0,j=0;
    
    //动态申请二维数组存放坐标
    Points=(float**)malloc(sizeof(float*)*30);
    for(int k=0;k<30;k++){
        *(Points+k)=(float*)malloc(sizeof(float)*5);
    }
    
    //打开文件，把坐标存入数组
    if(file)
    {
        while (getline(file, Line)) {//按行读取
            stringstream split(Line);//按空格分割
            while(split>>buf){
                Points[i][j]=stof(buf);
                j++;
            }
            i++;
            j=0;//不能忘了清零列标！
            Points_count++;
        }
    }
    file.close();
}

int main()
{
    //从文件中读取坐标点
    string filename="/Users/yunyi/Desktop/Calibration_Points.txt";
    readfile(filename);
    //显示28个坐标点
    cout<<"全部坐标点："<<endl;
    for(int l=0;l<Points_count;l++)
    {
        for (int m=0; m<5; m++) {
            cout<<Points[l][m]<<" ";
        }
        cout<<endl;
    }
    
    //选取6个标定点
    float Points_cali[6][5];
    //int index[6]={0,3,5,8,15,27};
    int index[6]={0,8,14,19,23,27};
    int ii=0;
    for (int i=0; i<6; i++) {
        for (int j=0; j<5; j++) {
            Points_cali[i][j]=Points[index[ii]][j];
        }
        ii++;
    }
    
    //其余的点
    float Points_remain[22][5];
    int index_remain[22]={1,2,3,4,5,6,7,9,10,11,12,13,15,16,17,18,20,21,22,24,25,26};
    ii=0;
    for(int i=0;i<22;i++){
        for (int j=0; j<5; j++) {
            Points_remain[i][j]=Points[index_remain[ii]][j];
        }
        ii++;
    }
    
    //构造A矩阵
    ii=0;
    float a[12][11]={0};
    for (int i=0; i<12; i++) {
        if(i%2==0)
        {
            a[i][0]=Points_cali[ii][0];
            a[i][1]=Points_cali[ii][1];
            a[i][2]=Points_cali[ii][2];
            a[i][3]=1;
            a[i][8]=-Points_cali[ii][0]*Points_cali[ii][3];
            a[i][9]=-Points_cali[ii][1]*Points_cali[ii][3];
            a[i][10]=-Points_cali[ii][2]*Points_cali[ii][3];
        }
        else{
            a[i][4]=Points_cali[ii][0];
            a[i][5]=Points_cali[ii][1];
            a[i][6]=Points_cali[ii][2];
            a[i][7]=1;
            a[i][8]=-Points_cali[ii][0]*Points_cali[ii][4];
            a[i][9]=-Points_cali[ii][1]*Points_cali[ii][4];
            a[i][10]=-Points_cali[ii][2]*Points_cali[ii][4];
            ii++;
        }
    }
    cout<<endl<<"A矩阵:"<<endl;
    Mat A(12, 11, CV_32F,a);//存放A矩阵
    cout<<A<<endl;
    
    //构造U矩阵
    float u[12]={0};
    ii=0;
    for(int i=0;i<12;i=i+2){
        u[i]=Points_cali[ii][3];
        u[i+1]=Points_cali[ii][4];
        ii++;
    }
    Mat U(12,1,CV_32F,u);
    cout<<endl<<"U矩阵："<<endl;
    cout<<U<<endl;
    
    //计算得到L矩阵
    Mat L=Mat::zeros(11, 1, CV_32F);
    L=(((A.t())*A).inv())*(A.t())*U;
    cout<<endl<<"L矩阵："<<endl<<L<<endl;
    
    //内参数计算
    
    float u0=((L.at<float>(0,0))*(L.at<float>(8,0))+(L.at<float>(1,0))*(L.at<float>(9,0))+(L.at<float>(2,0))*(L.at<float>(10,0)))/(pow(L.at<float>(8,0), 2)+pow(L.at<float>(9,0), 2)+pow(L.at<float>(10,0), 2));
    float v0=((L.at<float>(4,0))*(L.at<float>(8,0))+(L.at<float>(5,0))*(L.at<float>(9,0))+(L.at<float>(6,0))*(L.at<float>(10,0)))/(pow(L.at<float>(8,0), 2)+pow(L.at<float>(9,0), 2)+pow(L.at<float>(10,0), 2));
    //float u0=((L.at<float>(0,0))*(L.at<float>(8,0))+(L.at<float>(1,0))*(L.at<float>(9,0))+(L.at<float>(2,0))*(L.at<float>(10,0)));
    //float v0=((L.at<float>(4,0))*(L.at<float>(8,0))+(L.at<float>(5,0))*(L.at<float>(9,0))+(L.at<float>(6,0))*(L.at<float>(10,0)));
    
    //计算fu fv
    //一种计算方式
    //float fu=sqrt(((pow(L.at<float>(0,0),2))+(pow(L.at<float>(0,1),2))+(pow(L.at<float>(0,2),2)))/(pow(L.at<float>(0,8), 2)+pow(L.at<float>(0,9), 2)+pow(L.at<float>(0,10), 2))-pow(u0, 2));
    //float fv=sqrt(((pow(L.at<float>(0,4),2))+(pow(L.at<float>(0,5),2))+(pow(L.at<float>(0,6),2)))/(pow(L.at<float>(0,8), 2)+pow(L.at<float>(0,9), 2)+pow(L.at<float>(0,10), 2))-pow(v0, 2));
    //另一种计算方式
    float fu=sqrt((pow(u0*L.at<float>(8,0)-L.at<float>(0,0), 2)+pow(u0*L.at<float>(9,0)-L.at<float>(1,0), 2)+pow(u0*L.at<float>(10,0)-L.at<float>(2,0), 2))/(pow(L.at<float>(0,8), 2)+pow(L.at<float>(0,9), 2)+pow(L.at<float>(0,10), 2)));
    float fv=sqrt((pow(v0*L.at<float>(8,0)-L.at<float>(4,0), 2)+pow(v0*L.at<float>(9,0)-L.at<float>(5,0), 2)+pow(v0*L.at<float>(10,0)-L.at<float>(6,0), 2))/(pow(L.at<float>(0,8), 2)+pow(L.at<float>(0,9), 2)+pow(L.at<float>(0,10), 2)));
    
    Mat K=(Mat_<float>(3, 3)<<fu,0,u0,0,fv,v0,0,0,1);//内参数矩阵
    cout<<endl<<"内参数矩阵："<<endl<<K<<endl;
    
    
    //外参数计算,根据上一步计算得到的K矩阵和L矩阵计算得到
    //计算R参数
    Mat L1=(Mat_<float>(1, 3)<<L.at<float>(0,0),L.at<float>(1,0),L.at<float>(2,0));
    Mat L2=(Mat_<float>(1, 3)<<L.at<float>(4,0),L.at<float>(5,0),L.at<float>(6,0));
    Mat L3=(Mat_<float>(1, 3)<<L.at<float>(8,0),L.at<float>(9,0),L.at<float>(10,0));
    Mat r1=(L1-L3*u0)/fu;
    Mat r2=(L2-L3*v0)/fv;
    Mat r3=L3;
    
    //计算T参数
    float t1=(L.at<float>(0,3)-u0)/fu;
    float t2=(L.at<float>(0,7)-u0)/fv;
    float t3=1;
    
    Mat RT=(Mat_<float>(3, 4)<<r1.at<float>(0,0),r1.at<float>(0,1),r1.at<float>(0,2),t1,
            r2.at<float>(0,0),r2.at<float>(0,1),r2.at<float>(0,2),t2,
            r3.at<float>(0,0),r3.at<float>(0,1),r3.at<float>(0,2),t3);
    Mat L_34=(Mat_<float>(3, 4)<<L.at<float>(0,0),L.at<float>(1,0),L.at<float>(2,0),L.at<float>(3,0),L.at<float>(4,0),L.at<float>(5,0),L.at<float>(6,0),L.at<float>(7,0),L.at<float>(8,0),L.at<float>(9,0),L.at<float>(10,0),1);//L矩阵的3*4形式
    cout<<endl<<"外参数矩阵："<<endl<<(K.inv())*L_34/sqrt(pow(L.at<float>(0,8), 2)+pow(L.at<float>(0,9), 2)+pow(L.at<float>(0,10), 2))<<endl;
    //cout<<endl<<RT<<endl;
    
    
    //计算重投影误差
    //计算用于标定的坐标点重投影误差
    cout<<endl<<"标定点的误差计算：";
    float err_caliPoints[6]={0};
    float err_caliPoints_mean=0;
    float err_relative=0;
    float u_restruct,v_restruct;
    for (int i=0; i<6; i++) {
        u_restruct=(Points_cali[i][0]*L.at<float>(0,0)+Points_cali[i][1]*L.at<float>(1,0)+Points_cali[i][2]*L.at<float>(2,0)+L.at<float>(3,0))/(Points_cali[i][0]*L.at<float>(8,0)+Points_cali[i][1]*L.at<float>(9,0)+Points_cali[i][2]*L.at<float>(10,0)+1);
        v_restruct=(Points_cali[i][0]*L.at<float>(4,0)+Points_cali[i][1]*L.at<float>(5,0)+Points_cali[i][2]*L.at<float>(6,0)+L.at<float>(7,0))/(Points_cali[i][0]*L.at<float>(8,0)+Points_cali[i][1]*L.at<float>(9,0)+Points_cali[i][2]*L.at<float>(10,0)+1);
        err_caliPoints[i]=sqrt(pow(u_restruct-Points_cali[i][3], 2)+pow(v_restruct-Points_cali[i][4], 2));
        cout<<endl<<"标定点"<<i+1<<"的重投影误差："<<err_caliPoints[i]<<"   相对误差："<<abs(err_caliPoints[i])/sqrt(pow(Points_cali[i][3],2)+pow(Points_cali[i][4], 2))<<endl;
        err_caliPoints_mean=err_caliPoints_mean+err_caliPoints[i];
        err_relative=err_relative+abs(err_caliPoints[i])/sqrt(pow(Points_cali[i][3],2)+pow(Points_cali[i][4], 2));
    }
    cout<<"均值："<<err_caliPoints_mean/6<<"   "<<err_relative/6<<endl;
    
    //计算其余点的重投影误差
    cout<<endl<<"剩余点的误差计算：";
    float err_remainPoints[22]={0};
    float err_remainPoints_mean=0;
    float err_remainPoints_relative_mean=0;
    for (int i=0; i<22; i++) {
        u_restruct=(Points_remain[i][0]*L.at<float>(0,0)+Points_remain[i][1]*L.at<float>(1,0)+Points_remain[i][2]*L.at<float>(2,0)+L.at<float>(3,0))/(Points_remain[i][0]*L.at<float>(8,0)+Points_remain[i][1]*L.at<float>(9,0)+Points_remain[i][2]*L.at<float>(10,0)+1);
        v_restruct=(Points_remain[i][0]*L.at<float>(4,0)+Points_remain[i][1]*L.at<float>(5,0)+Points_remain[i][2]*L.at<float>(6,0)+L.at<float>(7,0))/(Points_remain[i][0]*L.at<float>(8,0)+Points_remain[i][1]*L.at<float>(9,0)+Points_remain[i][2]*L.at<float>(10,0)+1);
        err_remainPoints[i]=sqrt(pow(u_restruct-Points_remain[i][3], 2)+pow(v_restruct-Points_remain[i][4], 2));
        cout<<endl<<"标定点"<<i+1<<"的重投影误差："<<err_remainPoints[i]<<"   相对误差："<<abs(err_remainPoints[i])/sqrt(pow(Points_remain[i][3],2)+pow(Points_remain[i][4], 2))<<endl;
        err_remainPoints_mean=err_remainPoints_mean+err_remainPoints[i];
        err_remainPoints_relative_mean=err_remainPoints_relative_mean+abs(err_remainPoints[i])/sqrt(pow(Points_remain[i][3],2)+pow(Points_remain[i][4], 2));
    }
    cout<<"均值："<<err_remainPoints_mean/22<<"   "<<err_remainPoints_relative_mean/22<<endl;
    
    
    //释放空间
    for(int i = 0; i < 30;i++){
        free(*(Points+i));
    }
    return 0;
}
