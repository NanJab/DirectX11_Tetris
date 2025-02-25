// window api 사용을 위한 헤더
#include <windows.h>
// Direct2D 사용을 위한 헤더
#include "Tetris.h"
#include <d2d1.h>
#include <time.h>

// Direct2D 라이브러리 링크
#pragma comment(lib, "d2d1.lib")

Tetris game;

// 윈도우 프로시저 함수 선언 - 윈도우의 메시지를 처리하는 콜백 함수
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Direct2D 전역 객체들 선언
ID2D1Factory* pD2DFactory = nullptr;				// Direct2D 팩토리 객체
ID2D1HwndRenderTarget* pRenderTarget = nullptr;		// 렌더링 대상 객체
ID2D1SolidColorBrush* pBrush = nullptr;				// 색상 브러시 객체

void InitDirect2D(HWND hwnd)
{
	// Direct2D 팩토리 생성 - 싱글 스레드 모드
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	// 클라이언트 영역 크기 설정
	RECT rc;
	GetClientRect(hwnd, &rc);
	// 클라이언트 영역 크기로 D2D1_SIZE_U 구조체 생성
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	// 윈도우에 연결된 렌더타겟 생성
	pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),					// 기본 렌더타겟 속성
		D2D1::HwndRenderTargetProperties(hwnd, size),	// 윈도우 핸들과 크기 지정
		&pRenderTarget
	);

	// 하늘색 브러시 생성
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue), &pBrush);
}

// Direct2D 객체들을 안전하게 해제하는 함수
void CleanUpDirect2D()
{
	if (pBrush)
	{
		pBrush->Release();
	}
	if (pRenderTarget)
	{
		pRenderTarget->Release();
	}
	if (pD2DFactory)
	{
		pD2DFactory->Release();
	}
}

// 윈도우 프로그램의 진입점
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	// 윈도우 클래스 설정
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;		// 포르시저 함수 지정
	wc.hInstance = hInstance;			// 프로그램 인스턴스 핸들
	wc.lpszClassName = L"Direct2D";		// 클래스 이름

	RegisterClass(&wc);					// 윈도우 클래스 등록

	// 윈도우 생성
	HWND hwnd = CreateWindowEx(
		0,											// 확장 윈도우 스타일
		L"Direct2D",								// 윈도우 클래스 이름
		L"테트리스",								// 윈도우 제목			
		WS_OVERLAPPEDWINDOW,						// 윈도우 스타일
		CW_USEDEFAULT, CW_USEDEFAULT, 720, 640,		// 위치와 크기
		nullptr,									// 부모 윈도우 없음
		nullptr,									// 메뉴 없음
		hInstance,									// 프로그램 인스턴스 핸들
		nullptr										// 추가 데이터 없음
	);	

	if (hwnd == nullptr) return 0;	// 윈도우 생성 실패 시 종료

	ShowWindow(hwnd, nCmdShow);		// 윈도우 표시	

	// 메시지 루프
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);	// 키보드 입력 메시지 변환
		DispatchMessage(&msg);	// 윈도우 프로시저로 메시지 전달
	}
	return 0;
}

