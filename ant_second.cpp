// AO.cpp : 定义控制台应用程序的入口点。
#pragma once

#include<iostream>
#include<math.h>
#include<time.h>
#include<string.h>
#include <windows.h>
#include <process.h>
#include <fstream>
#include <stdlib.h>
#include <cstdio>
#include <random>
#include<set>

using namespace std;
const double ALPHA = 1.0; //启发因子，信息素的重要程度
const double BETA = 2.0;  //期望因子，城市间距离的重要程度
const double ROU = 0.5; //信息素残留参数
//0 3 5 1 2 4 6 7
const int N_ANT_COUNT = 1000; //蚂蚁数量
const int N_IT_COUNT = 100000; //迭代次数
const int N_CITY_COUNT = 8; //城市数量

const double DBQ = 200.0; //总的信息素
const double DB_MAX = 10e9; //一个标志数，10的9次方

double g_Trial[N_CITY_COUNT][N_CITY_COUNT];
//两两城市间信息素，就是环境信息素


double g_Distance[N_CITY_COUNT][N_CITY_COUNT] = {
        { 0, 300, 360, 210, 530, 475, 500, 690},
        { 300, 0, 380, 270, 230, 285, 200, 390},
        { 360, 380, 0, 510, 230, 665, 490, 680},
        { 210, 270, 510, 0, 470, 265, 450, 640},
        { 530, 230, 230, 470, 0, 515, 260, 450},
        { 475, 285, 665, 265, 515, 0, 460, 650},
        { 500, 200, 490, 450, 260, 460, 0, 190},
        { 690, 390, 680, 640, 450, 650, 190, 0}
};//两两城市间距离
double g_Walk[N_CITY_COUNT][N_CITY_COUNT] = {
        {0,9,10.8,6.3,15.9,14.25,15,20.7},
        {9,0,11.4,8.1,6.9,8.55,6,11.7},
        {10.8,11.4,0,15.3,6.9,19.95,14.7,20.4},
        {6.3,8.1,15.3,0,14.1,7.95,13.5,19.2},
        {15.9,6.9,6.9,14.1,0,15.45,7.8,13.5},
        {14.25,8.55,19.95,7.95,15.45,0,13.8,19.5},
        {15,6,14.7,13.5,7.8,13.8,0,5.7},
        {20.7,11.7,20.4,19.2,13.5,19.5,5.7,0}
};
double bestLength = DB_MAX;

//set<double> timeCheck = [0,30,60,90,120,150,180,210,240];
/*
这里确认下距离矩阵对不对
*/

//eil51.tsp城市坐标数据

//检查节点是否都被访问
bool check(int* group)
{
        int sizeofgroup = sizeof(group);
        int flag = 0;
        for(int i=0; i<sizeofgroup-1; i++)
        {
                if(group[i]==0)
                {
                       flag+=1;
                }
        }
        if(flag == sizeofgroup-1)return true;
        else return false;
}

//返回指定范围内的随机整数
int rnd(int nLow, int nUpper)
{
        return nLow + (nUpper - nLow) * rand() / (RAND_MAX + 1);
}


//返回指定范围内的随机浮点数
double rnd(double dbLow, double dbUpper)
{
        double dbTemp = rand() / ((double)RAND_MAX + 1.0);
        return dbLow + dbTemp * (dbUpper - dbLow);
}

//返回浮点数四舍五入取整后的浮点数
double ROUND(double dbA)
{
        return(double)((int)(dbA+0.5));
}


random_device rd;
mt19937 gen(rd());

//生成一个符合截断正态分布的随机数
double normal_rdn(double lowwer, double upper, double v)        //下限，上限，方差
{

        double e = (lowwer + upper) / 2;
        normal_distribution<double> normal(e, v);
        double ans = (double)normal(gen);
        if(ans >= lowwer && ans <= upper) return ans;
        else return (double)normal_rdn(lowwer, upper, v);
}

//森林小剧场，dt = 30，且开放时间为整除30的数
//因为到湿地商业街还需时间，故t <= 240
//编号 3
bool theater(int t)
{

        if(t % 30 || t > 240)
                return false;
        return true;
}

//游客服务中心，10 <= dt <= 30，且t_end <= 240
//编号 1
bool s_center(int t, int dt)
{
        if(t + dt > 240)
                return false;
        return true;
}

