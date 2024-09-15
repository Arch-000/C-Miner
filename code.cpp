#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<graphics.h>
#include<windows.h>
#include<math.h>
#include<time.h>
#define PI 3.1415926535898
#pragma comment(lib,"Winmm.lib")

IMAGE img_bk, img_button, img_game_bk_leve0, img_man_mask, img_man, img_brick, img_button1, img_button2, img_button3;
IMAGE img_hook, img_hook_mask, img_mine1, img_mine1_mask, img_mine2, img_mine2_mask, img_stone1, img_stone1_mask, img_pass;
IMAGE img[7], img_over, img_11, img_12;
MOUSEMSG m;
time_t t1, t2;                      //时间相关变量
int  sum, pas, left;                //时间相关变量
int button_x, button_y;             //定义开始按钮的坐标
int gamestate;                      //是否在游戏中(0=不在游戏中，1=游戏中，2=关卡中间)
int pass;                           //进入游戏判断
int user;                           //是否登入
int man_x, man_y;                   //矿工的坐标
char s[20];                         //用户名
char str[1000];                     //计时相关
int tempx, tempy;                   //钩爪相关
int score;                          //用户得分
int exe;                            //是否执行程序
int start;                          //是否开始游戏
int level;                          //


struct Hook
{
	int x, y;                       //钩爪起点
	int x1, y1;                     //钩爪末端
	int len;                        //钩爪长度
	int dir;                        //钩爪向左还是向右转
	double angle;                   //角度
	int vx, vy;                     //钩爪伸长速度
	int state1;                     //钩爪主状态  Change or Swing
	int state2;                     //钩爪次状态
}hook;

enum state
{
	Right,                          //钩爪右转
	Left,                           //钩爪左转
	Swing,                          //钩爪摆动
	Long,                           //钩爪伸长
	Short,                          //钩爪缩短
	Change,                         //钩爪不再是摆动
	gold,                           //物品类型金子
	stone,                          //物品类型石头            
	demond,                         //物品类型钻石
};
struct Obj
{
	int x;                          //物体坐标
	int y;
	int size1;                      //长
	int size2;                      //宽
	int exis;                       //是否存在
	int type;                       //物品类型
	int over;                       //是否不再出现
	int score;                      //分值
}Obj[7];

typedef struct LNode {
	int x;                           //矿石坐标x
	int y;                           //矿石坐标y
	int z;                           //矿石类型
	struct LNode* next;
}LNode, * LinkList;
LinkList creatlist();


void userin();//用户登录
void show();
void updateWithoutInput();
void updateWithInput();
void times();//计时
void Drawbutton();//按钮绘制
void grap();//钩爪抓取
void hookRock();//过抓摆动
void minedraw();//绘制矿石
void WriteFile(int a);//写入文件
int ReadFile();//读取文件


void startup()
{
	initgraph(790, 580);
	loadimage(&img_bk, "./image/start.png");
	loadimage(&img_button, "./image/startbottom1.png");
	loadimage(&img_game_bk_leve0, "./image/level-background-0.jpg", 790, 580);
	loadimage(&img_man_mask, "./image/char1_mask.jpg");
	loadimage(&img_man, "./image/char1.jpg");
	loadimage(&img_brick, "./image/brick.png");
	loadimage(&img_button1, "./image/start1.png", 110, 55);
	loadimage(&img_button2, "./image/pause.png", 110, 55);
	loadimage(&img_button3, "./image/exit.jpg", 110, 55);
	loadimage(&img_hook_mask, "./image/hook_mask.bmp");
	loadimage(&img_hook, "./image/hook.bmp");
	loadimage(&img_pass, "./image/pass.png", 790, 580);
	loadimage(&img_over, "./image/over1.png", 790, 580);
	loadimage(&img_11, "./image/21.jpg");
	loadimage(&img_12, "./image/22.jpg");
	//加载矿石图片
	for (int i = 1; i < 7; i++)
	{
		char addr[1000] = "";
		sprintf_s(addr, "./image/%d.png", i);
		loadimage(&img[i], addr);
		Obj[i].over = 0;
		Obj[i].exis = 1;
	}

	gamestate = 0;//未进入游戏
	user = 0;//未登入
	score = 0;//得分清零
	button_x = 100;//开始按钮坐标
	button_y = 100;
	//初始化矿工
	man_x = 340;
	man_y = 30;
	//初始化钩爪
	hook.x = 369;
	hook.y = 99;
	hook.angle = 0;
	hook.dir = Right;
	hook.len = 70;
	hook.state1 = Swing;
	exe = 1;//程序执行与否
	start = 0;//进入游戏界面而不执行
	//初始化时间
	sum = 60;
	pas = 0;
	left = 0;
	t1 = time(NULL);


	mciSendString("open ./image/233.mp3 alias bkmusic", NULL, 0, NULL);
	mciSendString("play bkmusic repeat ", NULL, 0, NULL);

	//矿石的随机位置
	srand((unsigned)time(NULL));
	for (int i = 1; i < 6; i++)
	{
		Obj[i].x = rand() % 700;
		Obj[i].y = rand() % 350 + 170;
		Obj[i].type = rand() % 3 + 1;
	}
}

