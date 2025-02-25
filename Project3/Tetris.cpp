#include "Tetris.h"

#include <vector>

using namespace std;
TetrisBlock T_Block =
{
	// Bitmask 회전사용
	{
		0x0E40, // 0도
		0x04C4, // 90도
		0x04E0, // 180도
		0x4640  // 270도
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
	0.0f,								// 초기 x 위치
	0.0f,								// 초기 y 위치
	D2D1::ColorF(D2D1::ColorF::Cyan),	// 색상
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
		// 최초 한 번 난수 생성
		srand(time(NULL));
		initialized = true;
	}

	TetrisBlock* newBlock = nullptr;
	int randomBlock = rand() % 7; // 0~6 사이 랜덤 숫자 생성

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
			// 보드 기준으로 블럭사이즈를 한 칸으로 초반 위치를 중앙으로 형성
			// 보드 중앙에 형성
			newBlock->x = BoardStartX + (BoardWidth / 2 - 2) * BlockSize;
			newBlock->y = 0;
		}
		else
		{
			// 블럭 교환 체크
			if (!exchangeBlockCheck)
			{
				// 미리보기 블럭의 위치 설정
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
	// 보드에 블럭이 나타나는 오류를 없애기 위해 예외 처리
	for (int y = 0; y < BoardHeight; y++)
	{
		for (int x = 0; x < BoardWidth; x++)
		{
			Board[y][x] = 0;
		}
	}

	// 1초마다 블록을 떨어뜨리는 타이머
	SetTimer(hwnd, 1, 500, nullptr);

	// 초기 블록 생성
	currentBlock = CreateRandomBlcok();
	nextBlock = CreateRandomBlcok();	
}

// 블럭 떨어지는 속도 조절 함수
void Tetris::UpdateDropSpeed(HWND hwnd, int newSpeed)
{
	BlockSpeed = newSpeed;
	KillTimer(hwnd, 1); // 기존 타이머 제거
	SetTimer(hwnd, 1, BlockSpeed, nullptr); // 새로운 속도로 타이머 재설정
}

// 바닥 충돌 체크 함수
bool Tetris::CheckCollision()
{
	if (!currentBlock) return false;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (currentBlock->shape[currentBlock->rotation] & (0x8000 >> (i * 4 + j)))
			{
				// 보드 안에서 위치를 설정해야 하기 때문에 보드의 시작 좌표를 빼고, 블럭사이즈로 보드를 그리드로 만들어서 블럭 위치 조정
				int x = (currentBlock->x - BoardStartX) / BlockSize + j;
				// 한 칸 더 아래로 검사하여 블럭의 충돌 처리에 오류없게 확인
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

// 블럭을 쌓아주는 함수
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

// 블럭 회전 함수
void Tetris::RotateBlock()
{
	if (gameOver) return;

	int nextRotation = (currentBlock->rotation + 1) % 4; // 0 ~ 3

	// 임시 회전 적용 후 충돌 검사
	if (!CanRotate(currentBlock, nextRotation))
	{
		return; // 회전이 불가능하면 취소
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
				// 보드 안의 블럭 위치를 그리드로 변환
				int boardX = (block->x - BoardStartX) / BlockSize + x;
				int boardY = (block->y - BoardStartY) / BlockSize + y;

				// 보드 밖으로 벗어나면 회전 불가
				if (boardX < 0 || boardX >= BoardWidth || boardY >= BoardHeight || boardY < 0)
				{
					return false;
				}
				// 다른 블럭과 겹치면 회전 불가능
				else if (Board[boardY][boardX] == 1)
				{
					return false;
				}
			}
		}
	}
	return true; // 예외사항 없으면 회전 가능
}

// 블럭 제거 함수
// j의 모든값이 1이 됐을때 i에 해당하는 줄이 삭제
// (줄삭제가 여러개가 될 경우 한번에 되도록 수정해보기)
// 삭제 된 줄 위에 있는 블럭들은 한 칸 아래로 이동
// 점수 시스템 추가
void Tetris::DeleteBlocks()
{
	if (gameOver) return;
	// 여러 개의 줄을 한번에 삭제하기 위해 삭제할 줄의 목록을 저장
	vector<int> fullRows;

	bool isFull = true;

	// 블럭이 존재하는지 체크
	for (int i = 20; i >= 0; i--)
	{
		for (int j = 0; j < 10; j++)
		{
			// 블럭이 하나라도 존재하지 않는 줄일 경우
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
			// 삭제할 줄 번호를 등록
			fullRows.push_back(i);			
		}
	}

	// 점수 증가
	if (!fullRows.empty())
	{
		// 기본 줄 삭제 점수 10점
		score += fullRows.size() * 10;

		// 1줄 이상 삭제시 한 줄당 5점 추가
		if (fullRows.size() > 1)
		{
			score += fullRows.size() * 5;
		}
	}

	// 저장된 줄 삭제, 해당 행을 0으로 초기화
	for (int row : fullRows)
	{
		for (int j = 0; j < 10; j++)
		{
			Board[row][j] = 0; // 한 줄 삭제, 해당 행을 0으로 초기화
		}
	}

	// 삭제된 줄의 개수만큼 블록을 내림
	//               삭제한 줄의 개수
	for (int idx = fullRows.size() - 1; idx >= 0; idx--)
	{
		// 삭제된 줄의 인덱스
		int row = fullRows[idx];

		for (int i = row; i > 0; i--)
		{
			for (int j = 0; j < 10; j++)
			{
				// 한칸씩 줄을 내린다.
				Board[i][j] = Board[i - 1][j];
			}
		}
	}

}

// 떨어지고 있는 블럭 그려주는 함수
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

				// 블럭 색상 채워주기
				pBrush->SetColor(currentBlock->color);
				pRenderTarget->FillRectangle(rect, pBrush);

				// 블럭 테두리 색상 그리기
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

	// 미리보기 영역 배경을 검은색으로 채우기
	D2D1_RECT_F previewBackground = D2D1::RectF(
		previewX,
		previewY,
		previewX + previewWidth,
		previewY + previewHeight
	);
	// 미리보기 배경 검은색으로 초기화
	pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	pRenderTarget->FillRectangle(previewBackground, pBrush);

	// 미리보기 테두리 색
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

					// 블럭 색상 채워주기
					pBrush->SetColor(nextBlock->color);
					pRenderTarget->FillRectangle(rect, pBrush);

					// 블럭 테두리 색상 그리기
					pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
					pRenderTarget->DrawRectangle(rect, pBrush, 1.0f);
				}
			}
		}
	}
	
}

