
#include "Header/Header.h"		//リソースファイル


//メイン関数(ダイアログバージョン)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HANDLE hMutex;
	HANDLE hEventrow, hEventMvc, hEventPattern,hEventLLGMN,hEventProb;

	//多重起動判定
	hMutex = CreateMutex(NULL, TRUE, DEF_MUTEX_NAME);		//ミューテックスオブジェクトの生成
	if (GetLastError() == ERROR_ALREADY_EXISTS) {				//2重起動の有無を確認
		MessageBox(NULL, TEXT("既に起動されています．"), NULL, MB_OK | MB_ICONERROR);
		return 0;											//終了
	}
	//イベントの生成
	hEventrow = CreateEvent(NULL, TRUE, FALSE, EVENT_ROW);
	hEventMvc = CreateEvent(NULL, TRUE, FALSE, EVENT_MVC);
	hEventPattern = CreateEvent(NULL, TRUE, FALSE, EVENT_PATTERN);
	hEventLLGMN= CreateEvent(NULL, TRUE, FALSE, EVENT_LLGMN);
	hEventProb = CreateEvent(NULL, TRUE, FALSE, EVENT_PROB);


	//ダイアログ起動
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, MainDlgProc);

	//イベントの後処理
	CloseHandle(hEventrow);
	CloseHandle(hEventMvc);
	CloseHandle(hEventPattern);

	return FALSE;			//終了
}

