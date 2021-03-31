#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"../Header/LLGMN.h"
#pragma warning(disable: 4996)

int LLGMN_main(void) {

	//1データの教師数,入力次元d,クラス数k,コンポーネント数m,入力データ数n,
	//int a = 200, d = 2, h = 1 + d*(d + 3) / 2, k = 4, m = 2 ,n = a * k;
	//int a = 1501, d = 4, h = 1 + d*(d + 3) / 2, k = 4, m = 2, n = a * k;
	int d = 4, h = 1 + d * (d + 3) / 2, k = 4, m = 2, n = 7951;//n = 1591;//


	int i, j, l;
	int count;
	eldata el;
	wdata w;
	double e;			//二乗誤差
	double E = 1;

	printf("各領域の確保\n");
	//**************************************各領域の確保***********************************************

	//教師データ
	//printf("教師データ\n");
	el.Tin = (double**)malloc(sizeof(double*) * n);
	el.Tin[0] = (double*)malloc(sizeof(double) * n * d);				//2次元
	for (i = 1; i < n; i++) {
		el.Tin[i] = el.Tin[0] + i * d;
	}

	el.Tout = (double**)malloc(sizeof(double*) * n);
	el.Tout[0] = (double*)malloc(sizeof(double) * n * k);				//2次元
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

	//printf("出力\n");
	el.out = (double**)malloc(sizeof(double) * n);
	el.out[0] = (double*)malloc(sizeof(double) * n * k);				//2次元
	for (i = 1; i < n; i++) {
		el.out[i] = el.out[0] + i * k;
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < k; j++) {
			el.out[i][j] = 0;
		}
	}

	//非線形変換後 X...hin
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



	//printf("中間層\n");
	el.mid = (double**)malloc(sizeof(double*) * n);
	el.mid[0] = (double*)malloc(sizeof(double) * n * (k * m));	//2次元
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

	error.out = (double**)malloc(sizeof(double*) * n);						//1次元
	error.out[0] = (double*)malloc(sizeof(double) * n * k);
	for (i = 1; i < n; i++) {
		error.out[i] = error.out[0] + i * k;
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < k; j++) {
			error.out[i][k] = 0;
		}
	}



	printf("重み\n");
	//重みの領域確保と初期化
	w.w = (double**)malloc(sizeof(double*) * h);
	w.w[0] = (double*)malloc(sizeof(double) * h * (k * m));						//2次元
	for (i = 1; i < h; i++) {
		w.w[i] = w.w[0] + i * (k * m);
	}


	//乱数の初期化
	srand((unsigned int)time(NULL));

	printf("重みの初期値\n");
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

	//printf("領域の確保が完了\n");

	FILE* fp;
	//if ((fp = fopen("lea_sig.csv", "r")) == NULL) {
	//	printf("file open error!!\n");
	//	exit(EXIT_FAILURE);	/* (3)エラーの場合は通常、異常終了する */
	//}

	//printf("教師データ\n");
	//i = 0;
	//while ((fscanf_s(fp, "%lf,%lf", &el.Tin[i][0], &el.Tin[i][1])) != EOF) {
	//	//printf("%f %f\n", el.Tin[i][0], el.Tin[i][1]);
	//	i++;
	//}
	//fclose(fp);

	/*printf("教師データ(入力)の取り込み完了\n");
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


	/***筋電**/
	//if ((fp = fopen("data", "r")) == NULL) {
	//if ((fp = fopen("Data.txt", "r")) == NULL) {
	if ((fp = fopen("Data.txt", "r")) == NULL) {
		printf("file open error!!\n");
		exit(EXIT_FAILURE);	/* (3)エラーの場合は通常、異常終了する */
	}

	/*printf("教師データ読み込み\n");
	i = 0;*/
	/*while ((fscanf_s(fp, "%lf %lf %lf %lf", &el.Tin[i][0], &el.Tin[i][1], &el.Tin[i][2], &el.Tout[i])) != EOF) {
	printf("%f %f %f %f\n", el.Tin[i][0], el.Tin[i][1], el.Tin[i][2], el.Tout[i]);
	i++;
	}*/
	for (i = 0; i < n; i++) {
		for (j = 0; j < d; j++) {
			//教師入力データ読み込み
			if (fscanf_s(fp, "%lf", &el.Tin[i][j]) == EOF) {
				printf("EOF!\n");
				fclose(fp);
				exit(EXIT_FAILURE);	/* (3)エラーの場合は通常、異常終了する */
			}
		}
		//printf("%d  ",i);
		for (l = 0; l < k; l++) {

			//教師出力データ読み込み
			if (fscanf_s(fp, "%lf", &el.Tout[i][l]) == EOF) {
				printf("EOF!\n");
				fclose(fp);
				exit(EXIT_FAILURE);	/* (3)エラーの場合は通常、異常終了する */
			}
			//printf("%f  ", el.Tout[i][l]);
		}
		//printf("\n");

	}

	fclose(fp);


	printf("教師データ(出力)の設定完了\n");

	//**********************************入力を非線形変換してhinへ*********************************************
	
	change(n,d, h, &el);

	//*********************************非線形変換の終了*******************************************************


	//誤差のファイル出力のため
	if ((fp = fopen("error.csv", "w")) == NULL) {
		printf("file open error!!\n");
		exit(EXIT_FAILURE);	/* (3)エラーの場合は通常、異常終了する */
	}



	//**************************************ループ***********************************************************
	count = 0;
	printf("学習\n");
	do{

		//重みをかけて中間層へ
		//中間層から出力へ
		learn(n,h,m,k, &el,&error,&w);

		//誤差を計算
		e = colerror(n, k, &el, &error,fp);

		//教師データと比較し、重みを修正
		weight(n,h, m, k, &el, &error, &w);

		printf("count %d  ", count);
		count++;
	} while (count < N && e > setE);

	//**************************************ループ***********************************************************

	printf("\n");

	fclose(fp);

	//未学習データを与える

	//if ((fp = fopen("dis_sig.csv", "r")) == NULL) {
	//	printf("file open error!!\n");
	//	exit(EXIT_FAILURE);	/* (3)エラーの場合は通常、異常終了する */
	//}

	//////printf("教師データ\n");
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
		exit(EXIT_FAILURE);	/* (3)エラーの場合は通常、異常終了する */
	}

	for (i = 0; i < n; i++) {
		for (j = 0; j < d; j++) {
			//教師入力データ読み込み
			if (fscanf_s(fp, "%lf", &el.Tin[i][j]) == EOF) {
				printf("EOF!\n");
				fclose(fp);
				exit(EXIT_FAILURE);	/* (3)エラーの場合は通常、異常終了する */
			}
		}

	}


	printf("未学習データ(入力)の取り込み完了\n");
	//未学習データのを学習したネットワークに入れた時の出力
	//**********************************入力を非線形変換してhinへ*********************************************

	change(n, d, h, &el);

	//*********************************非線形変換の終了*******************************************************

	

	//出力を求める
	dislearn(n, h, m, k, &el, &error, &w);

	//エンター待ち
	do {
		printf("終了します。エンターキーを押して下さい。");
	} while (getchar() != '\n');


	//メモリの開放




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


