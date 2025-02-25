#include "Tetris.h"

#include <vector>

using namespace std;
TetrisBlock T_Block =
{
	// Bitmask ȸ�����
	{
		0x0E40, // 0��
		0x04C4, // 90��
		0x04E0, // 180��
		0x4640  // 270��
	},
	0.0f,
	0.0f,
	D2D1::ColorF(D2D1::ColorF::Purple),
	0
};

TetrisBlock O_Block =
{
	{
		0x6600,
		0x6600,
		0x6600,
		0x6600
	},
	0.0f,
	0.0f,
	D2D1::ColorF(D2D1::ColorF::Orange),
	0
};

TetrisBlock S_Block =
{
	{
		0x06C0,
		0x4620,
		0x06C0,
		0x4620
	},
	0.0f,
	0.0f,
	D2D1::ColorF(D2D1::ColorF::Green),
	0
};

TetrisBlock Z_Block =
{
	{
		0x0C60,
		0x2640,
		0x0C60,
		0x2640
	},
	0.0f,
	0.0f,
	D2D1::ColorF(D2D1::ColorF::Red),
	0
};

TetrisBlock I_Block =
{
	{
		0x0F00,
		0x2222,
		0x0F00,
		0x2222
	},
	0.0f,								// �ʱ� x ��ġ
	0.0f,								// �ʱ� y ��ġ
	D2D1::ColorF(D2D1::ColorF::Cyan),	// ����
	0
};

TetrisBlock L_Block =
{
	{
		0x88C0,
		0xE800,
		0xC440,
		0x2E00
	},
	0.0f,
	0.0f,
	D2D1::ColorF(D2D1::ColorF::Yellow),
	0
};

TetrisBlock J_Block = {
	{
		0x44C0,
		0x8E00,
		0xC880,
		0xE200
	},
	0.0f,
	0.0f,
	D2D1::ColorF(D2D1::ColorF::Blue),
	0
};

TetrisBlock* Tetris::CreateRandomBlcok()
{
	static bool initialized = false;
	if (!initialized)
	{
		// ���� �� �� ���� ����
		srand(time(NULL));
		initialized = true;
	}

	TetrisBlock* newBlock = nullptr;
	int randomBlock = rand() % 7; // 0~6 ���� ���� ���� ����

	switch (randomBlock)
	{
	case 0:
		newBlock = new TetrisBlock(I_Block);
		break;
	case 1:
		newBlock = new TetrisBlock(L_Block);
		break;
	case 2:
		newBlock = new TetrisBlock(T_Block);
		break;
	case 3:
		newBlock = new TetrisBlock(O_Block);
		break;
	case 4:
		newBlock = new TetrisBlock(S_Block);
		break;
	case 5:
		newBlock = new TetrisBlock(Z_Block);
		break;
	case 6:
		newBlock = new TetrisBlock(J_Block);
		break;
	}

	if (newBlock)
	{
		if (!currentBlock)
		{
			// ���� �������� ������� �� ĭ���� �ʹ� ��ġ�� �߾����� ����
			// ���� �߾ӿ� ����
			newBlock->x = BoardStartX + (BoardWidth / 2 - 2) * BlockSize;
			newBlock->y = 0;
		}
		else
		{
			// �� ��ȯ üũ
			if (!exchangeBlockCheck)
			{
				// �̸����� ���� ��ġ ����
				newBlock->x = 50.0f;
				newBlock->y = 50.0f;
			}
			
		}						
	}

	return newBlock;
}

void Tetris::InitGame(HWND hwnd)
{
	score = 0;
	gameOver = false;

	bool check = false;
	// ���忡 ���� ��Ÿ���� ������ ���ֱ� ���� ���� ó��
	for (int y = 0; y < BoardHeight; y++)
	{
		for (int x = 0; x < BoardWidth; x++)
		{
			Board[y][x] = 0;
		}
	}

	// 1�ʸ��� ����� ����߸��� Ÿ�̸�
	SetTimer(hwnd, 1, 500, nullptr);

	// �ʱ� ��� ����
	currentBlock = CreateRandomBlcok();
	nextBlock = CreateRandomBlcok();	
}

// �� �������� �ӵ� ���� �Լ�
void Tetris::UpdateDropSpeed(HWND hwnd, int newSpeed)
{
	BlockSpeed = newSpeed;
	KillTimer(hwnd, 1); // ���� Ÿ�̸� ����
	SetTimer(hwnd, 1, BlockSpeed, nullptr); // ���ο� �ӵ��� Ÿ�̸� �缳��
}