//メインプロシージャ（ダイアログ）
BOOL CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont;				//フォント
	//スレッドのハンドル
	static HANDLE hThreadReadRow;
	static HANDLE hThreadDrowWave;
	static HANDLE hThreadReadMvc;
	static HANDLE hThreadDrowMvc;
	static HANDLE hThreadReadPattern;
	static HANDLE hThreadDrowRadar;
	static HANDLE hThreadLLGMN;
	static HANDLE hThreadDrowProb;

	static UINT thIDReadRow,thIDDrowWave;
	static UINT thIDReadMvc, thIDDrowMvc;
	static UINT thIDReadPattern, thIDDrowRadar;
	static UINT thIDLLGMN,thIDDrowProb;

	static SEND_POINTER_STRUCT Sps;

	static TCHAR strTmp[1024];
	
	//ウィンドウ名用
	static char txtWave[] = "Wave";
	static char txtMvc[] = "Mvc";
	static char txtRadar[] = "Radar";
	static char txtProb[] = "Prob";
	static char txtAll[] = "All";
	//static HWND hEdit;

	switch (uMsg) {
	case WM_INITDIALOG:		//ダイアログ初期化
		Sps.hwnd = hDlg;
		//hEdit = GetDlgItem(hDlg, IDC_EDIT1);
		//Sps.hEdit = hEdit;
		return TRUE;

	case WM_COMMAND:		//ボタンが押された時
		Sps.modeID = LOWORD(wParam);	//計測モードの変更（スレッド用）

		switch (LOWORD(wParam)) {
			//ボタンによって分岐***********************************************************************
		case ID_START:			//OKボタン

			Sps.hPictWave = GetDlgItem(hDlg, IDC_WAVE);
			if (Sps.hWndWave == NULL) {
				WinInitialize(NULL, hDlg, (HMENU)110, txtWave, Sps.hPictWave, WndProcWave, &Sps.hWndWave); //初期化
			}

			//読み込みフラグ
			Sps.waveFlag = TRUE;
			//データ読み込みスレッド起動
			hThreadReadRow = (HANDLE)_beginthreadex(NULL, 0, ReadRowData, (PVOID)&Sps, 0, &thIDReadRow);   //_beginthreadex→スレッドを立ち上げる関数
			//描画スレッド
			hThreadDrowWave = (HANDLE)_beginthreadex(NULL, 0, DrowWave, (PVOID)&Sps, 0, &thIDDrowWave);

			//他のボタンの無効化
			//ButtonInvalidation(hDlg, ID_WAVE_START);
			//ボタン状態の変更
			//SetDlgItemText(hDlg, ID_WAVE_START, L"Stop");

			ButtonInvalidation(hDlg, ID_STOP);		//EnableWindowで入力を無効または有効にする。

			return TRUE;

		case ID_STOP:	//停止・再開ボタン

		/*　サスペンドカウンタ　**************************
		実行を許可するまでスレッドを動かさない。
		ResumeThread：　サスペンドカウンタを1減らす
		SuspendThread：　サスペンドカウンタを1増やす
		0のときは実行。それ以外は待機する。
		**************************************************/
			if (ResumeThread(hThreadReadRow) == 0) {					//停止中かを調べる(サスペンドカウントを１減らす)
				SuspendThread(hThreadReadRow);
				SetDlgItemText(hDlg, ID_STOP, TEXT("再開"));	//再開に変更
				ButtonActivation(hDlg);
			}
			else {
				//SuspendThread(hThreadRead);						//スレッドの実行を停止(サスペンドカウントを１増やす)
				SetDlgItemText(hDlg, ID_STOP, TEXT("停止"));	//停止に変更
				ButtonInvalidation(hDlg, ID_STOP);
			}
			if (ResumeThread(hThreadDrowWave) == 0) {					//停止中かを調べる(サスペンドカウントを１減らす)
				SuspendThread(hThreadDrowWave);					//スレッドの実行を停止(サスペンドカウントを１増やす)
			}
			else {

			}
			return TRUE;

		case ID_WAVE_START:		//波形描画
			//ボタンの状態（テキスト）を取得
			GetDlgItemText(hDlg, ID_WAVE_START, strTmp, 1024);	//ボタンの状態(テキスト)を取得
			if (lstrcmp(strTmp, L"WAVE") == 0) {
				Sps.hPictWave = GetDlgItem(hDlg, IDC_WAVE);
				if (Sps.hWndWave == NULL) {
					WinInitialize(NULL, hDlg, (HMENU)110, txtWave, Sps.hPictWave, WndProcWave, &Sps.hWndWave); //初期化
				}
				//読み込みフラグ
				Sps.waveFlag = TRUE;
				//データ読み込みスレッド起動
				hThreadReadRow = (HANDLE)_beginthreadex(NULL, 0, ReadRowData, (PVOID)&Sps, 0, &thIDReadRow);   //_beginthreadex→スレッドを立ち上げる関数
				//描画スレッド
				hThreadDrowWave = (HANDLE)_beginthreadex(NULL, 0, DrowWave, (PVOID)&Sps, 0, &thIDDrowWave);

				//他のボタンの無効化
				ButtonInvalidation(hDlg, ID_WAVE_START);
				//ボタン状態の変更
				SetDlgItemText(hDlg, ID_WAVE_START, L"Stop");
			}
			else {
				//読み込みフラグ
				Sps.waveFlag = FALSE;
				//ボタン状態の変更
				SetDlgItemText(hDlg, ID_WAVE_START, L"WAVE");
				ButtonActivation(hDlg);
			}
			return TRUE;

		case ID_MVC_START:		//MVCのバー表示
			GetDlgItemText(hDlg, ID_MVC_START, strTmp, 1024);	//ボタンの状態(テキスト)を取得
			if (lstrcmp(strTmp, L"MVC") == 0) {

				Sps.hPictMvc = GetDlgItem(hDlg, IDC_MVC);
				if (Sps.hWndMvc == NULL) {
					WinInitialize(NULL, hDlg, (HMENU)110, txtMvc, Sps.hPictMvc, WndProcMvc, &Sps.hWndMvc); //初期化
				}
				Sps.waveFlag = TRUE;
				//データ読み込みスレッド起動
				hThreadReadMvc = (HANDLE)_beginthreadex(NULL, 0, ReadMvcData, (PVOID)&Sps, 0, &thIDReadMvc);   //_beginthreadex→スレッドを立ち上げる関数
				//描画スレッド
				hThreadDrowMvc = (HANDLE)_beginthreadex(NULL, 0, DrowMvc, (PVOID)&Sps, 0, &thIDDrowMvc);

				//他のボタンの無効化
				ButtonInvalidation(hDlg, ID_MVC_START);
				//ボタン状態の変更
				SetDlgItemText(hDlg, ID_MVC_START, L"Stop");
			}
			else {
				//読み込みフラグ
				Sps.waveFlag = FALSE;
				//ボタン状態の変更
				SetDlgItemText(hDlg, ID_MVC_START, L"MVC");
				ButtonActivation(hDlg);
			}
			return TRUE;

		case ID_RADAR_START:	//レーダーチャートの表示
			GetDlgItemText(hDlg, ID_RADAR_START, strTmp, 1024);	//ボタンの状態(テキスト)を取得
			if (lstrcmp(strTmp, L"RADAR") == 0) {

				Sps.hPictRadar = GetDlgItem(hDlg, IDC_RADAR);
				if (Sps.hWndRadar == NULL) {
					WinInitialize(NULL, hDlg, (HMENU)110, txtRadar, Sps.hPictRadar, WndProcRadar, &Sps.hWndRadar); //初期化
				}
				Sps.waveFlag = TRUE;
				Sps.multiObjectsFlag = 1;
				//データ読み込みスレッド起動
				hThreadReadPattern = (HANDLE)_beginthreadex(NULL, 0, ReadPatternData, (PVOID)&Sps, 0, &thIDReadPattern);   //_beginthreadex→スレッドを立ち上げる関数
				//描画スレッド
				hThreadDrowRadar = (HANDLE)_beginthreadex(NULL, 0, DrowRadar, (PVOID)&Sps, 0, &thIDDrowRadar);

				//他のボタンの無効化
				ButtonInvalidation(hDlg, ID_RADAR_START);
				//ボタン状態の変更
				SetDlgItemText(hDlg, ID_RADAR_START, L"Stop");
			}
			else {
				//読み込みフラグ
				Sps.waveFlag = FALSE;
				//ボタン状態の変更
				SetDlgItemText(hDlg, ID_RADAR_START, L"RADAR");
				ButtonActivation(hDlg);
			}
			return TRUE;

		case ID_LLGMN_START:	//LLGMNの開始（コンソール）

			//すべてのボタンの無効化
			ButtonInvalidation(hDlg, -1);
			//コンソールの作成
			DispConsole();
			//LLGMNの学習部分の起動
			LLGMN_main();
			//コンソールの破棄（ウィンドウは消えない）
			FreeConsole();
			//ボタンの有効化
			ButtonActivation(hDlg);

			return TRUE;

		case ID_PROB_START:		//動作推定と結果の描画
			GetDlgItemText(hDlg, ID_PROB_START, strTmp, 1024);	//ボタンの状態(テキスト)を取得
			if (lstrcmp(strTmp, L"CHECK") == 0) {
				Sps.hPictRadar = GetDlgItem(hDlg, IDC_RADAR);
				if (Sps.hWndRadar == NULL) {
					WinInitialize(NULL, hDlg, (HMENU)110, txtRadar, Sps.hPictRadar, WndProcRadar, &Sps.hWndRadar); //初期化
				}
				//MVCと同じなので流用している
				Sps.hPictProb = GetDlgItem(hDlg, IDC_PROB);
				if (Sps.hWndProb == NULL) {
					WinInitialize(NULL, hDlg, (HMENU)110, txtProb, Sps.hPictProb, WndProcMvc, &Sps.hWndProb); //初期化
				}
				Sps.waveFlag = TRUE;
				Sps.multiObjectsFlag = 2;
				//パターンデータ読み込みスレッド起動
				hThreadReadPattern = (HANDLE)_beginthreadex(NULL, 0, ReadPatternData, (PVOID)&Sps, 0, &thIDReadPattern);   //_beginthreadex→スレッドを立ち上げる関数
				//LLGMNにパターンデータを渡すスレッド
				hThreadLLGMN = (HANDLE)_beginthreadex(NULL, 0, Llgmn, (PVOID)&Sps, 0, &thIDLLGMN);   //_beginthreadex→スレッドを立ち上げる関数
				//事後確率描画スレッド
				hThreadDrowProb = (HANDLE)_beginthreadex(NULL, 0, DrowProb, (PVOID)&Sps, 0, &thIDDrowProb);
				hThreadDrowRadar = (HANDLE)_beginthreadex(NULL, 0, DrowRadar, (PVOID)&Sps, 0, &thIDDrowRadar);

				//他のボタンの無効化
				ButtonInvalidation(hDlg, ID_PROB_START);
				//ボタン状態の変更
				SetDlgItemText(hDlg, ID_PROB_START, L"Stop");
			}
			else {
				//読み込みフラグ
				Sps.waveFlag = FALSE;
				//ボタン状態の変更
				SetDlgItemText(hDlg, ID_PROB_START, L"CHECK");
				ButtonActivation(hDlg);
			}
			return TRUE;

		case ID_ALL_START:		//波形描画，MVC，レーダーチャート，動作推定すべて行う
			GetDlgItemText(hDlg, ID_ALL_START, strTmp, 1024);	//ボタンの状態(テキスト)を取得
			if (lstrcmp(strTmp, L"ALL") == 0) {
				Sps.hPictWave = GetDlgItem(hDlg, IDC_WAVE);
				if (Sps.hWndWave == NULL) {
					WinInitialize(NULL, hDlg, (HMENU)110, txtWave, Sps.hPictWave, WndProcWave, &Sps.hWndWave); //初期化
				}
				Sps.hPictMvc = GetDlgItem(hDlg, IDC_MVC);
				if (Sps.hWndMvc == NULL) {
					WinInitialize(NULL, hDlg, (HMENU)110, txtMvc, Sps.hPictMvc, WndProcMvc, &Sps.hWndMvc); //初期化
				}
				Sps.hPictRadar = GetDlgItem(hDlg, IDC_RADAR);
				if (Sps.hWndRadar == NULL) {
					WinInitialize(NULL, hDlg, (HMENU)110, txtRadar, Sps.hPictRadar, WndProcRadar, &Sps.hWndRadar); //初期化
				}
				Sps.hPictProb = GetDlgItem(hDlg, IDC_PROB);
				if (Sps.hWndProb == NULL) {
					WinInitialize(NULL, hDlg, (HMENU)110, txtProb, Sps.hPictProb, WndProcMvc, &Sps.hWndProb); //初期化
				}
				Sps.waveFlag = TRUE;
				Sps.multiObjectsFlag = 2;
				//生波形データ読み込みスレッド起動
				hThreadReadMvc = (HANDLE)_beginthreadex(NULL, 0, ReadRowData, (PVOID)&Sps, 0, &thIDReadRow);   //_beginthreadex→スレッドを立ち上げる関数
				//波形描画スレッド
				hThreadDrowMvc = (HANDLE)_beginthreadex(NULL, 0, DrowWave, (PVOID)&Sps, 0, &thIDDrowWave);
				//MVCデータ読み込みスレッド起動
				hThreadReadMvc = (HANDLE)_beginthreadex(NULL, 0, ReadMvcData, (PVOID)&Sps, 0, &thIDReadMvc);   //_beginthreadex→スレッドを立ち上げる関数
				//MVCバー描画スレッド
				hThreadDrowMvc = (HANDLE)_beginthreadex(NULL, 0, DrowMvc, (PVOID)&Sps, 0, &thIDDrowMvc);
				//パターンデータ読み込みスレッド起動
				hThreadReadPattern = (HANDLE)_beginthreadex(NULL, 0, ReadPatternData, (PVOID)&Sps, 0, &thIDReadPattern);   //_beginthreadex→スレッドを立ち上げる関数
				//レーダーチャート描画スレッド
				hThreadDrowRadar = (HANDLE)_beginthreadex(NULL, 0, DrowRadar, (PVOID)&Sps, 0, &thIDDrowRadar);
				//LLGMNにパターンデータを渡すスレッド
				hThreadLLGMN = (HANDLE)_beginthreadex(NULL, 0, Llgmn, (PVOID)&Sps, 0, &thIDLLGMN);   //_beginthreadex→スレッドを立ち上げる関数
				//事後確率描画スレッド
				hThreadDrowProb = (HANDLE)_beginthreadex(NULL, 0, DrowProb, (PVOID)&Sps, 0, &thIDDrowProb);

				//他のボタンの無効化
				ButtonInvalidation(hDlg, ID_ALL_START);
				//ボタン状態の変更
				SetDlgItemText(hDlg, ID_ALL_START, L"Stop");
			}
			else {
				//読み込みフラグ
				Sps.waveFlag = FALSE;
				//ボタン状態の変更
				SetDlgItemText(hDlg, ID_ALL_START, L"ALL");
				ButtonActivation(hDlg);
			}
			return TRUE;

		case WM_CLOSE:
			EndDialog(hDlg, 0);			//ダイアログ終了
			return TRUE;
		}

	case WM_CLOSE:				//ウィンドウを閉じたときの処理
		EndDialog(hDlg, 0);
		return TRUE;
	}

	return FALSE;
}