void show()
{
	//未进入游戏界面
	if (gamestate == 0)
	{
		putimage(0, 0, &img_bk);
		putimage(button_x, button_y, &img_button);
	}
	//进入游戏界面
	if (gamestate == 1)
	{
		BeginBatchDraw();
		putimage(0, 0, &img_game_bk_leve0);
		putimage(man_x, man_y, &img_man_mask, NOTSRCERASE);
		putimage(man_x, man_y, &img_man, SRCINVERT);
		minedraw();

		//绘制砖块
		for (int j = 0; j < 15; j++)
		{
			putimage(j * 70, 100, &img_brick);
		}
		COLORREF fb = RGB(255, 185, 15);
		setbkcolor(fb);
		clearrectangle(790, 0, 1040, 580);

		Drawbutton();
		hookRock();
		grap();
		times();
	}

	FlushBatchDraw();
}

//用链表储存矿石的位置
LinkList creatlist()
{
	LinkList L = (LinkList)malloc(sizeof(LNode));
	LinkList r = NULL;
	L->next = NULL;
	r = L;
	for (int i = 1; i < 6; i++)
	{
		LinkList p = (LinkList)malloc(sizeof(LNode));
		p->x = Obj[i].x;
		p->y = Obj[i].y;
		p->z = Obj[i].type;
		p->next = NULL;
		r->next = p;
		r = p;
	}
	return L;
}

//利用链表遍历绘制矿石
void minedraw()
{
	LinkList p = NULL;
	p = creatlist();
	int i = 0;
	do
	{
		if (Obj[i].over == 0)
		{
			switch (p->z)
			{
			case 1:
				putimage(p->x, p->y, &img[3], NOTSRCERASE);
				putimage(p->x, p->y, &img[4], SRCINVERT);
				Obj[i].size1 = Obj[i].size2 = 43;
				Obj[i].score = 50;
				break;
			case 2:
				putimage(p->x, p->y, &img[1], NOTSRCERASE);
				putimage(p->x, p->y, &img[2], SRCINVERT);
				Obj[i].size1 = Obj[i].size2 = 56;
				Obj[i].score = 100;
				break;
			case 3:
				putimage(p->x, p->y, &img[5], NOTSRCERASE);
				putimage(p->x, p->y, &img[6], SRCINVERT);
				Obj[i].size1 = 31; Obj[i].size2 = 24;
				Obj[i].score = 200;
				break;
			}
		}
		i++;
		p = p->next;
	} while (p != NULL);
}

//用户登录
void userin()
{
	if (user == 0)
	{
		InputBox(s, 10, "输入用户名", "用户登录界面");
		user = 1;
		level = 1;
		WriteFile(0);
	}
}

//倒计时
void times()
{
	t2 = time(NULL);
	pas = t2 - t1;
	left = sum - pas;
	sprintf_s(str, "%d", left);
	outtextxy(900, 310, str);
}