//delta time Array：在每个景点停留时间上下限
double dtAry[N_CITY_COUNT][2] = {
    {0, 0},//0 source, ignore
    {10, 30},//1
    {20, 60},//2
    {30, 30},//3,constant int
    {30, 60},//4
    {20, 60},//5
    {30, 60},//6
    {30, 30}//7 destination, ignore it in problem 2
};

//返回一个介于上限和下限之间的rnd
int randTime(int CITY_NO)
{
    int a = dtAry[CITY_NO][0], b =dtAry[CITY_NO][1];
    return rnd(a,b);
}




//定义蚂蚁类
class CAnt
{
public:
        CAnt(void);
        ~CAnt(void);

public:

        int m_nPath[N_CITY_COUNT]; //蚂蚁走的路径
        double m_dbPathLength; //蚂蚁走过的路径长度
        double m_dbPathTime;//蚂蚁用的时间

        int m_nAllowedCity[N_CITY_COUNT]; //没去过的城市
        int m_nCurCityNo; //当前所在城市编号
        int m_nMovedCityCount; //已经去过的城市数量
        double m_nTimeCost[N_CITY_COUNT];//每个节点进入的时间
        double  m_nTimeEachCost[N_CITY_COUNT]; //= {0};//每个节点花的时间

public:

        int ChooseNextCity(); //选择下一个城市
        void Init();//初始化
        void Move();//蚂蚁在城市间移动
        void Search(); //搜索路径
        void CalPathLength(); //计算蚂蚁走过的路径长度
        void CalPathLengthTest();//测试用
        void CalPathTime();//计算蚂蚁用的时间
        void CalPathTimeTest();//test
};

//构造函数
CAnt::CAnt(void)
{
}

//析构函数
CAnt::~CAnt(void)
{
}

//初始化函数，蚂蚁搜索前调用
void CAnt::Init()
{
        for(int i = 0; i < N_CITY_COUNT; i++)
        {
                m_nAllowedCity[i] = 1; //设置全部城市为没有去过
                m_nPath[i] = 0; //蚂蚁走的路径全部设置为0
        }

        //蚂蚁走过的路径长度设置为0
        m_dbPathLength = 0.0;

        //选择一个出发城市
        m_nCurCityNo = 0;
        //m_nCurCityNo = rnd(0,N_CITY_COUNT);

        //把出发城市保存入路径数组中
        m_nPath[0] = m_nCurCityNo;

        //标识出发城市为已经去过了
        m_nAllowedCity[m_nCurCityNo] = 0;

        //每个节点所要花费的时间，初始点设为0
        m_nTimeEachCost[m_nCurCityNo] = 0;

        //已经去过的城市数量设置为1
        m_nMovedCityCount = 1;

        //把出发时间定为0
        m_nTimeCost[m_nCurCityNo] = 0;

}