//ボタンの有効化
void ButtonActivation(HWND hDlg) {
	int i;
	for (i = ID_START; i <= ID_ALL_START; i++) {
		EnableWindow(GetDlgItem(hDlg, i), TRUE);
	}
}
//ボタンの無効化
void ButtonInvalidation(HWND hDlg, int ignore) {
	int i;
	for (i = ID_START; i <= ID_ALL_START; i++) {
		if (i != ignore || ignore == -1) {
			EnableWindow(GetDlgItem(hDlg, i), FALSE);
		}
	}
}

//生データ読み込み用スレッド
UINT WINAPI ReadRowData(LPVOID thParam)
{
	int i;
	int error;
	static SEND_POINTER_STRUCT *FU = (SEND_POINTER_STRUCT*)thParam;        //構造体のポインタ取得

	HANDLE hEvent;		//イベント管理用
	FILE *fp;			//ファイルポインタ
	DWORD DNum = 0, beforeTime;
	beforeTime = timeGetTime();						//現在の時刻計算（初期時間）
	int count = 0;

	//イベントのハンドル取得
	//すべてのアクセス権
	//継承不可
	//EVENT_NAMEのイベント
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_ROW);
	ResetEvent(hEvent);
	//ファイルオープン
	if ((fopen_s(&fp, FNAME_ROW, "r")) != 0) {
		MessageBox(NULL, TEXT("Input File Open ERROR!"), NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//データ読み込み
	while (FU->waveFlag == TRUE) {
		DWORD nowTime, progress, idealTime;

		//時間の調整
		nowTime = timeGetTime();					//現在の時刻計算
		progress = nowTime - beforeTime;				//処理時間を計算
		idealTime = (DWORD)(DNum * (1000.0F / (double)DEF_SAMPLING_RATE));	//理想時間を計算
		if (idealTime > progress) Sleep(idealTime - progress);			//理想時間になるまで待機

		//データの読み込み
		//生波形
		for (i = 0; i < CH_NUM; i++) {
			error = fscanf_s(fp, "%lf", &FU->rowData[i]);
			if (error == EOF) {
				FU->rowDataFlag = FALSE;			//ループ終了フラグ
				FU->waveFlag = FALSE;
				MessageBox(NULL, TEXT("終了1"), TEXT("INFORMATION"), MB_OK | MB_ICONEXCLAMATION);
				//他のボタン有効化
				ButtonActivation(FU->hwnd);
				//ボタン状態の変更
				SetDlgItemText(FU->hwnd, ID_WAVE_START, L"WAVE");
				SetDlgItemText(FU->hwnd, ID_ALL_START, L"ALL");
				
				return FALSE;
			}
		}

		//イベントをセット(計測したよと知らせる)
		count++; 
		if (count % DEF_DATAPERS == 0) {
			SetEvent(hEvent);
			count = 0;
		}

		DNum++;

		//スレッドを止めた時に発生する時間のずれに対応
		//若干誤差はある
		if (DNum > 100) {
			DNum = 0;
			beforeTime = timeGetTime();
		}
	}
	//ボタン状態の変更
	SetDlgItemText(FU->hwnd, ID_WAVE_START, L"WAVE");
	SetDlgItemText(FU->hwnd, ID_ALL_START, L"ALL");
	//イベントの後処理
	CloseHandle(hEvent);

	return 0;
}

//波形描画用スレッド
UINT WINAPI DrowWave(LPVOID thParam) {
	int i;
	int errMsg;
	static SEND_POINTER_STRUCT* FU = (SEND_POINTER_STRUCT*)thParam;        //構造体のポインタ取得
	int width, hight;				//PICTBOXの幅
	RECT rect;
	int x_time = 0;
	int x, y[CH_NUM];				//プロットする座標
	int bx, by[CH_NUM];				//1時刻前の座標
	int xOffset, yOffset[CH_NUM];	//各座標の初期位置

	HANDLE hEvent;		//イベント管理用
	
	DWORD DNum = 0, beforeTime;
	DWORD nowTime, progress, idealTime;
	beforeTime = timeGetTime();						//現在の時刻計算（初期時間）

	//イベントのハンドル取得
	//すべてのアクセス権
	//継承不可
	//EVENT_NAMEのイベント
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_ROW);
	ResetEvent(hEvent);
	GetClientRect(FU->hWndWave, &rect);	//PICTBOXの情報を入手
	width = rect.left + rect.right;
	hight = rect.top + rect.bottom;

	//初期位置の指定
	xOffset = int(0.01 * width);
	bx = xOffset;
	x = xOffset;
	for (i = 0; i < CH_NUM; i++) {
		yOffset[i] = int(hight / (CH_NUM + 1) * (i + 1));
		by[i] = yOffset[i];
		y[i] = yOffset[i];
	}
	InvalidateRect(FU->hWndWave, NULL, FALSE);		//画面再描画
	//区間内でループ（ボタンが押されている間）
	while (FU->waveFlag == TRUE) {
		//次のデータが来るまで待機
		//WaitForSingleObject(hEvent, INFINITE);
		//時間の調整
		nowTime = timeGetTime();					//現在の時刻計算
		progress = nowTime - beforeTime;				//処理時間を計算
		idealTime = (DWORD)(DNum * (1000.0F / (double)DEF_FPS));	//理想時間を計算
		if (idealTime > progress) {
			Sleep(idealTime - progress);			//理想時間になるまで待機
		}
		//x座標
		x = x_time + xOffset;
		//描画
		//座標の計算
		for (i = 0; i < CH_NUM; i++) {
			y[i] = int(FU->rowData[i] * (-1) * DEF_WAVE_RATE + yOffset[i]);
		}
		PlotWave(FU->hWndWave, x, y, bx, by);
		ResetEvent(hEvent);
		bx = x;
		for (i = 0; i < CH_NUM; i++) {
			by[i] = y[i];
		}
		//x軸進める
		x_time++;
		
		DNum++;
		if (DNum > 100) {
			DNum = 0;
			beforeTime = timeGetTime();
		}

		//端に行ったかを確認
		if (x + 1 > width) {
			x_time = 0;
			bx = xOffset;
			//再描画
			//REPaintWave(FU->hWnd);
			InvalidateRect(FU->hWndWave, NULL, FALSE);		//画面再描画
		}
	}

	//イベントの後処理
	CloseHandle(hEvent);

	return 0;
}