//写入文件
void WriteFile(int a)
{
	FILE* fp;
	fp = fopen("./topscore.txt", "w");
	fprintf(fp, "%d", a);
	fclose(fp);
}
//读取文件
int ReadFile()
{
	int topscore;
	FILE* fp;
	fp = fopen("./topscore.txt", "r");
	if (fp == NULL)
	{
		WriteFile(0);
		return 0;
	}
	else
	{
		fscanf(fp, "%d", &topscore);
		fclose(fp);
		return topscore;
	}
}

//按钮绘制
void Drawbutton()
{
	if (gamestate == 1)
	{
		putimage(865, 23, &img_button1);
		putimage(865, 101, &img_button2);
		putimage(865, 179, &img_button3);
		setlinecolor(BLACK);
		setlinestyle(PS_SOLID);
		line(790, 254, 1040, 254);
		char name[20] = "用户名：";
		sprintf_s(name, "用户名：%s", s);
		outtextxy(830, 280, name);
		char time[20] = "倒计时：";
		outtextxy(830, 310, time);
		char score1[50];
		sprintf_s(score1, "得分：%d", score);
		outtextxy(830, 340, score1);
		char topscore[20];
		sprintf_s(topscore, "历史最高：%d", ReadFile());
		outtextxy(830, 370, topscore);
		char aim[20] = "目标得分：300";
		outtextxy(830, 400, aim);
		char lv[20];
		sprintf_s(lv, "关卡：%d", level);
		outtextxy(830, 430, lv);
	}
}

void updateWithInput()
{
	//用户登录
	if (gamestate == 0)
	{
		if (user == 0)
		{
			userin();
		}
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
		{
			if (m.x >= 100 && m.x <= 246 && m.y >= 100 && m.y <= 184)
			{
				gamestate = 1;
				initgraph(1040, 580);
				t1 = time(NULL);
			}
		}
	}
	//用户输入
	if (gamestate == 1)
	{
		//钩爪伸长
		if (_kbhit())
		{
			char input = _getch();
			if (input == ' ')
			{
				tempx = hook.x1;
				tempy = hook.y1;
				hook.state1 = Change;
				hook.state2 = Long;
			}
		}
	}
	//按钮点击
	if (MouseHit())
	{
		m = GetMouseMsg();
		FlushMouseMsgBuffer();
		if (m.uMsg == WM_LBUTTONDOWN)
		{
			if (gamestate == 1)
			{
				//Start
				if (m.x >= 865 && m.x <= 975 && m.y >= 23 && m.y <= 78)
				{
					exe *= -1;
					start = 1;
					sum = left;
					t1 = time(NULL);
				}
				//Stop
				if (m.x >= 865 && m.x <= 975 && m.y >= 98 && m.y <= 153)
				{
					exe *= -1;
					sum = left;
					t1 = time(NULL);
				}
				//Exit
				if (m.x >= 865 && m.x <= 975 && m.y >= 173 && m.y <= 228)
				{
					gamestate = 0;
					startup();
					initgraph(790, 580);
					user = 1;
				}
				//鼠标点击发射
				if (m.x >= 0 && m.x <= 790 && m.y >= 0 && m.y <= 580)
				{
					tempx = hook.x1;
					tempy = hook.y1;
					hook.state1 = Change;
					hook.state2 = Long;
				}
			}
			if (gamestate == 2)//通关成功
			{
				//Exit
				if (m.x >= 70 && m.x <= 391 && m.y >= 440 && m.y <= 625)
				{
					gamestate = 0;
					startup();
					initgraph(790, 580);
				}
				//下一关
				if (m.x >= 610 && m.x <= 720 && m.y >= 420 && m.y <= 530)
				{
					startup();
					gamestate = 1;
					initgraph(1040, 580);
				}
			}
			if (gamestate == 3)//通关失败
			{
				//Again
				if (m.x >= 190 && m.x <= 580 && m.y >= 310 && m.y <= 403)
				{
					startup();
					gamestate = 1;
					user = 1;
					initgraph(1040, 580);
				}
				//Exit
				if (m.x >= 190 && m.x <= 580 && m.y >= 404 && m.y <= 503)
				{
					startup();
					gamestate = 0;
					user = 1;
				}
			}
		}
	}
}