//选择下一个城市
//返回值 为城市编号
int CAnt::ChooseNextCity()
{
        int nSelectedCity=-1; //返回结果，先暂时把其设置为-1

        //==============================================================================
        //计算当前城市和没去过的城市之间的信息素总和

        double dbTotal=0.0;
        double prob[N_CITY_COUNT]; //保存各个城市被选中的概率
        double temp_enteringTime[N_CITY_COUNT] = {0};
        double TimeHereCost = randTime(m_nCurCityNo);
        m_nTimeEachCost[m_nCurCityNo] = TimeHereCost;
        for (int i = 0; i < N_CITY_COUNT;i++)
        {
                temp_enteringTime[i] = m_nTimeCost[m_nCurCityNo] + g_Walk[m_nCurCityNo][i] + TimeHereCost;
                //对城市号进行确认，如果不符合条件直接概率调零
                if(i == 1)
                {
                        //我们需要临时时间
                        if(!s_center(temp_enteringTime[i],randTime(i)))
                        {
                                prob[i] = 0;
                                continue;
                        }
                }
                if(i == 3)
                {
                        if(!theater(temp_enteringTime[i]))
                        {
                                prob[i]=0;
                                continue;
                        }
                }
                if (m_nAllowedCity[i] == 1) //城市没去过
                {

                        prob[i]=pow(g_Trial[m_nCurCityNo][i],ALPHA)*pow(1.0/g_Distance[m_nCurCityNo][i],BETA);
                        //该城市和当前城市间的信息素
                        dbTotal=dbTotal+prob[i]; //累加信息素，得到总和
                }
                else //如果城市去过了，则其被选中的概率值为0
                {
                        prob[i]=0.01;
                }
        }

   //==============================================================================
   //进行轮盘选择
    double dbTemp=0.0;
    if (dbTotal> 0.0) //总的信息素值大于0
    {
       dbTemp=rnd(0.0,dbTotal); //取一个随机数

       for (int i=0;i<N_CITY_COUNT;i++)
       {
           if (m_nAllowedCity[i] == 1) //城市没去过
           {
               dbTemp=dbTemp-prob[i]; //这个操作相当于转动轮盘，如果对轮盘选择不熟悉，仔细考虑一下
               if (dbTemp < 0.0&& i < N_CITY_COUNT-1) //轮盘停止转动，记下城市编号，直接跳出循环
                {
                   nSelectedCity=i;
                   //发现不是终点则可以选
                   break;
                }
                else if(dbTemp < 0.0&& i == N_CITY_COUNT-1)
                {
                        if(check(m_nAllowedCity)){nSelectedCity=i;break;}
                        //确认是否全部都选过了
                       else break;
                }
           }
       }
    }

   //==============================================================================
   //如果城市间的信息素非常小 ( 小到比double能够表示的最小的数字还要小 )
   //那么由于浮点运算的误差原因，上面计算的概率总和可能为0
   //会出现经过上述操作，没有城市被选择出来
   //出现这种情况，就把第一个没去过的城市作为返回结果

   //题外话：刚开始看的时候，下面这段代码困惑了我很长时间，想不通为何要有这段代码，后来才搞清楚。
    if(nSelectedCity == -1)
    {
       for (int i=0;i<N_CITY_COUNT;i++)
       {
           if (m_nAllowedCity[i] == 1) //城市没去过
           {
                //对i进行判断
               nSelectedCity=i;
               break;
           }
       }
    }

   //==============================================================================
   //返回结果，就是城市的编号
        //记录进入时间
        m_nTimeCost[nSelectedCity] = temp_enteringTime[nSelectedCity];
    return nSelectedCity;
}


//蚂蚁在城市间移动
void CAnt::Move()
{
        int nCityNo=ChooseNextCity(); //选择下一个城市

        m_nPath[m_nMovedCityCount]=nCityNo; //保存蚂蚁走的路
        m_nAllowedCity[nCityNo]=0;//把这个城市设置成已经去过了
        m_nCurCityNo=nCityNo; //改变当前所在城市为选择的城市
        m_nMovedCityCount++; //已经去过的城市数量加1
}

//蚂蚁进行搜索一次
void CAnt::Search()
{
        Init();//蚂蚁搜索前，先初始化

        //如果蚂蚁去过的城市数量小于城市数量，就继续移动
        while(m_nMovedCityCount < N_CITY_COUNT)
        {
           Move();
        }

        //完成搜索后计算走过的路径长度
        CalPathLength();
        CalPathTime();
        //printf("%.0lf\n", m_dbPathLength);
}


//计算蚂蚁走过的路径长度
void CAnt::CalPathLength()
{

        m_dbPathLength = 0.0; //先把路径长度置0
        int m = 0;
        int n = 0;
        for(int i = 1; i < N_CITY_COUNT; i++)
        {
                m = m_nPath[i];
                n = m_nPath[i-1];
                m_dbPathLength = m_dbPathLength + g_Distance[n][m];
        }

        //加上从最后城市返回出发城市的距离
        /*
        n = m_nPath[0];
        m_dbPathLength = m_dbPathLength + g_Distance[m][n];
        */
}

//计算蚂蚁走过的路径长度
void CAnt::CalPathLengthTest()
{
        //
        m_dbPathLength = 0.0; //先把路径长度置0
        int m = 0;
        int n = 0;
        for(int i = 1; i < N_CITY_COUNT; i++)
        {
                m = m_nPath[i];
                n = m_nPath[i-1];
                m_dbPathLength = m_dbPathLength + g_Distance[n][m];
                cout<<n<<" "<<m<<" "<<" "<<g_Distance[n][m]<<" "<<m_dbPathLength<<endl;
        }
        //加上从最后城市返回出发城市的距离
        /*
        n = m_nPath[0];
        m_dbPathLength = m_dbPathLength + g_Distance[m][n];
        */
}