//子ウィンドウプロシージャ
//軸を描く
HRESULT CALLBACK WndProcWave(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HBRUSH		hBrush, hOldBrush;	//ブラシ
	HPEN		hPen, hOldPen;		//ペン
	int i;							//作業用
	if (uMsg == WM_PAINT) {
		HDC hdc = BeginPaint(hWnd, &ps);
		int width, hight;

		color = RGB(0, 0, 0);		//黒
		axiscolor = RGB(255, 255, 255);
		hBrush = CreateSolidBrush(color);				//ブラシ生成
		hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);	//ブラシ設定
		hPen = CreatePen(PS_SOLID, 2, axiscolor);		//ペン生成
		hOldPen = (HPEN)SelectObject(hdc, hPen);		//ペン設定

		//PICTBOXの大きさ
		RECT rect;
		GetClientRect(hWnd, &rect);
		width = rect.left + rect.right;
		hight = rect.top + rect.bottom;

		//黒く塗りつぶす
		Rectangle(hdc, 0, 0, width + 1, hight + 1);

		//縦軸
		MoveToEx(hdc, width * 0.01, 0, NULL);
		LineTo(hdc, width * 0.01, hight);
		//横軸
		for (i = 0; i < CH_NUM; i++) {
			MoveToEx(hdc, width * 0.01, hight / (CH_NUM + 1) * (i + 1), NULL);
			LineTo(hdc, width, hight / (CH_NUM + 1) * (i + 1));
		}

		//デバイスコンテキストのハンドル破棄
		EndPaint(hWnd, &ps);

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);

	}
	return TRUE;
}

