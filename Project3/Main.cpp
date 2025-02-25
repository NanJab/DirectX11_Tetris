// window api ����� ���� ���
#include <windows.h>
// Direct2D ����� ���� ���
#include "Tetris.h"
#include <d2d1.h>
#include <time.h>

// Direct2D ���̺귯�� ��ũ
#pragma comment(lib, "d2d1.lib")

Tetris game;

// ������ ���ν��� �Լ� ���� - �������� �޽����� ó���ϴ� �ݹ� �Լ�
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Direct2D ���� ��ü�� ����
ID2D1Factory* pD2DFactory = nullptr;				// Direct2D ���丮 ��ü
ID2D1HwndRenderTarget* pRenderTarget = nullptr;		// ������ ��� ��ü
ID2D1SolidColorBrush* pBrush = nullptr;				// ���� �귯�� ��ü

void InitDirect2D(HWND hwnd)
{
	// Direct2D ���丮 ���� - �̱� ������ ���
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	// Ŭ���̾�Ʈ ���� ũ�� ����
	RECT rc;
	GetClientRect(hwnd, &rc);
	// Ŭ���̾�Ʈ ���� ũ��� D2D1_SIZE_U ����ü ����
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	// �����쿡 ����� ����Ÿ�� ����
	pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),					// �⺻ ����Ÿ�� �Ӽ�
		D2D1::HwndRenderTargetProperties(hwnd, size),	// ������ �ڵ�� ũ�� ����
		&pRenderTarget
	);

	// �ϴû� �귯�� ����
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue), &pBrush);
}

// Direct2D ��ü���� �����ϰ� �����ϴ� �Լ�
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

// ������ ���α׷��� ������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	// ������ Ŭ���� ����
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;		// �������� �Լ� ����
	wc.hInstance = hInstance;			// ���α׷� �ν��Ͻ� �ڵ�
	wc.lpszClassName = L"Direct2D";		// Ŭ���� �̸�

	RegisterClass(&wc);					// ������ Ŭ���� ���

	// ������ ����
	HWND hwnd = CreateWindowEx(
		0,											// Ȯ�� ������ ��Ÿ��
		L"Direct2D",								// ������ Ŭ���� �̸�
		L"��Ʈ����",								// ������ ����			
		WS_OVERLAPPEDWINDOW,						// ������ ��Ÿ��
		CW_USEDEFAULT, CW_USEDEFAULT, 720, 640,		// ��ġ�� ũ��
		nullptr,									// �θ� ������ ����
		nullptr,									// �޴� ����
		hInstance,									// ���α׷� �ν��Ͻ� �ڵ�
		nullptr										// �߰� ������ ����
	);	

	if (hwnd == nullptr) return 0;	// ������ ���� ���� �� ����

	ShowWindow(hwnd, nCmdShow);		// ������ ǥ��	

	// �޽��� ����
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);	// Ű���� �Է� �޽��� ��ȯ
		DispatchMessage(&msg);	// ������ ���ν����� �޽��� ����
	}
	return 0;
}

// �������� �޽����� ó���ϴ� �Լ�
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE: // �����찡 ������ ��
		InitDirect2D(hwnd);
		game.InitGame(hwnd);
		return 0;

	case WM_PAINT:	// �����츦 �׷��� �� ��
	{
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		
		// Direct2D �׸��� ����
		pRenderTarget->BeginDraw();
		// ���������� ȭ�� Ŭ����
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		// ������ ũ��
		D2D1_RECT_F boardRect = D2D1::RectF(
			game.BoardStartX, 
			game.BoardStartY,
			game.BoardStartX + (game.BoardWidth * game.BlockSize),
			game.BoardStartY + (game.BoardHeight * game.BlockSize)
		);
		pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		pRenderTarget->DrawRectangle(boardRect, pBrush);


		// ���� ��ϵ� �׸���
		game.DrawBoard(pRenderTarget, pBrush);

		// ���� �����̴� ��� �׸���
		if (game.currentBlock)
		{
			game.DrawBlock(pRenderTarget, pBrush);
		}		

		// ���� �̸����� �� �׸���
		if (game.nextBlock)
		{
			game.DrawPreview(pRenderTarget, pBrush);

		}

		game.DrawScore(pRenderTarget, pBrush);

		// �׸��� ����
		pRenderTarget->EndDraw();

		EndPaint(hwnd, &ps);

		return 0;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
			case VK_LEFT:
				// ���� �̵�
				game.BlockMove(-1, 0);
				break;
			case VK_RIGHT:
				// ������ �̵�
				game.BlockMove(1, 0);
				break;
			case VK_UP:
				// �ٷ� �ٴ����� �̵�
				while (!game.CheckCollision())
				{
					// ������ ���� �߰��Ǳ⶧���� �ٷ� �ٴ����� �̵��ϰ� ��
					game.MoveDown();
				}
				// �ٴڿ� ���̴� �Լ�
				game.BuildBlocks();
				// ���� �� ����
				delete game.currentBlock;
				// ���ο� �� ���� �� ���� ������ ���
				game.currentBlock = game.CreateRandomBlcok();
				break;
			case VK_DOWN:
				// �� �����̵� üũ
				if (!game.isFastDrop)
				{
					game.isFastDrop = true;
					// ���� �̵�
					game.UpdateDropSpeed(hwnd, 150);
				}
				break;
			case VK_SPACE:
				// ��ȸ��
				game.RotateBlock();
				break;
			case VK_SHIFT:
				game.BlockExchange();
				break;
			case VK_ESCAPE:
				PostQuitMessage(0);				
				break;
		}
		InvalidateRect(hwnd, NULL, TRUE); // ȭ�� ����
		return 0;
	}
	case WM_KEYUP:
		// �Ʒ�Ű�� ������ �� ���
		if (wParam == VK_DOWN)
		{
			game.isFastDrop = false;
			// ���� �������� �ӵ��� ����
			game.UpdateDropSpeed(hwnd, 500);
		}
		return 0;
	case WM_TIMER:
	{
		if (wParam == 1)
		{
			if (game.currentBlock)
			{
				// �ٴڿ� ���� �ʾ��� ���
				if (!game.CheckCollision())
				{
					// �Ʒ��� ��������.
					game.currentBlock->y += game.BlockSize;
					InvalidateRect(hwnd, nullptr, TRUE); // ȭ�鿡 �ٽñ׸���
				}
				else
				{
					// ���� �׾��ִ� �Լ�
					game.BuildBlocks();

					game.CheckGameOver();
					
					if (!game.gameOver)  // ���ӿ����� �ƴ� ��쿡�� ���� ��� ó��
					{
						// ���� ����� ���� ������� ����
						game.currentBlock = game.nextBlock;

						if (game.currentBlock)
						{
							game.currentBlock->x = game.BoardStartX + (game.BoardWidth / 2 - 2) * game.BlockSize;
							game.currentBlock->y = 0;

							// �� ��� ��ġ���� �߰� �浹 �˻�
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
					game.nextBlock = game.CreateRandomBlcok(); // �̸����� ��
				}
				
			}
		}
		game.DeleteBlocks();
		return 0;
	}
	case WM_DESTROY: // �����찡 �ı��� ��
		CleanUpDirect2D();  // Direct2D wjdfl
		PostQuitMessage(0); // ���α׷� ���� �޽��� �߼�
		return 0;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam); // �⺻ �޽��� ó��
	}
}