// 쌓이는 블럭들 그려주는 함수
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
	IDWriteFactory* pDWriteFactory = nullptr; // DirectWrite(텍스트 렌더링 API)에서 텍스트 서식을 생성하는 팩토리 객체.
	IDWriteTextFormat* pTextFormat = nullptr; // 폰트, 크기, 스타일 등의 정보를 저장하는 객체.

	// Direct2D 텍스트 작성 팩토리 생성
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory)
	);

	// 텍스트 포맷 생성
	pDWriteFactory->CreateTextFormat(
		L"Arial",                   // 폰트
		NULL,                       // 폰트 컬렉션
		DWRITE_FONT_WEIGHT_NORMAL,  // 폰트 두께
		DWRITE_FONT_STYLE_NORMAL,   // 폰트 스타일
		DWRITE_FONT_STRETCH_NORMAL, // 폰트 스트레치
		20.0f,                      // 폰트 크기
		L"ko-kr",                   // 로케일 (한국어)
		&pTextFormat                // 결과 텍스트 포맷
	);

	// 텍스트 정렬 설정
	pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); // 가운데 정렬
	pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); // 세로 중앙 정렬

	// 점수 텍스트 생성
	WCHAR scoreText[100];
	swprintf_s(scoreText, L"점수: %d", score);

	// 텍스트 영역 설정
	D2D1_RECT_F textRect = D2D1::RectF(
		520.0f,  // 왼쪽
		30.0f,   // 위쪽
		670.0f,  // 오른쪽
		60.0f    // 아래쪽
	);

	// 텍스트 배경
	pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	pRenderTarget->FillRectangle(textRect, pBrush);

	// 텍스트 테두리
	pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
	pRenderTarget->DrawRectangle(textRect, pBrush, 1.0f);

	// 텍스트 색상
	pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));

	// 텍스트 그리기
	pRenderTarget->DrawText(
		scoreText,                // 텍스트
		wcslen(scoreText),        // 텍스트 길이
		pTextFormat,              // 텍스트 포맷
		textRect,                 // 텍스트 영역
		pBrush                    // 브러시
	);

	// 게임오버 표시
	if (gameOver) {
		D2D1_RECT_F gameOverRect = D2D1::RectF(
			BoardStartX + 50.0f,                    // 왼쪽
			BoardStartY + (BoardHeight / 2) * BlockSize - 25.0f, // 위쪽
			BoardStartX + (BoardWidth * BlockSize) - 50.0f,      // 오른쪽
			BoardStartY + (BoardHeight / 2) * BlockSize + 25.0f  // 아래쪽
		);

		// 게임오버 배경
		pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkRed, 0.8f));
		pRenderTarget->FillRectangle(gameOverRect, pBrush);

		// 게임오버 텍스트
		pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		pRenderTarget->DrawText(
			L"GAME OVER",        // 텍스트
			9,                    // 텍스트 길이
			pTextFormat,          // 텍스트 포맷
			gameOverRect,         // 텍스트 영역
			pBrush                // 브러시
		);
	}

	// 리소스 해제 (메모리 누수 방지)
	if (pTextFormat) pTextFormat->Release();
	if (pDWriteFactory) pDWriteFactory->Release();
}