//波形描画
BOOL PlotWave(HWND hWnd, int x, int* y, int bx, int* by) {
	int i;
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = GetDC(hWnd);
	HBRUSH		hBrush, hOldBrush;	//ブラシ
	HPEN		hPen, hOldPen;		//ペン
	colorPen = RGB(255, 255, 0);
	hBrush = CreateSolidBrush(color);				//ブラシ生成
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);	//ブラシ設定
	hPen = CreatePen(PS_SOLID, 2, colorPen);		//ペン生成
	hOldPen = (HPEN)SelectObject(hdc, hPen);		//ペン設定

	//ここをループ
	for (i = 0; i < CH_NUM; i++) {
		MoveToEx(hdc, bx, by[i], NULL);
		LineTo(hdc, x, y[i]);
	}
	ReleaseDC(hWnd, hdc);

	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	

	return TRUE;
}


//mvcデータ読み込み用スレッド
UINT WINAPI ReadMvcData(LPVOID thParam)
{
	int i;
	int error;
	static SEND_POINTER_STRUCT* FU = (SEND_POINTER_STRUCT*)thParam;        //構造体のポインタ取得

	HANDLE hEvent;		//イベント管理用
	FILE* fp;			//ファイルポインタ
	DWORD DNum = 0, beforeTime;
	beforeTime = timeGetTime();						//現在の時刻計算（初期時間）
	int count = 0;

	//イベントのハンドル取得
	//すべてのアクセス権
	//継承不可
	//EVENT_NAMEのイベント
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_MVC);
	ResetEvent(hEvent);
	//ファイルオープン
	if ((fopen_s(&fp, FNAME_MVC, "r")) != 0) {
		MessageBox(NULL, TEXT("Input File Open ERROR!"), NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//データ読み込み
	while (FU->waveFlag == TRUE) {
		DWORD nowTime, progress, idealTime;

		//時間の調整
		nowTime = timeGetTime();					//現在の時刻計算
		progress = nowTime - beforeTime;				//処理時間を計算
		idealTime = (DWORD)(DNum * (1000.0F / (double)DEF_SAMPLING_RATE));	//理想時間を計算
		if (idealTime > progress) Sleep(idealTime - progress);			//理想時間になるまで待機

		//データの読み込み
		//MVC
		for (i = 0; i < CH_NUM; i++) {
			error = fscanf_s(fp, "%lf", &FU->mvcData[i]);
			if (error == EOF) {
				FU->waveFlag = FALSE;									//ループ終了
				MessageBox(NULL, TEXT("終了2"), TEXT("INFORMATION"), MB_OK | MB_ICONEXCLAMATION);
				//EnableWindow(GetDlgItem(FU->hwnd, ID_START), TRUE);		//開始ボタン有効
				//他のボタン有効化
				ButtonActivation(FU->hwnd);
				//ボタン状態の変更
				SetDlgItemText(FU->hwnd, ID_MVC_START, L"MVC");
				SetDlgItemText(FU->hwnd, ID_ALL_START, L"ALL");
				
				return FALSE;
			}
		}

		//イベントをセット(計測したよと知らせる)
		count++;
		if (count % DEF_DATAPERS == 0) {
			SetEvent(hEvent);
			count = 0;
		}

		DNum++;

		//スレッドを止めた時に発生する時間のずれに対応
		//若干誤差はある
		if (DNum > 100) {
			DNum = 0;
			beforeTime = timeGetTime();
		}
	}

	//イベントの後処理
	CloseHandle(hEvent);
	//ボタン状態の変更
	SetDlgItemText(FU->hwnd, ID_MVC_START, L"MVC");
	SetDlgItemText(FU->hwnd, ID_ALL_START, L"ALL");

	return 0;
}

//MVC描画用スレッド
UINT WINAPI DrowMvc(LPVOID thParam) {
	int i;
	int errMsg;
	static SEND_POINTER_STRUCT* FU = (SEND_POINTER_STRUCT*)thParam;        //構造体のポインタ取得
	int width, hight;				//PICTBOXの幅
	RECT rect;
	int xl, xr[CH_NUM], yt[CH_NUM],yb[CH_NUM];				//プロットする座標xの左右，yの上下
	int xOffset, yOffset;	//各座標の初期位置
	int unit;				//iつ分の幅(棒グラフの太さ)

	HANDLE hEvent;		//イベント管理用
	DWORD DNum = 0, beforeTime;
	DWORD nowTime, progress, idealTime;
	beforeTime = timeGetTime();						//現在の時刻計算（初期時間）

	//イベントのハンドル取得
	//すべてのアクセス権
	//継承不可
	//EVENT_NAMEのイベント
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_MVC);
	ResetEvent(hEvent);
	GetClientRect(FU->hWndMvc, &rect);	//PICTBOXの情報を入手
	width = rect.left + rect.right;
	hight = rect.top + rect.bottom;

	//初期位置の指定
	xOffset = int(0.01 * width);
	xl = xOffset;

	unit = int(hight * (1 - 0.05 * 2) / CH_NUM);
	for (i = 0; i < CH_NUM; i++) {
		yOffset = int(hight * 0.05);
		yt[i] = unit * i + yOffset;
		yb[i] = yt[i] + unit - 2;
	}
	InvalidateRect(FU->hWndMvc, NULL, FALSE);		//画面再描画
	//区間内でループ（ボタンが押されている間）
	while (FU->waveFlag == TRUE) {
		//次のデータが来るまで待機
		//WaitForSingleObject(hEvent, INFINITE);
		//時間の調整
		nowTime = timeGetTime();					//現在の時刻計算
		progress = nowTime - beforeTime;				//処理時間を計算
		idealTime = (DWORD)(DNum * (1000.0F / (double)DEF_FPS));	//理想時間を計算
		if (idealTime > progress) Sleep(idealTime - progress);			//理想時間になるまで待機
		//再描画
		InvalidateRect(FU->hWndMvc, NULL, FALSE);		//画面再描画
		//座標の計算
		for (i = 0; i < CH_NUM; i++) {
			xr[i] = int((width - xOffset) * FU->mvcData[i] / 100 + xOffset);
		}
		//描画
		PlotMvc(FU->hWndMvc, xl, yt, xr, yb);
		ResetEvent(hEvent);

		DNum++;
		if (DNum > 100) {
			DNum = 0;
			beforeTime = timeGetTime();
		}
	}

	//イベントの後処理
	CloseHandle(hEvent);

	return 0;
}


