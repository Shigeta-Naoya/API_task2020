#include <windows.h>		//Windows��
#include <windowsx.h>		//Windows��
//#include<consoleapi.h>
#include <stdio.h>			//���o�͗p
#include <process.h>		//�X���b�h�p
#include <stdlib.h>
#include<math.h>
#pragma comment(lib,"winmm.lib")//�����x�^�C�}

#include "../Resource/resource.h"		//���\�[�X�t�@�C��
#include"LLGMN.h"


//�萔�錾
#define DEF_APP_NAME	TEXT("Waveform Test")
#define DEF_MUTEX_NAME	DEF_APP_NAME			//�~���[�e�b�N�X��
#define DEF_SAMPLING_RATE 1000
#define DEF_DATAPERS 20	//��ms��1�f�[�^�o�͂��邩
#define DEF_FPS 60	//1sec�ɉ��f�[�^�o�͂��邩

#define DEF_WAVE_RATE 200//�g�`�̔{��

#define CH_NUM 4		//ch��
#define CLASS_NUM 4		//class��

#define EVENT_ROW TEXT("row")		//�C�x���g�Ǘ��p
#define EVENT_MVC TEXT("mvc")		//�C�x���g�Ǘ��p
#define EVENT_PATTERN TEXT("pattern")		//�C�x���g�Ǘ��p
#define EVENT_LLGMN TEXT("LLGMN")		//�C�x���g�Ǘ��p
#define EVENT_PROB TEXT("prob")		//�C�x���g�Ǘ��p

#define FNAME_ROW "row_data.txt"
#define FNAME_MVC "mvc_data.txt"
#define FNAME_PATTERN "pattern_data.txt"
#define FNAME_PROB "prob_result.txt"

#define PI 3.14

//�֐��錾
BOOL CALLBACK MainDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );	//���C���_�C�A���O�v���V�[�W��
BOOL WinInitialize( HINSTANCE hInst, HWND hPaWnd, HMENU chID, char *cWinName, HWND PaintArea, WNDPROC WndProc ,HWND *hDC);//�q�E�B���h�E�𐶐�

UINT WINAPI ReadRowData( LPVOID thParam);								//���f�[�^�ǂݍ��ݗp�X���b�h
UINT WINAPI ReadMvcData(LPVOID thParam);								//mvc�f�[�^�ǂݍ��ݗp�X���b�h
UINT WINAPI ReadPatternData(LPVOID thParam);							//�p�^�[���f�[�^�ǂݍ��ݗp�X���b�h
UINT WINAPI Llgmn(LPVOID thParam);										//�p�^�[���f�[�^��LLGMN�ɓn��


UINT WINAPI DrowWave(LPVOID thParam);									//���g�`�`��p
UINT WINAPI DrowMvc(LPVOID thParam);									//MVC�`��p
UINT WINAPI DrowRadar(LPVOID thParam);									//���[�_�[�`��p
UINT WINAPI DrowProb(LPVOID thParam);									//����m���`��

void ButtonActivation(HWND hDlg);
void ButtonInvalidation(HWND hDlg, int ignore);

HRESULT CALLBACK WndProcWave(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);		//�q�E�B���h�E�v���V�[�W��
HRESULT CALLBACK WndProcMvc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);		//�q�E�B���h�E�v���V�[�W��
HRESULT CALLBACK WndProcRadar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);		//�q�E�B���h�E�v���V�[�W��



BOOL PlotWave(HWND hWnd, int x, int* y,int bx,int* by);								//�g�`�`��

BOOL PlotMvc(HWND hWnd, int xl, int* yt, int* xr, int* yb);							//MVC�o�[�`��
BOOL PlotRadar(HWND hWnd, int* x, int* y);							//���[�_�[�`���[�g�`��

//�R���\�[���̍쐬
void DispConsole();



//�ϐ��錾

typedef struct {
	HWND	hwnd;	//�_�C�A���O	
	HWND hPictWave;		//�E�B���h�E�n���h���iPictureBox�j
	HWND hPictMvc;		//�E�B���h�E�n���h���iPictureBox�j
	HWND hPictRadar;		//�E�B���h�E�n���h���iPictureBox�j
	HWND hPictProb;		//�E�B���h�E�n���h���iPictureBox�j

	HWND hWnd;		//�q�E�B���h�E�n���h���iwave�j
	HWND hWndWave;		//�q�E�B���h�E�n���h��
	HWND hWndMvc;		//�q�E�B���h�E�n���h��
	HWND hWndRadar;		//�q�E�B���h�E�n���h��
	HWND hWndProb;		//�q�E�B���h�E�n���h��


	BOOL waveFlag;
	BOOL rowDataFlag;
	int multiObjectsFlag;

	int modeID;		//�{�^���ɂ�镪��

	double rowData[CH_NUM];			//���g�`
	double mvcData[CH_NUM];			//%MVC
	double patternData[CH_NUM];		//�p�^�[���f�[�^
	double probResult[CLASS_NUM];	//�c�ʌ���


}SEND_POINTER_STRUCT;
COLORREF	color,colorEll, colorPen, bkcolor,axiscolor;	//�F