// �ٴ� �浹 üũ �Լ�
bool Tetris::CheckCollision()
{
	if (!currentBlock) return false;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (currentBlock->shape[currentBlock->rotation] & (0x8000 >> (i * 4 + j)))
			{
				// ���� �ȿ��� ��ġ�� �����ؾ� �ϱ� ������ ������ ���� ��ǥ�� ����, ��������� ���带 �׸���� ���� �� ��ġ ����
				int x = (currentBlock->x - BoardStartX) / BlockSize + j;
				// �� ĭ �� �Ʒ��� �˻��Ͽ� ���� �浹 ó���� �������� Ȯ��
				int y = (currentBlock->y - BoardStartY) / BlockSize + i + 1;

				if (y < 0 || y >= BoardHeight || x < 0 || x >= BoardWidth)
				{
					return true;
				}
				else if (Board[y][x] == 1)
				{
					return true;
				}

			}
		}					
	}
	return false;
}

// ���� �׾��ִ� �Լ�
void Tetris::BuildBlocks()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (currentBlock->shape[currentBlock->rotation] & (0x8000 >> (i * 4 + j)))
			{
				int x = (currentBlock->x - BoardStartX) / BlockSize + j;
				int y = (currentBlock->y - BoardStartY) / BlockSize + i;

				if (x >= 0 && x < BoardWidth && y < BoardHeight && y >= 0)
				{
					Board[y][x] = 1;
				}

			}
		}				
	}
}

// �� ȸ�� �Լ�
void Tetris::RotateBlock()
{
	if (gameOver) return;

	int nextRotation = (currentBlock->rotation + 1) % 4; // 0 ~ 3

	// �ӽ� ȸ�� ���� �� �浹 �˻�
	if (!CanRotate(currentBlock, nextRotation))
	{
		return; // ȸ���� �Ұ����ϸ� ���
	}

	currentBlock->rotation = nextRotation;
}

bool Tetris::CanRotate(TetrisBlock* block, int nextRotation)
{
	if (gameOver) return false;

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			if (block->shape[nextRotation] & (0x8000 >> (y * 4 + x)))
			{
				// ���� ���� �� ��ġ�� �׸���� ��ȯ
				int boardX = (block->x - BoardStartX) / BlockSize + x;
				int boardY = (block->y - BoardStartY) / BlockSize + y;

				// ���� ������ ����� ȸ�� �Ұ�
				if (boardX < 0 || boardX >= BoardWidth || boardY >= BoardHeight || boardY < 0)
				{
					return false;
				}
				// �ٸ� ���� ��ġ�� ȸ�� �Ұ���
				else if (Board[boardY][boardX] == 1)
				{
					return false;
				}
			}
		}
	}
	return true; // ���ܻ��� ������ ȸ�� ����
}

// �� ���� �Լ�
// j�� ��簪�� 1�� ������ i�� �ش��ϴ� ���� ����
// (�ٻ����� �������� �� ��� �ѹ��� �ǵ��� �����غ���)
// ���� �� �� ���� �ִ� ������ �� ĭ �Ʒ��� �̵�
// ���� �ý��� �߰�
void Tetris::DeleteBlocks()
{
	if (gameOver) return;
	// ���� ���� ���� �ѹ��� �����ϱ� ���� ������ ���� ����� ����
	vector<int> fullRows;

	bool isFull = true;

	// ���� �����ϴ��� üũ
	for (int i = 20; i >= 0; i--)
	{
		for (int j = 0; j < 10; j++)
		{
			// ���� �ϳ��� �������� �ʴ� ���� ���
			if (Board[i][j] == 0)
			{
				isFull = false;
				break;
			}
			else
			{
				isFull = true;
			}

		}

		if (isFull)
		{
			// ������ �� ��ȣ�� ���
			fullRows.push_back(i);			
		}
	}

	// ���� ����
	if (!fullRows.empty())
	{
		// �⺻ �� ���� ���� 10��
		score += fullRows.size() * 10;

		// 1�� �̻� ������ �� �ٴ� 5�� �߰�
		if (fullRows.size() > 1)
		{
			score += fullRows.size() * 5;
		}
	}

	// ����� �� ����, �ش� ���� 0���� �ʱ�ȭ
	for (int row : fullRows)
	{
		for (int j = 0; j < 10; j++)
		{
			Board[row][j] = 0; // �� �� ����, �ش� ���� 0���� �ʱ�ȭ
		}
	}

	// ������ ���� ������ŭ ����� ����
	//               ������ ���� ����
	for (int idx = fullRows.size() - 1; idx >= 0; idx--)
	{
		// ������ ���� �ε���
		int row = fullRows[idx];

		for (int i = row; i > 0; i--)
		{
			for (int j = 0; j < 10; j++)
			{
				// ��ĭ�� ���� ������.
				Board[i][j] = Board[i - 1][j];
			}
		}
	}

}