//子ウィンドウプロシージャ
//軸を描く
HRESULT CALLBACK WndProcMvc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HBRUSH		hBrush, hOldBrush;	//ブラシ
	HPEN		hPen, hOldPen;		//ペン
	int i;							//作業用
	if (uMsg == WM_PAINT) {
		HDC hdc = BeginPaint(hWnd, &ps);
		int width, hight;

		color = RGB(0, 0, 0);		//黒
		axiscolor = RGB(255, 255, 255);
		hBrush = CreateSolidBrush(color);				//ブラシ生成
		hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);	//ブラシ設定
		hPen = CreatePen(PS_SOLID, 2, axiscolor);		//ペン生成
		hOldPen = (HPEN)SelectObject(hdc, hPen);		//ペン設定

		//PICTBOXの大きさ
		RECT rect;
		GetClientRect(hWnd, &rect);
		width = rect.left + rect.right;
		hight = rect.top + rect.bottom;

		//黒く塗りつぶす
		Rectangle(hdc, 0, 0, width + 1, hight + 1);
		//縦軸
		MoveToEx(hdc, width * 0.01, 0, NULL);
		LineTo(hdc, width * 0.01, hight);

		//デバイスコンテキストのハンドル破棄
		EndPaint(hWnd, &ps);

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
		

	}
	return TRUE;
}

//MVC描画
BOOL PlotMvc(HWND hWnd, int xl, int* yt, int* xr, int* yb) {
	int i;
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = GetDC(hWnd);
	HBRUSH		hBrush, hOldBrush;	//ブラシ
	color = RGB(255, 255, 0);
	hBrush = CreateSolidBrush(color);				//ブラシ生成
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);	//ブラシ設定

	//ここをループ
	for (i = 0; i < CH_NUM; i++) {
		Rectangle(hdc, xl, yt[i], xr[i], yb[i]);
	}

	ReleaseDC(hWnd, hdc);

	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
	

	return TRUE;
}




//パターンデータ読み込み用スレッド
UINT WINAPI ReadPatternData(LPVOID thParam)
{
	int i;
	int error;
	static SEND_POINTER_STRUCT* FU = (SEND_POINTER_STRUCT*)thParam;        //構造体のポインタ取得

	HANDLE hEventPattern;		//イベント管理用
	HANDLE hEventLLGMN;		//イベント管理用
	FILE* fp;			//ファイルポインタ
	DWORD DNum = 0, beforeTime;
	beforeTime = timeGetTime();						//現在の時刻計算（初期時間）
	int count = 0;

	//DispConsole();
	//イベントのハンドル取得
	//すべてのアクセス権
	//継承不可
	//EVENT_NAMEのイベント
	hEventPattern = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_PATTERN);
	hEventLLGMN= OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_LLGMN);
	if (hEventLLGMN == NULL) {
		MessageBox(NULL, TEXT("ERROR!"), NULL, MB_OK | MB_ICONERROR);
	}
	ResetEvent(hEventPattern);
	ResetEvent(hEventLLGMN);
	//ファイルオープン
	if ((fopen_s(&fp, FNAME_PATTERN, "r")) != 0) {
		MessageBox(NULL, TEXT("Input File Open ERROR!"), NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//データ読み込み
	while (FU->waveFlag == TRUE) {
		DWORD nowTime, progress, idealTime;

		//時間の調整
		nowTime = timeGetTime();					//現在の時刻計算
		progress = nowTime - beforeTime;				//処理時間を計算
		idealTime = (DWORD)(DNum * (1000.0F / (double)DEF_SAMPLING_RATE));	//理想時間を計算
		if (idealTime > progress) Sleep(idealTime - progress);			//理想時間になるまで待機

		
		//データの読み込み
		//パターン
		for (i = 0; i < CH_NUM; i++) {
			error = fscanf_s(fp, "%lf", &FU->patternData[i]);
			if (error == EOF) {
				FU->rowDataFlag = FALSE;			//ループ終了フラグ
				FU->waveFlag = FALSE;
				MessageBox(NULL, TEXT("終了3"), TEXT("INFORMATION"), MB_OK | MB_ICONEXCLAMATION);
				//EnableWindow(GetDlgItem(FU->hwnd, ID_START), TRUE);		//開始ボタン有効
				ButtonActivation(FU->hwnd);
				//ボタン状態の変更
				SetDlgItemText(FU->hwnd, ID_RADAR_START, L"RADAR");
				SetDlgItemText(FU->hwnd, ID_ALL_START, L"ALL");
				
				return FALSE;
			}
		}

		//イベントをセット(計測したよと知らせる)
		count++;
		if (count % DEF_DATAPERS == 0) {
			//printf("%d\n", count);
			SetEvent(hEventPattern);
			SetEvent(hEventLLGMN);
			count = 0;
		}

		DNum++;

		//スレッドを止めた時に発生する時間のずれに対応
		//若干誤差はある
		if (DNum > 100) {
			DNum = 0;
			beforeTime = timeGetTime();
		}
	}

	//イベントの後処理
	CloseHandle(hEventPattern);
	CloseHandle(hEventLLGMN);

	//ボタン状態の変更
	SetDlgItemText(FU->hwnd, ID_RADAR_START, L"RADAR");
	SetDlgItemText(FU->hwnd, ID_PROB_START, L"CHECK");
	SetDlgItemText(FU->hwnd, ID_ALL_START, L"ALL");
	return 0;
}

//レーダーチャート描画用スレッド
UINT WINAPI DrowRadar(LPVOID thParam) {
	int i;
	int errMsg;
	static SEND_POINTER_STRUCT* FU = (SEND_POINTER_STRUCT*)thParam;        //構造体のポインタ取得
	int width, hight;				//PICTBOXの幅
	RECT rect;
	int x[CH_NUM],y[CH_NUM];				//プロットする座標xy
	int xOrigin, yOrigin;	//各座標の原点
	int rx,ry;

	DWORD DNum = 0, beforeTime;
	DWORD nowTime, progress, idealTime;
	beforeTime = timeGetTime();						//現在の時刻計算（初期時間）

	HANDLE hEvent;		//イベント管理用

	//イベントのハンドル取得
	//すべてのアクセス権
	//継承不可
	//EVENT_NAMEのイベント
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_PATTERN);
	ResetEvent(hEvent);
	GetClientRect(FU->hWndRadar, &rect);	//PICTBOXの情報を入手
	width = rect.left + rect.right;
	hight = rect.top + rect.bottom;

	//原点の計算
	xOrigin = width / 2;
	yOrigin = hight / 2;
	rx = xOrigin * 0.95;
	ry = yOrigin * 0.95;

	//int count = 0;
	//DispConsole();
	InvalidateRect(FU->hWndRadar, NULL, FALSE);		//画面再描画
	//区間内でループ（ボタンが押されている間）
	while (FU->waveFlag == TRUE) {
		//次のデータが来るまで待機+
		//WaitForMultipleObjects(2,hEvent,TRUE,INFINITE);
		//WaitForSingleObject(hEvent, INFINITE);
		//printf("%d\n", count);
		//時間の調整
		nowTime = timeGetTime();					//現在の時刻計算
		progress = nowTime - beforeTime;				//処理時間を計算
		idealTime = (DWORD)(DNum * (1000.0F / (double)DEF_FPS));	//理想時間を計算
		if (idealTime > progress) Sleep(idealTime - progress);			//理想時間になるまで待機
		//再描画
		InvalidateRect(FU->hWndRadar, NULL, FALSE);		//画面再描画
		//座標の計算
		for (i = 0; i < CH_NUM; i++) {
			x[i] = int(sin(PI/2 * i) * FU->patternData[i] * rx + xOrigin);
			y[i] = int(-cos(PI/2 * i) * FU->patternData[i] * ry + yOrigin);
		}
		//描画
		PlotRadar(FU->hWndRadar,x,y);
		ResetEvent(hEvent);
		//count++;

		DNum++;
		if (DNum > 100) {
			DNum = 0;
			beforeTime = timeGetTime();
		}
	}

	//イベントの後処理
	CloseHandle(hEvent);

	return 0;

}