void updateWithoutInput()
{
	if (gamestate == 1)
	{
		//钩爪回弹
		if (hook.y1 >= 560 || hook.x1 >= 775 || hook.x1 <= 20)
		{
			hook.state1 = Change;
			hook.state2 = Short;
		}
		//钩爪恢复摆动
		if (hook.state2 == Short && hook.x1 == tempx && hook.y1 == tempy)
		{
			hook.state1 = Swing;
		}
		//初始游戏画面
		if (start == 0)
		{
			exe = -1;
		}
		//文件记录最高分数
		if (left == 0 && score > ReadFile())
		{
			WriteFile(score);
		}

		//游戏结束
		if (left == 0)
		{
			if (score >= 300)
			{
				initgraph(790, 580);
				gamestate = 2;
				putimage(0, 0, &img_pass);
				putimage(70, 440, &img_button3);
				putimage(610, 420, &img_12, NOTSRCERASE);
				putimage(610, 420, &img_11, SRCINVERT);
				level++;
			}
			else {
				initgraph(790, 580);
				putimage(0, 0, &img_over);
				gamestate = 3;
			}
		}

	}
}

//钩爪摆动
void hookRock()
{
	if (hook.state1 == Swing)
	{
		if (hook.dir == Right)//钩爪右摆
		{
			hook.angle += 0.80;
		}
		else { hook.angle -= 0.80; }//左摆
		//钩爪变向
		if (hook.angle >= 180)
		{
			hook.dir = Left;
		}
		if (hook.angle <= 0)
		{
			hook.dir = Right;
		}
		//钩爪绘制
		hook.x1 = hook.x - cos(PI / 180 * hook.angle) * hook.len;
		hook.y1 = hook.y + sin(PI / 180 * hook.angle) * hook.len;
		setlinecolor(BROWN);
		setlinestyle(PS_SOLID | PS_JOIN_BEVEL, 3);
		line(hook.x, hook.y, hook.x1, hook.y1);
		int a = hook.x1;
		int b = hook.y1;
		putimage(a - 20, b, &img_hook_mask, NOTSRCERASE);
		putimage(a - 20, b, &img_hook, SRCINVERT);
	}
	if (hook.state1 == Change)//钩爪主状态(非摆动)
	{
		hook.vx = cos(PI / 180 * hook.angle) * 4.5;
		hook.vy = sin(PI / 180 * hook.angle) * 4.5;
		if (hook.state2 == Long)//伸长
		{
			hook.x1 -= hook.vx;
			hook.y1 += hook.vy;
		}
		else
		{
			hook.x1 += hook.vx;
			hook.y1 -= hook.vy;
		}//缩短
		setlinecolor(BROWN);
		setlinestyle(PS_SOLID | PS_JOIN_BEVEL, 3);
		line(hook.x, hook.y, hook.x1, hook.y1);
		int a = hook.x1;
		int b = hook.y1;
		putimage(a - 20, b, &img_hook_mask, NOTSRCERASE);
		putimage(a - 20, b, &img_hook, SRCINVERT);
	}

	Sleep(20);
}

//钩爪抓取
void grap()
{
	for (int i = 1; i < 6; i++)
	{
		if (Obj[i].exis == 1 && hook.x1 > Obj[i].x && hook.x1<(Obj[i].x + Obj[i].size1)
			&& hook.y1>Obj[i].y && hook.y1 < (Obj[i].y + Obj[i].size2))
		{
			hook.state2 = Short;
			Obj[i].exis = 0;
		}//是否抓到
		if (Obj[i].exis == 0 && Obj[i].over == 0)
		{
			Obj[i].x = hook.x1 - Obj[i].size1 / 2;
			Obj[i].y = hook.y1;
			if (hook.state2 == Short && hook.x1 == tempx && hook.y1 == tempy)
			{
				hook.state1 = Swing;
				Obj[i].over = 1;//物体不再出现
				score += Obj[i].score;
			}//钩爪恢复正常
		}//抓取过程
	}
}


int main()
{
	startup();
	while (1)
	{
		if (exe == 1)
		{
			show();
			updateWithoutInput();
		}
		updateWithInput();
	}
	return 0;
}

