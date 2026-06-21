// [---Include---]
/* STD */
#include <stdio.h>
#include <stdbool.h>
/* ETC */
#include <conio.h>
#include <windows.h>

// [---Define---]
#define GAME_NAME "편의점에서 살아남기"
#define GAME_VER "v26.0.2.0-0618"

// [---Enum---]
typedef enum
{
    NOMAL
} Customer_id;

typedef enum
{
    SC_EXIT,
    SC_MAIN,
    SC_HELP,
    SC_GAME,
    SC_GAME_SALES,
    SC_GAME_STATS,
    SC_ENDING
} Screen;

// [---Var---]
/* 시스템 변수 */
/** @brief 직전 화면 상태를 저장하는 변수 */
Screen Before_Screen = SC_MAIN;
/** @brief 현재 화면 상태를 저장하는 변수 */
Screen Current_Screen;
/** @brief 메뉴에서 현재 가리키고 있는 화살표의 위치 (1부터 시작) */
int Menu_Pointer = 1;
/** @brief 프로그램의 생명 조건 */
bool Program_Flag = true;
/** @brief 이전 화면으로 돌아갈 수 있는 조건 */
bool CanMoveBack = true;

/* 환경 변수 */
/** @brief 게임을 진행한 날짜 수 */
int Day = 1;
/** @brief 인게임 시간 */
int Hour = 0;
/** @brief 게임 컨트롤러 */
int Tick = 0;

// [---List---]

// [---Struct---]
/**
 * @brief 건물
 * @details 플레이어 및 게임에 등장하는 건물의 데이터 저장
 */
typedef struct
{
    int Money;  /**<돈*/
    int HP;     /**<남은 체력*/
    int repute; /**<호감도*/
} Building;

/**
 * @brief 손님
 * @details 손님의 데이터를 저장
 */
typedef struct
{
    Customer_id ID;  /**<손님의 ID*/
    int Probability; /**<이 손님이 나올 확률 (%)*/
    char *Message;   /**<이 손님의 메시지*/
    int IF_Money;    /**<만약 손님의 요구에 만족할 경우 돈의 증가값*/
    int IF_Repute;   /**<만약 손님의 요구에 만족할 경우 호감도의 증가값*/
    int ELSE_Money;  /**<만약 손님의 요구에 불만족할 경우 돈의 감소값*/
    int ELSE_Repute; /**<만약 손님의 요구에 불만족할 경우 돈의 감소값*/
} Customer;

// [---Entity---]
/** @brief 플레이어의 편의점 건물 데이터 */
Building CS = {.Money = 0, .HP = 100, .repute = 0};

// 손님들
/** @brief 손님1의 데이터 */
Customer C1 = {.ID = NOMAL, .Probability = 35, .Message = "안녕하세요! 빵 하나 주실래요?", .IF_Money = 10, .IF_Repute = 1, .ELSE_Money = 10, .ELSE_Repute = 1};

// [---Function---]
/**
 * @brief 콘솔 텍스트의 글자색과 배경색을 변경하는 함수
 * @param color 색상 코드 값 (0 ~ 255)
 */
void SetColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/**
 * @brief 콘솔 텍스트의 글자색과 배경색을 기본으로 설정하는 함수
 */
void SetDefaultColor(void)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

/**
 * @brief 콘솔 창의 특정 좌표(X, Y)로 커서를 이동시키는 함수
 */