//データ数n、変換後の次元h,コンポーネント数m,クラス数k,各素子の値el,重みw
void learn(int n,int h, int m, int k, struct eldata *el,struct errordata *error, struct wdata *w) {
	
	int i, j, l,c;
	double sumi = 0;		//中間層に入ってくる入力がの和
	
	/*
	for (i = 0; i < n; i++) {
		for (c = 0; c < k; c++) {
			error->out[i][c] = el->out[i][c] - el->Tout[i][c];
		}
	}
	*/

	//hinに重みをかけて中間層に渡す
	//データ数分
	for (i = 0; i < n; i++) {
		//中間層の数分(k*m)
		for (l = 0; l < k*m; l++) {
			el->mid[i][l] = 0;
			//非線形変換後の入力数分
			for (j = 0; j < h; j++) {
				el->mid[i][l] += w->w[j][l]*el->hin[i][j];
			}
		}
		//el->mid[i][l]には入力が入っている
		//各素子の出力を求めたい
		sumi = 0;
		for (l = 0; l<k*m; l++) {
			sumi += exp(el->mid[i][l]);
		}
		for (l = 0; l < k*m; l++) {
			el->mid[i][l] = exp(el->mid[i][l]) / sumi;
			//printf("mid  %f\n", el->mid[i][l]);
		}


		//出力は中間層からの入力をコンポーネント数ごとに分けて足したもの
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

	//すべてのデータに対して出力を求められた


}

//未学習用(結果を出力)
void dislearn(int n, int h, int m, int k, struct eldata *el, struct errordata *error, struct wdata *w) {

	int i, j, l, c;
	double sumi = 0;		//中間層に入ってくる入力がの和
	double max = 0;
	int maxclass = 0;

	FILE *fp;
	if ((fp = fopen("dis_out.csv", "w")) == NULL) {
		printf("file open error!!\n");
		exit(EXIT_FAILURE);	/* (3)エラーの場合は通常、異常終了する */
	}

	//hinに重みをかけて中間層に渡す
	//データ数分
	for (i = 0; i < n; i++) {
		//中間層の数分(k*m)
		for (l = 0; l < k*m; l++) {
			el->mid[i][l] = 0;
			//非線形変換後の入力数分
			for (j = 0; j < h; j++) {
				el->mid[i][l] += w->w[j][l] * el->hin[i][j];
			}
		}
		//el->mid[i][l]には入力が入っている
		//各素子の出力を求めたい
		sumi = 0;
		for (l = 0; l<k*m; l++) {
			sumi += exp(el->mid[i][l]);
		}
		for (l = 0; l < k*m; l++) {
			el->mid[i][l] = exp(el->mid[i][l]) / sumi;
			//printf("mid  %f\n", el->mid[i][l]);
		}


		//出力は中間層からの入力をコンポーネント数ごとに分けて足したもの
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

			//fprintfに変更?
			//printf("out[%d][%d] = %f\n", i, c, el->out[i][c]);
			
		//	fprintf(fp, "%f,", el->out[i][c]);
		}for (c = 0; c < k; c++) {
			printf("%f,  ", el->out[i][c]);
		}
		for (c = 0; c < k; c++) {
			printf("%f,　", el->Tout[i][c]);
		}
		printf("\n");
		//fprintf(fp, "\n");
		fprintf(fp, "%f,%f,%f,%f,%d\n", el->out[i][0], el->out[i][1], el->out[i][2], el->out[i][3],maxclass);


	}

	//すべてのデータに対して出力を求められた
	fclose(fp);

}

//誤差を求める
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

//重みを更新
void weight(int n, int h, int m, int k, struct eldata *el, struct errordata *error, struct wdata *w) {
	//iデータ数n、j次元数h、lコンポーネント数m,cクラス数
	int i, j, l,c;
	double sume = 0;		//重み更新のための誤差の和

	


	//次元数分
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
					//重みを更新
					//printf("w[%d][%d] = %f  ",j,c*m+l, w->w[j][c*m + l]);
					w->w[j][c*m + l] = w->w[j][c*m + l] - sume*EP;
					//printf("w = %f\n", w->w[j][c*m+l]);
				}
			}
		}
	}
}

