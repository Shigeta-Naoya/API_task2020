#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"../Header/LLGMN.h"
#pragma warning(disable: 4996)

int LLGMN_main(void) {

	//1�f�[�^�̋��t��,���͎���d,�N���X��k,�R���|�[�l���g��m,���̓f�[�^��n,
	//int a = 200, d = 2, h = 1 + d*(d + 3) / 2, k = 4, m = 2 ,n = a * k;
	//int a = 1501, d = 4, h = 1 + d*(d + 3) / 2, k = 4, m = 2, n = a * k;
	int d = 4, h = 1 + d * (d + 3) / 2, k = 4, m = 2, n = 7951;//n = 1591;//


	int i, j, l;
	int count;
	eldata el;
	wdata w;
	double e;			//���덷
	double E = 1;

	printf("�e�̈�̊m��\n");
	//**************************************�e�̈�̊m��***********************************************

	//���t�f�[�^
	//printf("���t�f�[�^\n");
	el.Tin = (double**)malloc(sizeof(double*) * n);
	el.Tin[0] = (double*)malloc(sizeof(double) * n * d);				//2����
	for (i = 1; i < n; i++) {
		el.Tin[i] = el.Tin[0] + i * d;
	}

	el.Tout = (double**)malloc(sizeof(double*) * n);
	el.Tout[0] = (double*)malloc(sizeof(double) * n * k);				//2����
	for (i = 1; i < n; i++) {
		el.Tout[i] = el.Tout[0] + i * k;
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < k; j++) {
			el.Tout[i][j] = 0;
		}
	}



	/*
	int count = 0;
	for (i = 0; i < data; i++) {
	for (j = 0; j < in; j++) {
	el.Tin[i][j] = count;
	printf("%f %f %f %f\n", el.Tin[i][0], el.Tin[i][1], el.Tin[i][2], el.Tout[i]);
	count++;
	}
	}
	*/

	//printf("�o��\n");
	el.out = (double**)malloc(sizeof(double) * n);
	el.out[0] = (double*)malloc(sizeof(double) * n * k);				//2����
	for (i = 1; i < n; i++) {
		el.out[i] = el.out[0] + i * k;
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < k; j++) {
			el.out[i][j] = 0;
		}
	}

	//����`�ϊ��� X...hin
	//printf("hin\n");
	el.hin = (double**)malloc(sizeof(double*) * n);
	el.hin[0] = (double*)malloc(sizeof(double) * n * h);
	for (i = 1; i < n; i++) {
		el.hin[i] = el.hin[0] + i * h;
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < h; j++) {
			el.hin[i][j] = 0;
		}
	}



	//printf("���ԑw\n");
	el.mid = (double**)malloc(sizeof(double*) * n);
	el.mid[0] = (double*)malloc(sizeof(double) * n * (k * m));	//2����
	for (i = 0; i < n; i++) {
		el.mid[i] = el.mid[0] + i * (k * m);
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < (k * m); j++) {
			el.mid[i][j] = 0;
			//printf("%f", el.mid[i][j]);
		}
	}
	//printf("NN\n");

	errordata error;

	error.out = (double**)malloc(sizeof(double*) * n);						//1����
	error.out[0] = (double*)malloc(sizeof(double) * n * k);
	for (i = 1; i < n; i++) {
		error.out[i] = error.out[0] + i * k;
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < k; j++) {
			error.out[i][k] = 0;
		}
	}



	printf("�d��\n");
	//�d�݂̗̈�m�ۂƏ�����
	w.w = (double**)malloc(sizeof(double*) * h);
	w.w[0] = (double*)malloc(sizeof(double) * h * (k * m));						//2����
	for (i = 1; i < h; i++) {
		w.w[i] = w.w[0] + i * (k * m);
	}


	//�����̏�����
	srand((unsigned int)time(NULL));

	printf("�d�݂̏����l\n");
	printf("w1\n");
	for (i = 0; i < h; i++) {
		for (j = 0; j < k * m; j++) {
			if (j == k * m - 1) {
				w.w[i][j] = 0;
			}
			else {
				w.w[i][j] = rand() / double(RAND_MAX) * 2 - 1;
				//w.w1[i][j] = 0.5;
			}
			printf("%f ", w.w[i][j]);
		}
		printf("\n");
	}

	//printf("�̈�̊m�ۂ�����\n");

	FILE* fp;
	//if ((fp = fopen("lea_sig.csv", "r")) == NULL) {
	//	printf("file open error!!\n");
	//	exit(EXIT_FAILURE);	/* (3)�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
	//}

	//printf("���t�f�[�^\n");
	//i = 0;
	//while ((fscanf_s(fp, "%lf,%lf", &el.Tin[i][0], &el.Tin[i][1])) != EOF) {
	//	//printf("%f %f\n", el.Tin[i][0], el.Tin[i][1]);
	//	i++;
	//}
	//fclose(fp);

	/*printf("���t�f�[�^(����)�̎�荞�݊���\n");
	for (i = 0; i < n; i++) {
		if (0 <= i && i < a) {
			el.Tout[i][0] = 1;
		}
		else if (a <= i && i < a * 2) {
			el.Tout[i][1] = 1;
		}else if (a * 2 <= i && i < a * 3) {
			el.Tout[i][2] = 1;
		}else if (a * 3 <= i && i < a * 4) {
			el.Tout[i][3] = 1;
		}
	}*/


	/***�ؓd**/
	//if ((fp = fopen("data", "r")) == NULL) {
	//if ((fp = fopen("Data.txt", "r")) == NULL) {
	if ((fp = fopen("Data.txt", "r")) == NULL) {
		printf("file open error!!\n");
		exit(EXIT_FAILURE);	/* (3)�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
	}

	/*printf("���t�f�[�^�ǂݍ���\n");
	i = 0;*/
	/*while ((fscanf_s(fp, "%lf %lf %lf %lf", &el.Tin[i][0], &el.Tin[i][1], &el.Tin[i][2], &el.Tout[i])) != EOF) {
	printf("%f %f %f %f\n", el.Tin[i][0], el.Tin[i][1], el.Tin[i][2], el.Tout[i]);
	i++;
	}*/
	for (i = 0; i < n; i++) {
		for (j = 0; j < d; j++) {
			//���t���̓f�[�^�ǂݍ���
			if (fscanf_s(fp, "%lf", &el.Tin[i][j]) == EOF) {
				printf("EOF!\n");
				fclose(fp);
				exit(EXIT_FAILURE);	/* (3)�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
			}
		}
		//printf("%d  ",i);
		for (l = 0; l < k; l++) {

			//���t�o�̓f�[�^�ǂݍ���
			if (fscanf_s(fp, "%lf", &el.Tout[i][l]) == EOF) {
				printf("EOF!\n");
				fclose(fp);
				exit(EXIT_FAILURE);	/* (3)�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
			}
			//printf("%f  ", el.Tout[i][l]);
		}
		//printf("\n");

	}

	fclose(fp);


	printf("���t�f�[�^(�o��)�̐ݒ芮��\n");

	//**********************************���͂����`�ϊ�����hin��*********************************************
	
	change(n,d, h, &el);

	//*********************************����`�ϊ��̏I��*******************************************************


	//�덷�̃t�@�C���o�͂̂���
	if ((fp = fopen("error.csv", "w")) == NULL) {
		printf("file open error!!\n");
		exit(EXIT_FAILURE);	/* (3)�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
	}



	//**************************************���[�v***********************************************************
	count = 0;
	printf("�w�K\n");
	do{

		//�d�݂������Ē��ԑw��
		//���ԑw����o�͂�
		learn(n,h,m,k, &el,&error,&w);

		//�덷���v�Z
		e = colerror(n, k, &el, &error,fp);

		//���t�f�[�^�Ɣ�r���A�d�݂��C��
		weight(n,h, m, k, &el, &error, &w);

		printf("count %d  ", count);
		count++;
	} while (count < N && e > setE);

	//**************************************���[�v***********************************************************

	printf("\n");

	fclose(fp);

	//���w�K�f�[�^��^����

	//if ((fp = fopen("dis_sig.csv", "r")) == NULL) {
	//	printf("file open error!!\n");
	//	exit(EXIT_FAILURE);	/* (3)�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
	//}

	//////printf("���t�f�[�^\n");
	//i = 0;
	//while ((fscanf_s(fp, "%lf,%lf", &el.Tin[i][0], &el.Tin[i][1])) != EOF) {
	//	//printf("%f %f\n", el.Tin[i][0], el.Tin[i][1]);
	//	i++;
	//}
	//fclose(fp);



	if ((fp = fopen("Datatest.txt", "r")) == NULL) {
	//if ((fp = fopen("Data.txt", "r")) == NULL) {
	//if ((fp = fopen("ReData.txt", "r")) == NULL) {
		printf("file open error!!\n");
		exit(EXIT_FAILURE);	/* (3)�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
	}

	for (i = 0; i < n; i++) {
		for (j = 0; j < d; j++) {
			//���t���̓f�[�^�ǂݍ���
			if (fscanf_s(fp, "%lf", &el.Tin[i][j]) == EOF) {
				printf("EOF!\n");
				fclose(fp);
				exit(EXIT_FAILURE);	/* (3)�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
			}
		}

	}


	printf("���w�K�f�[�^(����)�̎�荞�݊���\n");
	//���w�K�f�[�^�̂��w�K�����l�b�g���[�N�ɓ��ꂽ���̏o��
	//**********************************���͂����`�ϊ�����hin��*********************************************

	change(n, d, h, &el);

	//*********************************����`�ϊ��̏I��*******************************************************

	

	//�o�͂����߂�
	dislearn(n, h, m, k, &el, &error, &w);

	//�G���^�[�҂�
	do {
		printf("�I�����܂��B�G���^�[�L�[�������ĉ������B");
	} while (getchar() != '\n');


	//�������̊J��




	return 0;
}



//
void change(int n,int d, int h, struct eldata* el) {
	int i, j, l;
	int count;
	for (l = 0; l < n; l++) {
		//printf("l = %d\n", l);

		el->hin[l][0] = 1;
		//printf("el.hin[%d][0]  = %f\n",l, el.hin[l][0]);

		for (i = 0; i < d; i++) {
			el->hin[l][i + 1] = el->Tin[l][i];
			//printf("el.hin[%d][%d]  = %f\n", l,i + 1, el.hin[l][i + 1]);
		}
		//printf("\n");
		count = d + 1;
		for (i = 0; i < d; i++) {
			for (j = i; j < d; j++) {
				el->hin[l][count] = el->Tin[l][i] * el->Tin[l][j];
				//printf("el.Tin[%d][%d] * el.Tin[%d][%d] = el.hin[%d][%d] = %f\n",l, i,l, j,l, count, el.hin[l][count]);
				count++;
			}
			//printf("\n");
		}
	}
}


//�f�[�^��n�A�ϊ���̎���h,�R���|�[�l���g��m,�N���X��k,�e�f�q�̒lel,�d��w
void learn(int n,int h, int m, int k, struct eldata *el,struct errordata *error, struct wdata *w) {
	
	int i, j, l,c;
	double sumi = 0;		//���ԑw�ɓ����Ă�����͂��̘a
	
	/*
	for (i = 0; i < n; i++) {
		for (c = 0; c < k; c++) {
			error->out[i][c] = el->out[i][c] - el->Tout[i][c];
		}
	}
	*/

	//hin�ɏd�݂������Ē��ԑw�ɓn��
	//�f�[�^����
	for (i = 0; i < n; i++) {
		//���ԑw�̐���(k*m)
		for (l = 0; l < k*m; l++) {
			el->mid[i][l] = 0;
			//����`�ϊ���̓��͐���
			for (j = 0; j < h; j++) {
				el->mid[i][l] += w->w[j][l]*el->hin[i][j];
			}
		}
		//el->mid[i][l]�ɂ͓��͂������Ă���
		//�e�f�q�̏o�͂����߂���
		sumi = 0;
		for (l = 0; l<k*m; l++) {
			sumi += exp(el->mid[i][l]);
		}
		for (l = 0; l < k*m; l++) {
			el->mid[i][l] = exp(el->mid[i][l]) / sumi;
			//printf("mid  %f\n", el->mid[i][l]);
		}


		//�o�͂͒��ԑw����̓��͂��R���|�[�l���g�����Ƃɕ����đ���������
		c = -1;
		for (l = 0; l < k*m; l++) {
			if (l%m == 0) {
				if (l != 0) {
					//printf("out[%d][%d] = %f\n",i,c,el->out[i][c]);
				}
				c++;
				el->out[i][c] = 0;
			}
			el->out[i][c] += el->mid[i][l];
		}
		for (c = 0; c < k; c++) {
			//printf("out[%d][%d] = %f\n", i, c, el->out[i][c]);
		}

	}

	//���ׂẴf�[�^�ɑ΂��ďo�͂����߂�ꂽ


}

//���w�K�p(���ʂ��o��)
void dislearn(int n, int h, int m, int k, struct eldata *el, struct errordata *error, struct wdata *w) {

	int i, j, l, c;
	double sumi = 0;		//���ԑw�ɓ����Ă�����͂��̘a
	double max = 0;
	int maxclass = 0;

	FILE *fp;
	if ((fp = fopen("dis_out.csv", "w")) == NULL) {
		printf("file open error!!\n");
		exit(EXIT_FAILURE);	/* (3)�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
	}

	//hin�ɏd�݂������Ē��ԑw�ɓn��
	//�f�[�^����
	for (i = 0; i < n; i++) {
		//���ԑw�̐���(k*m)
		for (l = 0; l < k*m; l++) {
			el->mid[i][l] = 0;
			//����`�ϊ���̓��͐���
			for (j = 0; j < h; j++) {
				el->mid[i][l] += w->w[j][l] * el->hin[i][j];
			}
		}
		//el->mid[i][l]�ɂ͓��͂������Ă���
		//�e�f�q�̏o�͂����߂���
		sumi = 0;
		for (l = 0; l<k*m; l++) {
			sumi += exp(el->mid[i][l]);
		}
		for (l = 0; l < k*m; l++) {
			el->mid[i][l] = exp(el->mid[i][l]) / sumi;
			//printf("mid  %f\n", el->mid[i][l]);
		}


		//�o�͂͒��ԑw����̓��͂��R���|�[�l���g�����Ƃɕ����đ���������
		c = -1;
		for (l = 0; l < k*m; l++) {
			if (l%m == 0) {
				if (l != 0) {
					//printf("out[%d][%d] = %f\n",i,c,el->out[i][c]);
				}
				c++;
				el->out[i][c] = 0;
			}
			el->out[i][c] += el->mid[i][l];
		}
		max = 0;
		for (c = 0; c < k; c++) {
			if (max < el->out[i][c]) {
				max = el->out[i][c];
				maxclass = c+1;
			}

			//fprintf�ɕύX?
			//printf("out[%d][%d] = %f\n", i, c, el->out[i][c]);
			
		//	fprintf(fp, "%f,", el->out[i][c]);
		}for (c = 0; c < k; c++) {
			printf("%f,  ", el->out[i][c]);
		}
		for (c = 0; c < k; c++) {
			printf("%f,�@", el->Tout[i][c]);
		}
		printf("\n");
		//fprintf(fp, "\n");
		fprintf(fp, "%f,%f,%f,%f,%d\n", el->out[i][0], el->out[i][1], el->out[i][2], el->out[i][3],maxclass);


	}

	//���ׂẴf�[�^�ɑ΂��ďo�͂����߂�ꂽ
	fclose(fp);

}

//�덷�����߂�
double colerror(int n, int k, struct eldata *el, struct errordata *error,FILE *fp) {

	int i, c;
	double e = 0;
	for (i = 0; i < n; i++) {
		for (c = 0; c < k; c++) {
			error->out[i][c] = el->out[i][c] - el->Tout[i][c];
			//printf("errorout = %f\n", error->out[i][c]);
			e = e - el->Tout[i][c] * log(el->out[i][c]);
			//printf("e = %f\n");
		}
	}
	e /= n;
	printf("%f\n",e);
	fprintf(fp, "%f\n", e);
	//printf("%f\n", e);
	return e;
}

//�d�݂��X�V
void weight(int n, int h, int m, int k, struct eldata *el, struct errordata *error, struct wdata *w) {
	//i�f�[�^��n�Aj������h�Al�R���|�[�l���g��m,c�N���X��
	int i, j, l,c;
	double sume = 0;		//�d�ݍX�V�̂��߂̌덷�̘a

	


	//��������
	for (j = 0; j < h; j++) {
		for (c = 0; c < k; c++) {
			for (l = 0; l < m; l++) {
				if (c != k || l != m){
					sume = 0;
					for (i = 0; i < n; i++) {
						//printf("sume =  %f  ", sume);
						//printf("errorout %f  mid %f  hin %f  elout %f\n", error->out[i][c],el->mid[i][k*c+l],el->hin[i][j],el->out[i][c]);
						sume += error->out[i][c] * el->mid[i][c*m + l] * el->hin[i][j] / el->out[i][c];
						//printf("sume =  %f\n", sume);
					}
					//�d�݂��X�V
					//printf("w[%d][%d] = %f  ",j,c*m+l, w->w[j][c*m + l]);
					w->w[j][c*m + l] = w->w[j][c*m + l] - sume*EP;
					//printf("w = %f\n", w->w[j][c*m+l]);
				}
			}
		}
	}
}

