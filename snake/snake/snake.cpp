#include<stdio.h>
#include<graphics.h>
#include<conio.h>
#include <stdlib.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

/*
	课程内容：贪吃蛇
	知识点：结构体，循坏，函数，easyx，数组
	easyx 图形界面库！
*/
//做界面，创建一个窗口,图形窗口
#define SANKE_NUM 100//蛇的最大尺寸
#define HEIGHT 256
#define WIDTH 256
#define _CRT_SECURE_NO_WARNINGS


struct Snake
{
	int size; //蛇的尺寸
	int dir;//蛇的方向
	double speed;//蛇的速度
	POINT coor[SANKE_NUM];//坐标 蛇的坐标数组

}snake;
//食物的结构体
struct Food
{
	int x;
	int y;
	int r;//食物大小
	bool flag; //食物是否被吃
	DWORD color; //食物颜色
	int eatgrade;
}food;

enum DIR//枚举表示蛇的方向
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

void GameInit();
void GameDraw();
void snakemove();
void Eatfood();
int snakeDie();
void showGrade();

//数据的初始化
HWND hwnd = NULL;
void GameInit()
{
	//播放背景音乐
	mciSendString("open ./music/snakeattack.mp3 alias BGM",0,0,0);
	mciSendString("play BGM repeat", 0, 0, 0);
	srand(GetTickCount());//获取系统开机到现在的毫秒数
	//init 初始化 graph 图形窗口
	hwnd=initgraph(HEIGHT, WIDTH,SHOWCONSOLE);//SHOWCONSOLE显示控制台
	//初始化🐍,并且一开始有三节
	snake.size = 3;
	snake.speed = 5;
	snake.dir=RIGHT;
	for (int i = 0; i < snake.size; i++){
		snake.coor[i].x = 50-10*i;
		snake.coor[i].y = 10;
		printf("%d %d\n", snake.coor[i].x, snake.coor[i].y);
	}
	//初始化食物
	food.x = rand() % (HEIGHT - 20) + 10;
	food.y = rand() % (WIDTH - 20) + 10;
	food.color = RGB(1, 0, 0);
	food.r = rand() % 10 + 5;
	food.flag = true;
	food.eatgrade = 0; 
}

void GameDraw()
{
	//双缓冲绘图
	//BeginBatchDraw();
	//设置背景颜色 需要两步,第二步是清空绘图设备!
	setbkcolor(WHITE);
	cleardevice();
	//绘制蛇
	
	for (int i = 0; i < snake.size; i++){
		setfillcolor(rand() % 776);
		solidcircle(snake.coor[i].x, snake.coor[i].y, 5);
	}
	if (food.flag){//绘制食物
		solidcircle(food.x, food.y, food.r);
	}
	//EndBatchDraw();
}
//移动蛇
void snakemove()
{
	//移动是坐标发生改变
	for (int i = snake.size-1; i>0; i--)
	{
		snake.coor[i]= snake.coor[i-1];
	}
	switch (snake.dir)
	{
		case UP:
			snake.coor[0].y-=snake.speed;
			if (snake.coor[0].y+10 <= 0){
				snake.coor[0].y = WIDTH-10;
			}break;
		case DOWN:
			snake.coor[0].y+=snake.speed;
			if (snake.coor[0].y - 10 >= WIDTH) {
				snake.coor[0].y = 0+ 10;
			}break;
		case LEFT: 
			snake.coor[0].x-=snake.speed;
			if (snake.coor[0].x + 10 <= 0) {
				snake.coor[0].x = HEIGHT -10;
			} break;
		case RIGHT:
			snake.coor[0].x+=snake.speed;
			if (snake.coor[0].x - 10 >= HEIGHT) {
				snake.coor[0].x = 0 + 10;
			}break;
	}
}

//通过按键改变蛇的移动方向
void keyControl()
{
	//判断有没有按键，让它初始动不动,若有则真，无则假
	if (_kbhit())
	{
		switch (_getch())
		{
			//72 80 75 77上下左右键值
			//改变方向
		case 'W':
		case 'w':
		case 72:if(snake.dir!=DOWN)
				snake.dir = UP;
			break;
		case 'S':
		case 's':
		case 80:if (snake.dir != UP)
				snake.dir = DOWN;
			break;
		case 'A':
		case 'a':
		case 75:if (snake.dir != RIGHT)
				snake.dir = LEFT;
			break;
		case 'D':
		case 'd':
		case 77:if (snake.dir != LEFT)
				snake.dir = RIGHT;
			break;
		case ' ':
			while (1)
			{
				showGrade();
				if (_getch() == ' ')
					return;
			}
		}
	}
}
void Eatfood()
{
	if (food.flag&&snake.coor[0].x >= food.x-food.r &&snake.coor[0].x<=food.x+food.r&&
		snake.coor[0].y>=food.y-food.r&&snake.coor[0].y<=food.y+food.r)
	{
		food.eatgrade += 10;
		snake.size++;
		food.flag = false;
		snake.speed += 0.02;
	}
	//可以加一个分数,吃一个食物,加n分

}
void Drawfood()
{
	setfillcolor(GREEN);
	food.x = rand() % (WIDTH - 20) + 10;
	food.y = rand() % (HEIGHT - 20) + 10;
	food.color = RGB(1, 0, 0);
	food.r = rand() % 10 + 5;
	food.flag = true;
}
//游戏暂停
//蛇死亡,就是游戏结束时
int snakeDie()
{
	for (int i = 1; i < snake.size; i++)
	{
		if ((snake.coor[0].x < snake.coor[i].x+5) && (snake.coor[0].x > snake.coor[i].x-5
			&& (snake.coor[0].y < snake.coor[i].y + 5)&& (snake.coor[0].y > snake.coor[i].y - 5)))
		{
			outtextxy(50, 50, "你把你自己吃了!");
			MessageBox(hwnd, "GameOver!","自杀警告!",MB_OK);
			return 1;
		}
	}
	return 0;
}

void showGrade()
{
	char grade[100] = "0";
	sprintf_s(grade, "%d", food.eatgrade);
	setbkmode(TRANSPARENT);//文字为透明的方式
	settextcolor(LIGHTBLUE);//文字颜色
	outtextxy(560, 20, "分数:");
	outtextxy(560+50,20,grade);
}


int main()
{
	GameInit();
	while (1)
	{
		GameDraw();
		if (snakeDie()){
			break;
		}
		Eatfood();
		if (!food.flag){
			Drawfood();
		}
		keyControl();
		snakemove();
		showGrade();
		Sleep(50);
	}
	getchar();
	closegraph();

	return 0;
}