//子ウィンドウプロシージャ
//軸を描く
HRESULT CALLBACK WndProcRadar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HBRUSH		hBrush, hOldBrush;	//ブラシ
	HPEN		hPen, hOldPen;		//ペン
	int i;							//作業用
	if (uMsg == WM_PAINT) {
		HDC hdc = BeginPaint(hWnd, &ps);
		int width, hight;
		int xOrigin, yOrigin;	//各座標の原点

		color = RGB(0, 0, 0);		//黒
		axiscolor = RGB(255, 255, 255);
		hBrush = CreateSolidBrush(color);				//ブラシ生成
		hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);	//ブラシ設定
		hPen = CreatePen(PS_SOLID, 2, axiscolor);		//ペン生成
		hOldPen = (HPEN)SelectObject(hdc, hPen);		//ペン設定

		//PICTBOXの大きさ
		RECT rect;
		GetClientRect(hWnd, &rect);
		width = rect.left + rect.right;
		hight = rect.top + rect.bottom;

		//原点の計算
		xOrigin = width / 2;
		yOrigin = hight / 2;

		//黒く塗りつぶす
		Rectangle(hdc, 0, 0, width + 1, hight + 1);
		//縦軸
		MoveToEx(hdc, xOrigin, hight * 0.05, NULL);
		LineTo(hdc, xOrigin, hight * 0.95);
		//横軸
		MoveToEx(hdc, width * 0.05, yOrigin, NULL);
		LineTo(hdc, width*0.95, yOrigin);

		//デバイスコンテキストのハンドル破棄
		EndPaint(hWnd, &ps);

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
		

	}
	return TRUE;
}

//レーダーチャート描画
BOOL PlotRadar(HWND hWnd, int* x, int* y) {
	int i;
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = GetDC(hWnd);
	HBRUSH		hBrush, hOldBrush;	//ブラシ
	HPEN	hPen, hOldPen;			//ペン
	colorEll = RGB(255, 0, 0);
	colorPen = RGB(255, 255, 0);
	hBrush = CreateSolidBrush(colorEll);				//ブラシ生成
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);	//ブラシ設定
	hPen = CreatePen(PS_SOLID, 3, colorPen);		//ペン生成
	hOldPen = (HPEN)SelectObject(hdc, hPen);		//ペン設定

	//まず各辺の描画
	//ここをループ
	for (i = 0; i < CH_NUM-1; i++) {
		MoveToEx(hdc, x[i], y[i],NULL);
		LineTo(hdc, x[i + 1], y[i + 1]);
	}
	MoveToEx(hdc, x[i], y[i],NULL);
	LineTo(hdc, x[0], y[0]);
	

	//ペンの変更
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));		//ペン生成
	hOldPen = (HPEN)SelectObject(hdc, hPen);		//ペン設定
	//頂点の円
	for (i = 0; i < CH_NUM; i++) {
		Ellipse(hdc, x[i] - 8, y[i] - 8, x[i] + 8, y[i] + 8);
	}

	ReleaseDC(hWnd, hdc);

	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	

	return TRUE;
}


//LLGMNからデータを取得
UINT WINAPI Llgmn(LPVOID thParam)
{
	int i;
	int error;
	FILE *fp;
	static SEND_POINTER_STRUCT* FU = (SEND_POINTER_STRUCT*)thParam;        //構造体のポインタ取得
	HANDLE hEventLLGMN;		//イベント管理用
	HANDLE hEventProb;		//イベント管理用
	
	DWORD DNum = 0, beforeTime;
	DWORD nowTime, progress, idealTime;
	beforeTime = timeGetTime();

	int count = 0;

	//ファイルオープン
	if ((fopen_s(&fp, FNAME_PROB, "r")) != 0) {
		MessageBox(NULL, TEXT("Input File Open ERROR!"), NULL, MB_OK | MB_ICONERROR);
		return FALSE;
		//ボタン状態の変更
		SetDlgItemText(FU->hwnd, ID_PROB_START, L"CHECK");
		SetDlgItemText(FU->hwnd, ID_ALL_START, L"ALL");
	}
	//イベントのハンドル取得
	//すべてのアクセス権
	//継承不可
	//EVENT_NAMEのイベント
	hEventLLGMN = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_LLGMN);
	hEventProb = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_PROB);
	
	ResetEvent(hEventLLGMN);
	ResetEvent(hEventProb);
	//DispConsole();

	//LLGMNに1つずつパターンデータを入れる
	//今回は1つずつ事後確率を取得する
	while (FU->waveFlag == TRUE) {
		//次のデータが来るまで待機
		//WaitForSingleObject(hEventLLGMN, INFINITE);
		//nowTime = timeGetTime();					//現在の時刻計算
		//progress = nowTime - beforeTime;				//処理時間を計算
		//idealTime = (DWORD)(DNum * (1000.0F / (double)DEF_FPS/2));	//理想時間を計算
		//if (idealTime > progress) Sleep(idealTime - progress);			//理想時間になるまで待機
		//printf("%d\n", count);
		for (i = 0; i < CLASS_NUM; i++) {
			FU->probResult[i] = FU->patternData[i];
			/*error = fscanf_s(fp, "%lf", &FU->probResult[i]);
			if (error == EOF) {
				MessageBox(NULL, TEXT("終了4"), TEXT("INFORMATION"), MB_OK | MB_ICONEXCLAMATION);
				return FALSE;
			}*/
		}
		count++;
		ResetEvent(hEventLLGMN);
		SetEvent(hEventProb);

		DNum++;
		if (DNum > 100) {
			DNum = 0;
			beforeTime = timeGetTime();
		}
	}
	//イベントの後処理
	CloseHandle(hEventLLGMN);
	CloseHandle(hEventProb);
	
	//ボタン状態の変更
	SetDlgItemText(FU->hwnd, ID_PROB_START, L"CHECK");
	SetDlgItemText(FU->hwnd, ID_ALL_START, L"ALL");

	return 0;
}