//计算蚂蚁游玩的时间
void CAnt::CalPathTime()
{

        m_dbPathTime = 0.0; //先置0
        int m = 0;
        int n = 0;
        for(int i = 0; i < N_CITY_COUNT; i++)
        {
                m = m_nTimeEachCost[i];
                m_dbPathTime = m_dbPathTime + m;
        }
        //cout<<m_dbPathTime<<endl;
        //加上从最后城市返回出发城市的距离
        /*
        n = m_nPath[0];
        m_dbPathLength = m_dbPathLength + g_Distance[m][n];
        */
}

//计算蚂蚁游玩的时间
void CAnt::CalPathTimeTest()
{

        m_dbPathTime = 0.0; //先置0
        int m = 0;
        int n = 0;
        for(int i = 0; i < N_CITY_COUNT; i++)
        {
                m = m_nTimeEachCost[i];
                m_dbPathTime = m_dbPathTime + m;
        }
        cout<<m_dbPathTime<<endl;
        //加上从最后城市返回出发城市的距离
        /*
        n = m_nPath[0];
        m_dbPathLength = m_dbPathLength + g_Distance[m][n];
        */
}

//tsp类
class CTsp
{
public:
        CTsp(void);
        ~CTsp(void);

public:
        CAnt m_cAntAry[N_ANT_COUNT]; //蚂蚁数组
        CAnt m_cBestAnt; //定义一个蚂蚁变量，用来保存搜索过程中的最优结果
                        //该蚂蚁不参与搜索，只是用来保存最优结果

public:

        //初始化数据
        void InitData();

        //开始搜索
        void Search();

        //更新环境信息素
        void UpdateTrial();
};


//构造函数
CTsp::CTsp(void)
{
}

CTsp::~CTsp(void)
{
}


//初始化数据
void CTsp::InitData()
{
        //先把最优蚂蚁的路径长度设置成一个很大的值
        m_cBestAnt.m_dbPathLength = DB_MAX;

        //计算两两城市间距离
        //这个是一开始就写好的

        //初始化环境信息素，先把城市间的信息素设置成一样
        //这里设置成1.0，设置成多少对结果影响不是太大，对算法收敛速度有些影响
        for(int i = 0; i < N_CITY_COUNT; i++)
        {
                for(int j = 0; j<N_CITY_COUNT; j++)
        {
                g_Trial[i][j] = 1.0;
       }
    }
}




//更新环境信息素

void CTsp::UpdateTrial()
{
        //临时数组，保存各只蚂蚁在两两城市间新留下的信息素
        double dbTempAry[N_CITY_COUNT][N_CITY_COUNT];
        memset(dbTempAry, 0, sizeof(dbTempAry)); //先全部设置为0

   //计算新增加的信息素,保存到临时数组里
    int m = 0;
    int n = 0;
    for(int i = 0; i < N_ANT_COUNT; i++) //计算每只蚂蚁留下的信息素
    {
           for (int j = 1; j < N_CITY_COUNT; j++)
           {
               m = m_cAntAry[i].m_nPath[j];
               n = m_cAntAry[i].m_nPath[j - 1];
               dbTempAry[n][m] = dbTempAry[n][m]+DBQ/m_cAntAry[i].m_dbPathLength;
               //要对时间点进行惩罚
               dbTempAry[m][n] = dbTempAry[n][m];
           }

           //最后城市和开始城市之间的信息素
           n = m_cAntAry[i].m_nPath[0];
           dbTempAry[n][m] = dbTempAry[n][m]+ DBQ/m_cAntAry[i].m_dbPathLength;
           dbTempAry[m][n] = dbTempAry[n][m];

    }

   //==================================================================
   //更新环境信息素
        for (int i = 0; i < N_CITY_COUNT; i++)
        {
                for (int j = 0; j < N_CITY_COUNT; j++)
                {
                        g_Trial[i][j] = g_Trial[i][j] * ROU + dbTempAry[i][j]; //最新的环境信息素 =留存的信息素 + 新留下的信息素
                }
        }
}