// 윈도우의 메시지를 처리하는 함수
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE: // 윈도우가 생성될 때
		InitDirect2D(hwnd);
		game.InitGame(hwnd);
		return 0;

	case WM_PAINT:	// 윈도우를 그려야 할 때
	{
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		
		// Direct2D 그리기 시작
		pRenderTarget->BeginDraw();
		// 검정색으로 화면 클리어
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		// 보드의 크기
		D2D1_RECT_F boardRect = D2D1::RectF(
			game.BoardStartX, 
			game.BoardStartY,
			game.BoardStartX + (game.BoardWidth * game.BlockSize),
			game.BoardStartY + (game.BoardHeight * game.BlockSize)
		);
		pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		pRenderTarget->DrawRectangle(boardRect, pBrush);


		// 쌓인 블록들 그리기
		game.DrawBoard(pRenderTarget, pBrush);

		// 현재 움직이는 블록 그리기
		if (game.currentBlock)
		{
			game.DrawBlock(pRenderTarget, pBrush);
		}		

		// 다음 미리보기 블럭 그리기
		if (game.nextBlock)
		{
			game.DrawPreview(pRenderTarget, pBrush);

		}

		game.DrawScore(pRenderTarget, pBrush);

		// 그리기 종료
		pRenderTarget->EndDraw();

		EndPaint(hwnd, &ps);

		return 0;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
			case VK_LEFT:
				// 왼쪽 이동
				game.BlockMove(-1, 0);
				break;
			case VK_RIGHT:
				// 오른쪽 이동
				game.BlockMove(1, 0);
				break;
			case VK_UP:
				// 바로 바닥으로 이동
				while (!game.CheckCollision())
				{
					// 누르는 동안 추가되기때문에 바로 바닥으로 이동하게 됨
					game.MoveDown();
				}
				// 바닥에 쌓이는 함수
				game.BuildBlocks();
				// 현재 블럭 없음
				delete game.currentBlock;
				// 새로운 블럭 생성 후 현재 블럭으로 등록
				game.currentBlock = game.CreateRandomBlcok();
				break;
			case VK_DOWN:
				// 블럭 빠른이동 체크
				if (!game.isFastDrop)
				{
					game.isFastDrop = true;
					// 빠른 이동
					game.UpdateDropSpeed(hwnd, 150);
				}
				break;
			case VK_SPACE:
				// 블럭회전
				game.RotateBlock();
				break;
			case VK_SHIFT:
				game.BlockExchange();
				break;
			case VK_ESCAPE:
				PostQuitMessage(0);				
				break;
		}
		InvalidateRect(hwnd, NULL, TRUE); // 화면 갱신
		return 0;
	}
	case WM_KEYUP:
		// 아래키를 누르고 뗄 경우
		if (wParam == VK_DOWN)
		{
			game.isFastDrop = false;
			// 원래 떨어지는 속도로 복귀
			game.UpdateDropSpeed(hwnd, 500);
		}
		return 0;
	case WM_TIMER:
	{
		if (wParam == 1)
		{
			if (game.currentBlock)
			{
				// 바닥에 닿지 않았을 경우
				if (!game.CheckCollision())
				{
					// 아래로 떨어진다.
					game.currentBlock->y += game.BlockSize;
					InvalidateRect(hwnd, nullptr, TRUE); // 화면에 다시그리기
				}
				else
				{
					// 블럭을 쌓아주는 함수
					game.BuildBlocks();

					game.CheckGameOver();
					
					if (!game.gameOver)  // 게임오버가 아닐 경우에만 다음 블록 처리
					{
						// 다음 블록을 현재 블록으로 설정
						game.currentBlock = game.nextBlock;

						if (game.currentBlock)
						{
							game.currentBlock->x = game.BoardStartX + (game.BoardWidth / 2 - 2) * game.BlockSize;
							game.currentBlock->y = 0;

							// 새 블록 위치에서 추가 충돌 검사
							for (int i = 0; i < 4; i++)
							{
								for (int j = 0; j < 4; j++)
								{
									if (game.currentBlock->shape[game.currentBlock->rotation] & (0x8000 >> (i * 4 + j)))
									{
										int x = (game.currentBlock->x - game.BoardStartX) / game.BlockSize + j;
										int y = (game.currentBlock->y - game.BoardStartY) / game.BlockSize + i;

										if (x >= 0 && x < game.BoardWidth && y >= 0 && y < game.BoardHeight && game.Board[0][x] == 1)
										{
											game.gameOver = true;
											break;
										}
									}
								}
								if (game.gameOver) break;
							}
						}
					}
					game.nextBlock = game.CreateRandomBlcok(); // 미리보기 블럭
				}
				
			}
		}
		game.DeleteBlocks();
		return 0;
	}
	case WM_DESTROY: // 윈도우가 파괴될 때
		CleanUpDirect2D();  // Direct2D wjdfl
		PostQuitMessage(0); // 프로그램 종료 메시지 발송
		return 0;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam); // 기본 메시지 처리
	}
}