// �������� �ִ� �� �׷��ִ� �Լ�
void Tetris::DrawBlock(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (currentBlock->shape[currentBlock->rotation] & (0x8000 >> (i * 4 + j)))
			{
				D2D1_RECT_F rect = D2D1::RectF(
					currentBlock->x + (j * BlockSize),
					currentBlock->y + (i * BlockSize),
					currentBlock->x + ((j + 1) * BlockSize),
					currentBlock->y + ((i + 1) * BlockSize)
				);

				// �� ���� ä���ֱ�
				pBrush->SetColor(currentBlock->color);
				pRenderTarget->FillRectangle(rect, pBrush);

				// �� �׵θ� ���� �׸���
				pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
				pRenderTarget->DrawRectangle(rect, pBrush, 1.0f);
			}
		}
	}
}

void Tetris::DrawPreview(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush)
{
	if (gameOver) return;

	float previewX = 30.0f;
	float previewY = 30.0f;
	float previewWidth = 140.0f;
	float previewHeight = 140.0f;

	// �̸����� ���� ����� ���������� ä���
	D2D1_RECT_F previewBackground = D2D1::RectF(
		previewX,
		previewY,
		previewX + previewWidth,
		previewY + previewHeight
	);
	// �̸����� ��� ���������� �ʱ�ȭ
	pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	pRenderTarget->FillRectangle(previewBackground, pBrush);

	// �̸����� �׵θ� ��
	pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
	pRenderTarget->DrawRectangle(previewBackground, pBrush, 1.0f);

	if (nextBlock)
	{
		float blockStartX = previewX + (previewWidth - 4 * BlockSize) / 2;
		float blockStartY = previewY + (previewHeight - 4 * BlockSize) / 2;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (nextBlock->shape[nextBlock->rotation] & (0x8000 >> (i * 4 + j)))
				{
					D2D1_RECT_F rect = D2D1::RectF(
						previewX + (j * BlockSize),
						previewY + (i * BlockSize),
						previewX + ((j + 1) * BlockSize),
						previewY + ((i + 1) * BlockSize)
					);

					// �� ���� ä���ֱ�
					pBrush->SetColor(nextBlock->color);
					pRenderTarget->FillRectangle(rect, pBrush);

					// �� �׵θ� ���� �׸���
					pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
					pRenderTarget->DrawRectangle(rect, pBrush, 1.0f);
				}
			}
		}
	}
	
}

// ���̴� ���� �׷��ִ� �Լ�
void Tetris::DrawBoard(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush)
{
	for (int y = 0; y < BoardHeight; y++)
	{
		for (int x = 0; x < BoardWidth; x++)
		{
			if (Board[y][x] == 1)
			{
				D2D1_RECT_F rect = D2D1::RectF(
					BoardStartX + (x * BlockSize),
					BoardStartY + (y * BlockSize),
					BoardStartX + ((x + 1) * BlockSize),
					BoardStartY + ((y + 1) * BlockSize)
				);

				pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Gray));
				pRenderTarget->FillRectangle(rect, pBrush);

				pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
				pRenderTarget->DrawRectangle(rect, pBrush);
			}
		}
	}
}