void CTsp::Search()
{

        char cBuf[256]; //打印信息用

        //在迭代次数内进行循环
        for(int i = 0; i < N_IT_COUNT; i++)
        {
                //每只蚂蚁搜索一遍
                for (int j = 0; j < N_ANT_COUNT;j++)
                {
                        m_cAntAry[j].Search();
                        //这里要让多个团体同时进行
                        //cout<<m_cAntAry[j].m_dbPathLength<<endl;
                        //查看变量，打断点
                }

                //保存最佳结果
                for (int j = 0; j < N_ANT_COUNT; j++)
                {
                        if(m_cAntAry[j].m_nPath[N_CITY_COUNT-1] != 7
                        ||m_cAntAry[j].m_nTimeCost[N_CITY_COUNT-1] >= 300)continue;
                         if(m_cAntAry[j].m_dbPathLength <2320 )
                        {
                                for(int k = 0; k<N_CITY_COUNT;k++)
                                {
                                        sprintf(cBuf, "%d ", m_cAntAry[j].m_nPath[k]);
                                        printf(cBuf);
                                }
                                cout<<"  "<<m_cAntAry[j].m_dbPathLength <<endl;
                        }
                        if ((330-(m_cAntAry[j].m_dbPathLength)*30) > (330-(m_cBestAnt.m_dbPathLength)*30))
                        {
                                m_cBestAnt = m_cAntAry[j];
                                for(int k = 0; k<N_CITY_COUNT;k++)
                                {
                                        sprintf(cBuf, "%d ", m_cAntAry[j].m_nPath[k]);
                                        //printf(cBuf);
                                }
                                cout<<endl;
                                bestLength = m_cAntAry[j].m_dbPathLength;
                                //m_cAntAry[j].CalPathLengthTest();
                                //m_cAntAry[j].CalPathTimeTest();
                                //cout<<endl;


                                //cout<<"\n"<<bestLength<<endl;

                        }
                }
                /*
                cout<<"********"<<endl;
                for (int j = 0; j < N_ANT_COUNT; j++)
                {
                                for(int k = 0; k<N_CITY_COUNT;k++)
                                {
                                        sprintf(cBuf, "%d ", m_cAntAry[j].m_nPath[k]);
                                        printf(cBuf);
                                }
                                bestLength = m_cAntAry[j].m_dbPathLength;
                                cout<<"\n"<<bestLength<<endl;

                }
                */
                //更新环境信息素
                UpdateTrial();

                //输出目前为止找到的最优路径的长度
                //cout<<m_cBestAnt.m_dbPathLength<<endl;
                //sprintf(cBuf, "\n[%d] %.0lf", i + 1, m_cBestAnt.m_dbPathLength);
                //printf(cBuf);
        }
        //cout<<bestLength<<endl;
        //printf(cBuf);
}



int main()
{
        //用当前时间点初始化随机种子，防止每次运行的结果都相同
        time_t tm;
        time(&tm);
        unsigned int nSeed = (unsigned int)tm;
        srand(nSeed);

        //开始搜索
        CTsp tsp;

        //g_distanc
        tsp.InitData(); //初始化
        tsp.Search(); //开始搜索

        //输出结果
        printf("\nThe best tour is :\n");

        char cBuf[128];

        for (int i = 0; i < N_CITY_COUNT; i++)
        {
                sprintf(cBuf, "%d ", tsp.m_cBestAnt.m_nPath[i]);
                if (i % 20 == 0)
                {
                        printf("\n");
                }

                printf(cBuf);
                if(i>0)
                {
                        //cout<<tsp.m_cBestAnt.m_nTimeCost[tsp.m_cBestAnt.m_nPath[i]]<<" "<<tsp.m_cBestAnt.m_nTimeCost[tsp.m_cBestAnt.m_nPath[i]]-tsp.m_cBestAnt.m_nTimeCost[tsp.m_cBestAnt.m_nPath[i-1]]
                        //-g_Walk[tsp.m_cBestAnt.m_nPath[i-1]][tsp.m_cBestAnt.m_nPath[i]]<< " "<<endl;
                        cout<<tsp.m_cBestAnt.m_nTimeEachCost[tsp.m_cBestAnt.m_nPath[i]] <<" "<<tsp.m_cBestAnt.m_nTimeCost[tsp.m_cBestAnt.m_nPath[i]]<<endl;
                }
                else
                {
                        cout<<tsp.m_cBestAnt.m_nTimeCost[tsp.m_cBestAnt.m_nPath[i]]<<" "<<tsp.m_cBestAnt.m_nTimeCost[tsp.m_cBestAnt.m_nPath[i]]<<endl;
                }
        }

        return 0;

}

