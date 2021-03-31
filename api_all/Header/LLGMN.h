#ifndef INC_LLGMN_H
#define INC_LLGMN_H

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

//�w�K��
#define N  1000 //10000;
//���덷
#define  setE  0.01
//�w�K��
#define EP 0.002 //0,001;
//�e�f�q�̒l�����\����
struct eldata {

	//���t�f�[�^(�w�K��͖��w�K�f�[�^)
	double **Tin;
	double **Tout;

	double **hin;
	double **mid;
	double **out;

	
};

//�덷�`�����Ɋe�f�q�ɓ��͂����l
struct errordata {
	//double **in;
	//double ***mid;
	double **out;
};

//�e�f�q�Ԃ̏d��
struct wdata {
	double **w;
};

int LLGMN_main();
//�������̕Ԋ�
void change(int n,int d, int h, struct eldata  *el);
//�o�͂��v�Z
void learn(int n, int h, int m, int k, struct eldata *el, struct errordata *error, struct wdata *w);
//�o�͂̌v�Z�ƕ\��(���w�K�f�[�^�p)
void dislearn(int n, int h, int m, int k, struct eldata *el, struct errordata *error, struct wdata *w);
//�덷�����߂�
double colerror(int n, int k, struct eldata *el, struct errordata *error,FILE *fp);
//�d�݂̍X�V
void weight(int n, int h, int m, int k, struct eldata *el, struct errordata *error, struct wdata *w);

#endif