void Tetris::DrawScore(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush)
{
	IDWriteFactory* pDWriteFactory = nullptr; // DirectWrite(�ؽ�Ʈ ������ API)���� �ؽ�Ʈ ������ �����ϴ� ���丮 ��ü.
	IDWriteTextFormat* pTextFormat = nullptr; // ��Ʈ, ũ��, ��Ÿ�� ���� ������ �����ϴ� ��ü.

	// Direct2D �ؽ�Ʈ �ۼ� ���丮 ����
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory)
	);

	// �ؽ�Ʈ ���� ����
	pDWriteFactory->CreateTextFormat(
		L"Arial",                   // ��Ʈ
		NULL,                       // ��Ʈ �÷���
		DWRITE_FONT_WEIGHT_NORMAL,  // ��Ʈ �β�
		DWRITE_FONT_STYLE_NORMAL,   // ��Ʈ ��Ÿ��
		DWRITE_FONT_STRETCH_NORMAL, // ��Ʈ ��Ʈ��ġ
		20.0f,                      // ��Ʈ ũ��
		L"ko-kr",                   // ������ (�ѱ���)
		&pTextFormat                // ��� �ؽ�Ʈ ����
	);

	// �ؽ�Ʈ ���� ����
	pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); // ��� ����
	pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); // ���� �߾� ����

	// ���� �ؽ�Ʈ ����
	WCHAR scoreText[100];
	swprintf_s(scoreText, L"����: %d", score);

	// �ؽ�Ʈ ���� ����
	D2D1_RECT_F textRect = D2D1::RectF(
		520.0f,  // ����
		30.0f,   // ����
		670.0f,  // ������
		60.0f    // �Ʒ���
	);

	// �ؽ�Ʈ ���
	pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	pRenderTarget->FillRectangle(textRect, pBrush);

	// �ؽ�Ʈ �׵θ�
	pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
	pRenderTarget->DrawRectangle(textRect, pBrush, 1.0f);

	// �ؽ�Ʈ ����
	pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));

	// �ؽ�Ʈ �׸���
	pRenderTarget->DrawText(
		scoreText,                // �ؽ�Ʈ
		wcslen(scoreText),        // �ؽ�Ʈ ����
		pTextFormat,              // �ؽ�Ʈ ����
		textRect,                 // �ؽ�Ʈ ����
		pBrush                    // �귯��
	);

	// ���ӿ��� ǥ��
	if (gameOver) {
		D2D1_RECT_F gameOverRect = D2D1::RectF(
			BoardStartX + 50.0f,                    // ����
			BoardStartY + (BoardHeight / 2) * BlockSize - 25.0f, // ����
			BoardStartX + (BoardWidth * BlockSize) - 50.0f,      // ������
			BoardStartY + (BoardHeight / 2) * BlockSize + 25.0f  // �Ʒ���
		);

		// ���ӿ��� ���
		pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkRed, 0.8f));
		pRenderTarget->FillRectangle(gameOverRect, pBrush);

		// ���ӿ��� �ؽ�Ʈ
		pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		pRenderTarget->DrawText(
			L"GAME OVER",        // �ؽ�Ʈ
			9,                    // �ؽ�Ʈ ����
			pTextFormat,          // �ؽ�Ʈ ����
			gameOverRect,         // �ؽ�Ʈ ����
			pBrush                // �귯��
		);
	}

	// ���ҽ� ���� (�޸� ���� ����)
	if (pTextFormat) pTextFormat->Release();
	if (pDWriteFactory) pDWriteFactory->Release();
}

// �� �̵�
void Tetris::BlockMove(int dx, int dy)
{
	if (gameOver) return;

	if (!currentBlock) return;

	// �̵����� ���� ���ο� ��ġ ���
	int newX = (currentBlock->x - BoardStartX) / BlockSize + dx;
	int newY = (currentBlock->y - BoardStartY) / BlockSize + dy;

	// �̵� ���� ���� Ȯ��
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (currentBlock->shape[currentBlock->rotation] & (0x8000 >> (i * 4 + j)))
			{
				int checkX = newX + j; // �̵��� ���ο� x ��ġ üũ
				int checkY = (currentBlock->y - BoardStartY) / BlockSize + i; // ���� y ��ġ üũ

				// ���̳� �ٸ� ��ϰ� �浹�ϸ� �̵� �Ұ�
				if (checkX < 0 || checkX >= BoardWidth || Board[checkY][checkX] == 1) return;
			}
		}
	}

	// �̵� ����
	currentBlock->x += dx * BlockSize;
	currentBlock->y += dy * BlockSize;
}

// ����ȯ
void Tetris::BlockExchange()
{	
	if (gameOver) return;
	// �� ��ȯ ����
	exchangeBlockCheck = true;

	if (exchangeBlockCheck)
	{
		// ���� ���� ����
		TetrisBlock* saveBlock_1 = currentBlock;
		TetrisBlock* saveBlock_2 = nextBlock;

		// �� ���� ��ȯ
		currentBlock = saveBlock_2;
		nextBlock = saveBlock_1;

		// �ٲ� ���� �� ��ġ ����
		currentBlock->x = BoardStartX + (BoardWidth / 2 - 2) * BlockSize;
		currentBlock->y = 0;

		// �ٲ� ���� �� ��ġ ����
		nextBlock->x = 50.0f;
		nextBlock->y = 50.0f;

		// �� ��ȯ �Ұ���
		exchangeBlockCheck = false;
	}
	

}

void Tetris::MoveDown()
{
	if (gameOver) return;

	currentBlock->y += BlockSize;
}

void Tetris::CheckGameOver()
{
	for (int x = 0; x < BoardWidth; x++)
	{
		if (Board[0][x] == 1)
		{
			gameOver = true;
			return;
		}
	}

	if (currentBlock)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (currentBlock->shape[currentBlock->rotation] & (0x8000 >> (i * 4 + j)))
				{
					int x = (currentBlock->x - BoardStartX) / BlockSize + j;
					int y = (currentBlock->y - BoardStartY) / BlockSize + i;

					// x, y�� ���� ���� �ְ�, �ش� ��ġ�� �̹� ����� ������ ���ӿ���
					if (x >= 0 && x < BoardWidth && y >= 0 && y < BoardHeight && Board[0][x] == 1)
					{
						gameOver = true;
						return;
					}
				}
			}
		}
	}
}