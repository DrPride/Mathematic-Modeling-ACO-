//计算总路径的小脚本
#include<iostream>
#include<math.h>
#include<time.h>
#include<string.h>
using namespace std;
int g_Distance[8][8] = {
        { 0, 300, 360, 210, 530, 475, 500, 690},
        { 300, 0, 380, 270, 230, 285, 200, 390},
        { 360, 380, 0, 510, 230, 665, 490, 680},
        { 210, 270, 510, 0, 470, 265, 450, 640},
        { 530, 230, 230, 470, 0, 515, 260, 450},
        { 475, 285, 665, 265, 515, 0, 460, 650},
        { 500, 200, 490, 450, 260, 460, 0, 190},
        { 690, 390, 680, 640, 450, 650, 190, 0}
};//两两城市间距离

void Output(int* Isize)
{
        int distance = 0;
        for(int i = 1; i< 8 ; i++)
        {
                distance += g_Distance[Isize[i-1]][Isize[i]];
                cout<<distance<<endl;
        }
        cout<<distance<<endl;
}

int main()
{
        int isize[8]= {0,1,6,2,4,3,5,7};
        Output(isize);
}
