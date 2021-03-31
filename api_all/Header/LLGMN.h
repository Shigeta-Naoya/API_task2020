#ifndef INC_LLGMN_H
#define INC_LLGMN_H

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

//学習回数
#define N  1000 //10000;
//二乗誤差
#define  setE  0.01
//学習率
#define EP 0.002 //0,001;
//各素子の値を持つ構造体
struct eldata {

	//教師データ(学習後は未学習データ)
	double **Tin;
	double **Tout;

	double **hin;
	double **mid;
	double **out;

	
};

//誤差伝搬時に各素子に入力される値
struct errordata {
	//double **in;
	//double ***mid;
	double **out;
};

//各素子間の重み
struct wdata {
	double **w;
};

int LLGMN_main();
//次元数の返還
void change(int n,int d, int h, struct eldata  *el);
//出力を計算
void learn(int n, int h, int m, int k, struct eldata *el, struct errordata *error, struct wdata *w);
//出力の計算と表示(未学習データ用)
void dislearn(int n, int h, int m, int k, struct eldata *el, struct errordata *error, struct wdata *w);
//誤差を求める
double colerror(int n, int k, struct eldata *el, struct errordata *error,FILE *fp);
//重みの更新
void weight(int n, int h, int m, int k, struct eldata *el, struct errordata *error, struct wdata *w);

#endif