// 블럭 이동
void Tetris::BlockMove(int dx, int dy)
{
	if (gameOver) return;

	if (!currentBlock) return;

	// 이동했을 때의 새로운 위치 계산
	int newX = (currentBlock->x - BoardStartX) / BlockSize + dx;
	int newY = (currentBlock->y - BoardStartY) / BlockSize + dy;

	// 이동 가능 여부 확인
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (currentBlock->shape[currentBlock->rotation] & (0x8000 >> (i * 4 + j)))
			{
				int checkX = newX + j; // 이동할 새로운 x 위치 체크
				int checkY = (currentBlock->y - BoardStartY) / BlockSize + i; // 현재 y 위치 체크

				// 벽이나 다른 블록과 충돌하면 이동 불가
				if (checkX < 0 || checkX >= BoardWidth || Board[checkY][checkX] == 1) return;
			}
		}
	}

	// 이동 실행
	currentBlock->x += dx * BlockSize;
	currentBlock->y += dy * BlockSize;
}

// 블럭교환
void Tetris::BlockExchange()
{	
	if (gameOver) return;
	// 블럭 교환 가능
	exchangeBlockCheck = true;

	if (exchangeBlockCheck)
	{
		// 기존 블럭들 보관
		TetrisBlock* saveBlock_1 = currentBlock;
		TetrisBlock* saveBlock_2 = nextBlock;

		// 각 블럭들 교환
		currentBlock = saveBlock_2;
		nextBlock = saveBlock_1;

		// 바뀐 현재 블럭 위치 조정
		currentBlock->x = BoardStartX + (BoardWidth / 2 - 2) * BlockSize;
		currentBlock->y = 0;

		// 바뀐 다음 블럭 위치 조정
		nextBlock->x = 50.0f;
		nextBlock->y = 50.0f;

		// 블럭 교환 불가능
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

					// x, y가 범위 내에 있고, 해당 위치에 이미 블록이 있으면 게임오버
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