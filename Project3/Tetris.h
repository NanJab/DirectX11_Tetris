#pragma once
#include <windows.h>
#include <cstdint>
#include <d2d1.h>
#include <time.h>
#include <dwrite.h>

#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

struct TetrisBlock
{
	uint16_t shape[4];	// ��� ���
	float x, y;			// ���� ��ġ
	D2D1_COLOR_F color; // ��� ����
	int rotation = 0;	// ȸ�� ����
};

class Tetris 
{
public:	
	// ��� �� ĭ�� ũ��
	float BlockSize = 30.0f;

	// ���� ���尡 �����ϴ� x ��ǥ
	float BoardStartX = 200.0f;
	// ���� ���尡 �����ϴ� y ��ǥ
	float BoardStartY = 0.0f;

	int BoardWidth = 10;
	int BoardHeight = 20;
	int BlockSpeed = 500;
	int score = 0;

	bool isFastDrop = false;
	bool exchangeBlockCheck = false;
	bool gameOver = false;

	int Board[20][10] = { 0 };

	void InitGame(HWND hwnd);
	bool CheckCollision();
	void BuildBlocks();
	void RotateBlock();
	void DeleteBlocks();
	void BlockMove(int dx, int dy);
	void MoveDown();
	void UpdateDropSpeed(HWND hwnd, int newSpeed);
	void BlockExchange();
	void CheckGameOver();

	bool CanRotate(TetrisBlock* block, int nextRotation);

	TetrisBlock* currentBlock = nullptr;
	TetrisBlock* nextBlock = nullptr;

	TetrisBlock* CreateRandomBlcok(); // ���� ��� ���� �Լ�
	void DrawBlock(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush);
	void DrawBoard(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush);
	void DrawPreview(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush);
	void DrawScore(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush);
};