//事後確率描画用スレッド
UINT WINAPI DrowProb(LPVOID thParam) {
	int i;
	int errMsg;
	static SEND_POINTER_STRUCT* FU = (SEND_POINTER_STRUCT*)thParam;        //構造体のポインタ取得
	int width, hight;				//PICTBOXの幅
	RECT rect;
	int xl, xr[CH_NUM], yt[CH_NUM], yb[CH_NUM];				//プロットする座標xの左右，yの上下
	int xOffset, yOffset;	//各座標の初期位置
	int unit;				//iつ分の幅(棒グラフの太さ)

	DWORD DNum = 0, beforeTime;
	DWORD nowTime, progress, idealTime;
	beforeTime = timeGetTime();	
	//現在の時刻計算（初期時間）

	HANDLE hEvent;		//イベント管理用

	//イベントのハンドル取得
	//すべてのアクセス権
	//継承不可
	//EVENT_NAMEのイベント
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_PROB);
	ResetEvent(hEvent);
	GetClientRect(FU->hWndProb, &rect);	//PICTBOXの情報を入手
	width = rect.left + rect.right;
	hight = rect.top + rect.bottom;

	//初期位置の指定
	xOffset = int(0.01 * width);
	xl = xOffset;

	unit = int(hight * (1 - 0.05 * 2) / CH_NUM);
	for (i = 0; i < CH_NUM; i++) {
		yOffset = int(hight * 0.05);
		yt[i] = unit * i + yOffset;
		yb[i] = yt[i] + unit - 2;
	}
	InvalidateRect(FU->hWndProb, NULL, FALSE);		//画面再描画
	//区間内でループ（ボタンが押されている間）
	while (FU->waveFlag == TRUE) {
		//次のデータが来るまで待機
		//WaitForSingleObject(hEvent, INFINITE);
		//WaitForMultipleObjects(2,hEvent,TRUE,INFINITE);
		//WaitForSingleObject(hEvent, INFINITE);
		//時間の調整
		nowTime = timeGetTime();					//現在の時刻計算
		progress = nowTime - beforeTime;				//処理時間を計算
		idealTime = (DWORD)(DNum * (1000.0F / (double)DEF_FPS/2));	//理想時間を計算
		if (idealTime > progress) Sleep(idealTime - progress);			//理想時間になるまで待機
		//再描画
		InvalidateRect(FU->hWndProb, NULL, FALSE);		//画面再描画
		//座標の計算
		for (i = 0; i < CH_NUM; i++) {
			xr[i] = int((width - xOffset) * FU->probResult[i] + xOffset);
		}
		//描画
		//PlotMvc(FU->hWndProb, xl, yt, xr, yb);
		PlotMvc(FU->hWndProb, xl, yt, xr, yb);
		ResetEvent(hEvent);
	}

	//イベントの後処理
	CloseHandle(hEvent);

	return 0;

}




BOOL WinInitialize(HINSTANCE hInst, HWND hPaWnd, HMENU chID, char *cWinName, HWND PaintArea, WNDPROC WndProc, HWND *hWnd)
{
	WNDCLASS wc;			//ウィンドウクラス
	WINDOWPLACEMENT	wplace;	//子ウィンドウ生成領域計算用（画面上のウィンドウの配置情報を格納する構造体）
	RECT WinRect;			//子ウィンドウ生成領域
	ATOM atom;				//アトム

							//ウィンドウクラス初期化
	wc.style = CS_HREDRAW ^ WS_MAXIMIZEBOX | CS_VREDRAW;	//ウインドウスタイル
	wc.lpfnWndProc = WndProc;									//ウインドウのメッセージを処理するコールバック関数へのポインタ
	wc.cbClsExtra = 0;											//
	wc.cbWndExtra = 0;
	wc.hCursor = NULL;										//プログラムのハンドル
	wc.hIcon = NULL;										//アイコンのハンドル
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);		//ウインドウ背景色
	wc.hInstance = hInst;										//ウインドウプロシージャがあるインスタンスハンドル
	wc.lpszMenuName = NULL;										//メニュー名
	wc.lpszClassName = (LPCTSTR)cWinName;									//ウインドウクラス名

	if (!(atom = RegisterClass(&wc))) {
		MessageBox(hPaWnd, TEXT("ウィンドウクラスの生成に失敗しました1．"), NULL, MB_OK | MB_ICONERROR);
		return false;
	}

	GetWindowPlacement(PaintArea, &wplace);	//描画領域ハンドルの情報を取得(ウィンドウの表示状態を取得)
	WinRect = wplace.rcNormalPosition;		//描画領域の設定

											//ウィンドウ生成
	*hWnd = CreateWindow(
		(LPCTSTR)atom,
		(LPCTSTR)cWinName,
		WS_CHILD | WS_VISIBLE,//| WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME |WS_VISIBLE, 
		WinRect.left, WinRect.top,
		WinRect.right - WinRect.left, WinRect.bottom - WinRect.top,
		hPaWnd, chID, hInst, NULL
	);

	if (*hWnd == NULL) {
		MessageBox(hPaWnd, TEXT("ウィンドウの生成に失敗しました2．"), NULL, MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

//コンソール作成
void DispConsole() {
	// コンソールを作成する
	AllocConsole();
	// 標準入出力に割り当てる
	FILE* fp = NULL;

	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
}