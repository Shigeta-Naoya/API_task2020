#include <windows.h>		//Windows環境
#include <windowsx.h>		//Windows環境
//#include<consoleapi.h>
#include <stdio.h>			//入出力用
#include <process.h>		//スレッド用
#include <stdlib.h>
#include<math.h>
#pragma comment(lib,"winmm.lib")//高精度タイマ

#include "../Resource/resource.h"		//リソースファイル
#include"LLGMN.h"


//定数宣言
#define DEF_APP_NAME	TEXT("Waveform Test")
#define DEF_MUTEX_NAME	DEF_APP_NAME			//ミューテックス名
#define DEF_SAMPLING_RATE 1000
#define DEF_DATAPERS 20	//何msに1データ出力するか
#define DEF_FPS 60	//1secに何データ出力するか

#define DEF_WAVE_RATE 200//波形の倍率

#define CH_NUM 4		//ch数
#define CLASS_NUM 4		//class数

#define EVENT_ROW TEXT("row")		//イベント管理用
#define EVENT_MVC TEXT("mvc")		//イベント管理用
#define EVENT_PATTERN TEXT("pattern")		//イベント管理用
#define EVENT_LLGMN TEXT("LLGMN")		//イベント管理用
#define EVENT_PROB TEXT("prob")		//イベント管理用

#define FNAME_ROW "row_data.txt"
#define FNAME_MVC "mvc_data.txt"
#define FNAME_PATTERN "pattern_data.txt"
#define FNAME_PROB "prob_result.txt"

#define PI 3.14

//関数宣言
BOOL CALLBACK MainDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );	//メインダイアログプロシージャ
BOOL WinInitialize( HINSTANCE hInst, HWND hPaWnd, HMENU chID, char *cWinName, HWND PaintArea, WNDPROC WndProc ,HWND *hDC);//子ウィンドウを生成

UINT WINAPI ReadRowData( LPVOID thParam);								//生データ読み込み用スレッド
UINT WINAPI ReadMvcData(LPVOID thParam);								//mvcデータ読み込み用スレッド
UINT WINAPI ReadPatternData(LPVOID thParam);							//パターンデータ読み込み用スレッド
UINT WINAPI Llgmn(LPVOID thParam);										//パターンデータをLLGMNに渡す


UINT WINAPI DrowWave(LPVOID thParam);									//生波形描画用
UINT WINAPI DrowMvc(LPVOID thParam);									//MVC描画用
UINT WINAPI DrowRadar(LPVOID thParam);									//レーダー描画用
UINT WINAPI DrowProb(LPVOID thParam);									//事後確率描画

void ButtonActivation(HWND hDlg);
void ButtonInvalidation(HWND hDlg, int ignore);

HRESULT CALLBACK WndProcWave(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);		//子ウィンドウプロシージャ
HRESULT CALLBACK WndProcMvc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);		//子ウィンドウプロシージャ
HRESULT CALLBACK WndProcRadar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);		//子ウィンドウプロシージャ



BOOL PlotWave(HWND hWnd, int x, int* y,int bx,int* by);								//波形描画

BOOL PlotMvc(HWND hWnd, int xl, int* yt, int* xr, int* yb);							//MVCバー描画
BOOL PlotRadar(HWND hWnd, int* x, int* y);							//レーダーチャート描画

//コンソールの作成
void DispConsole();



//変数宣言

typedef struct {
	HWND	hwnd;	//ダイアログ	
	HWND hPictWave;		//ウィンドウハンドル（PictureBox）
	HWND hPictMvc;		//ウィンドウハンドル（PictureBox）
	HWND hPictRadar;		//ウィンドウハンドル（PictureBox）
	HWND hPictProb;		//ウィンドウハンドル（PictureBox）

	HWND hWnd;		//子ウィンドウハンドル（wave）
	HWND hWndWave;		//子ウィンドウハンドル
	HWND hWndMvc;		//子ウィンドウハンドル
	HWND hWndRadar;		//子ウィンドウハンドル
	HWND hWndProb;		//子ウィンドウハンドル


	BOOL waveFlag;
	BOOL rowDataFlag;
	int multiObjectsFlag;

	int modeID;		//ボタンによる分岐

	double rowData[CH_NUM];			//生波形
	double mvcData[CH_NUM];			//%MVC
	double patternData[CH_NUM];		//パターンデータ
	double probResult[CLASS_NUM];	//議別結果


}SEND_POINTER_STRUCT;
COLORREF	color,colorEll, colorPen, bkcolor,axiscolor;	//色