void Gotoxy(int x, int y)
{
    COORD pos = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

/**
 * @brief 화면을 청소하는 함수
 * @details 현재 화면을 모두 비웁니다.
 * */
void CleanScreen()
{
    for (int i = 0; i < 20; i++)
    {
        Gotoxy(0, i);
        printf("                                              ");
    }
}

/**
 * @brief 화면을 그리는 함수
 * @details 현재 화면에 해당하는 화면을 그립니다.
 */
void DrawScreen(void)
{
    SetDefaultColor(); // 색 꼬임 방어
    CleanScreen();
    Gotoxy(0, 0);
    switch (Current_Screen)
    {
    case SC_MAIN:
    {
        SetColor(9);
        printf("  _______________________________  \n");
        printf(" /___________");
        SetDefaultColor();
        printf("_________");
        SetColor(9);
        printf("___________\\ \n");
        printf("/");
        SetDefaultColor();
        printf(" |          | CS 25 |          | ");
        SetColor(9);
        printf("\\ \n");
        SetDefaultColor();
        printf("  |          ‾‾‾‾‾‾‾‾‾          |   \n");
        printf("  |                             |   \n");
        printf("  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾   \n");
        printf("\n");
        printf("\n");
        printf("%s\n", GAME_NAME);
        printf("%s\n", GAME_VER);
        printf("\n");
        /* 메뉴 */
        SetColor(Menu_Pointer == 1 ? 14 : 7);
        printf("%s 종합 키 설명\n", Menu_Pointer == 1 ? ">" : " ");
        SetColor(Menu_Pointer == 2 ? 14 : 7);
        printf("%s 게임 시작하기\n", Menu_Pointer == 2 ? ">" : " ");
        SetColor(Menu_Pointer == 3 ? 14 : 7);
        printf("%s 게임 종료하기\n", Menu_Pointer == 3 ? ">" : " ");
        /* 색 롤백 */
        SetDefaultColor();
        break;
    }
    case SC_HELP:
        SetColor(9);
        printf("종합 키 설명\n");
        SetDefaultColor();
        printf("\n");
        printf("\n");
        printf("w: 위로\n");
        printf("s: 아래로\n");
        SetColor(2);
        printf("Enter: 선택\n");
        SetColor(4);
        printf("BackSpace: 취소/전 화면으로 돌아가기\n");
        printf("\n");
        printf("\n");
        SetColor(6);
        printf("아무 키를 누르면 전 화면으로 돌아갑니다.\n");
        SetDefaultColor();
        break;
    case SC_GAME:
        printf("DAY %d, %d시", Day, Hour);
        printf("\n");
        printf("\n");
        /* 메뉴 */
        SetColor(Menu_Pointer == 1 ? 14 : 7);
        printf("%s 영업 시작하기\n", Menu_Pointer == 1 ? ">" : " ");
        SetColor(Menu_Pointer == 2 ? 14 : 7);
        printf("%s 상태 확인하기\n", Menu_Pointer == 2 ? ">" : " ");
        SetColor(Menu_Pointer == 3 ? 14 : 7);
        printf("%s 다음날로\n", Menu_Pointer == 3 ? ">" : " ");
        SetColor(Menu_Pointer == 4 ? 14 : 7);
        printf("%s 홈화면으로\n", Menu_Pointer == 4 ? ">" : " ");
        /* 색 롤백 */
        SetDefaultColor();
        break;
    case SC_GAME_STATS:
        printf("STATS\n");
        printf("\n");
        printf("\n");
        printf("총 버틴 날짜 수: %d(클리어까지 ", Day);
        SetColor(4);
        if (CS.HP >= 20)
            SetColor(2);
        else if (CS.HP >= 10)
            SetColor(6);
        printf("%d", 31 - Day);
        SetDefaultColor();
        printf("일 남음)\n");
        printf("현재 남은 체력: ");
        SetColor(4);
        if (CS.HP >= 75)
            SetColor(2);
        else if (CS.HP >= 25)
            SetColor(6);
        printf("%d\n", CS.HP);
        SetDefaultColor();
        printf("현재 가지고 있는 골드 수: ");
        SetColor(6);
        printf("%dG\n", CS.Money);
        SetDefaultColor();
        printf("현재 편의점의 평판: ");
        SetColor(4);
        if (CS.HP >= 25)
            SetColor(2);
        else if (CS.HP > 0)
            SetColor(6);
        printf("%d점\n", CS.repute);
        SetDefaultColor();
        printf("\n");
        printf("아무 키를 누르면 전 화면으로 돌아갑니다.\n");
        break;
    case SC_GAME_SALES:
        printf("영업 시작(%d일차)\n", Day);
        printf("현재시간: %s %d시", Hour > 12 ? "오후" : "오전", Hour);
        printf("\n");
        printf("\n");
        SetColor(Menu_Pointer == 1 ? 14 : 7);
        printf("%s 손님 받기\n", Menu_Pointer == 1 ? ">" : " ");
        SetColor(Menu_Pointer == 2 ? 14 : 7);
        printf("%s 영업 종료하기\n", Menu_Pointer == 2 ? ">" : " ");
        SetDefaultColor();
        break;
    case SC_ENDING:
        printf("번 돈: %dG\n", CS.Money);
    default:
        break;
    }
}

/**
 * @brief 화면을 전환하는 함수
 * @details 목표 화면으로 화면 변수를 전환합니다.
 * * @param Target_SC 목표 화면
 */
void MoveScreen(Screen Target_SC)
{
    Screen Temp_SC = Current_Screen;
    Current_Screen = Target_SC;
    Before_Screen = Temp_SC;
    Menu_Pointer = 1;
    DrawScreen();
}

/**
 * @brief 사용자의 키보드 입력을 받는 함수
 * @details Enter를 누를 시 true를 반환, Backspace를 누를 시 전 화면으로 돌아가기, w/s를 누르면 포인터를 이동합니다.
 * * @param Max 포인터가 올라갈 수 있는 최대값 (포함)
 * @param pointer 값을 변동시킬 포인터
 * @return bool을 반환, 사용자의 키보드 입력이 Enter인 경우만 true 이외는 false 반환
 */
bool Point(int Max, int *pointer)
{
    int key = getch();
    if (key == 13)
        return true;
    if (key == 8 && CanMoveBack)
        MoveScreen(Before_Screen);
    else if ((key == 'w' || key == 'W') && *pointer != 1)
    {
        *pointer -= 1;
    }
    else if ((key == 's' || key == 'S') && *pointer != Max)
    {
        *pointer += 1;
    }
    DrawScreen();
    return false;
}

void NextDay(void)
{
    Day++;
    CS.HP += 10;
    DrawScreen();
    if (Day >= 31)
    {
        Tick = 0;
        Hour = 0;
        Day = 0;
        MoveScreen(SC_ENDING);
    }
}

void CheckTick(void)
{
    Tick++;
    if (Tick == 5 * 6000) // [Dev]
    {
        Hour++;
        Tick = 0;
        DrawScreen();
    }
    if (Hour >= 25)
    {
        Day++;
        Hour = 0;
        DrawScreen();
    }
}

//[---Main---]
int main(void)
{
    /* 기본 화면 설정 */
    SetConsoleOutputCP(65001);
    MoveScreen(SC_MAIN);
    /* 콘솔 커서 숨기기 */
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE; // 커서를 보이지 않게 설정
    SetConsoleCursorInfo(consoleHandle, &info);

    /* 게임 루프 */
    while (Program_Flag)
    {
        // 게임 화면 무단 침입 방지
        if (Before_Screen == SC_GAME_SALES)
            Before_Screen = SC_GAME;
        // 시간 추가
        switch (Current_Screen)
        {
        case SC_EXIT:
            Program_Flag = false;
            break;
        case SC_MAIN:
            if (_kbhit())
            {
                if (Point(3, &Menu_Pointer))
                {
                    if (Menu_Pointer == 1)
                        MoveScreen(SC_HELP);
                    if (Menu_Pointer == 2)
                        MoveScreen(SC_GAME);
                    if (Menu_Pointer == 3)
                        MoveScreen(SC_EXIT);
                }
            }
            break;
        case SC_HELP:
            getch();
            MoveScreen(Before_Screen);
            break;
        case SC_GAME:
            CheckTick();
            if (_kbhit())
            {
                if (Point(4, &Menu_Pointer))
                {
                    if (Menu_Pointer == 1)
                        MoveScreen(SC_GAME_SALES);
                    if (Menu_Pointer == 2)
                        MoveScreen(SC_GAME_STATS);
                    if (Menu_Pointer == 3)
                        NextDay();
                    if (Menu_Pointer == 4)
                        MoveScreen(SC_MAIN);
                }
            }
            break;
        case SC_GAME_SALES:
            CheckTick();
            CanMoveBack = false;
            if (_kbhit())
            {
                if (Point(2, &Menu_Pointer))
                {
                    if (Menu_Pointer == 1)
                        break;
                    if (Menu_Pointer == 2)
                        MoveScreen(SC_GAME);
                }
            }
            break;
        case SC_GAME_STATS:
            getch();
            CheckTick();
            if (_kbhit())
                getch();
            MoveScreen(Before_Screen);
            break;
        case SC_ENDING:
            getch();
            getch();
            Program_Flag = false;
        default:
            MoveScreen(SC_EXIT); // 버그 예방
            break;
        }
        Sleep(10);
    }
    /* 프로그램 종료*/
    return 0;
}