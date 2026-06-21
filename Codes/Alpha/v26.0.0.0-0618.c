// [---Include---]
/* STD */
#include <stdio.h>
#include <stdbool.h>
/* ETC */
#include <conio.h>
#include <windows.h>

// [---Define---]
#define GAME_NAME "편의점에서 살아남기"
#define GAME_VER "v26.0.0.0-0618"

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
    SC_GAME
} Screen;

// [---Var---]
/* 시스템 변수 */
/** @brief 직전 화면 상태를 저장하는 변수 */
Screen Before_Screen = SC_MAIN;
/** @brief 현재 화면 상태를 저장하는 변수 */
Screen Current_Screen = SC_MAIN;
/** @brief 메뉴에서 현재 가리키고 있는 화살표의 위치 (1부터 시작) */
int Menu_Pointer = 1;
/** @brief 프로그램의 생명 조건 */
bool Program_Flag = true;

/* 환경 변수 */
/** @brief 게임을 진행한 날짜 수 */
int Day = 1;
/** @brief 인게임 시간 */
int Hour = 0;

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
 * @brief 화면을 전환하는 함수
 * @details 목표 화면으로 화면 변수를 전환합니다.
 * * @param Target_SC 목표 화면
 */
void MoveScreen(Screen Target_SC)
{
    Screen Temp_SC = Current_Screen;
    Current_Screen = Target_SC;
    Before_Screen = Temp_SC;
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
    if (key == 8)
    {
        MoveScreen(Before_Screen);
        return false;
    }
    if (key == 'w' || key == 'W')
    {
        if (*pointer == 1)
            return false;
        *pointer = *pointer - 1;
    }
    else if (key == 's' || key == 'S')
    {
        if (*pointer == Max)
            return false;
        *pointer = *pointer + 1;
    }
    return false;
}

/**
 * @brief 화면을 그리는 함수
 * @details 현재 화면에 해당하는 화면을 그립니다.
 */
void DrawScreen(void)
{
    system("cls");
    switch (Current_Screen)
    {
    case SC_MAIN:
    {
        char A1 = Menu_Pointer == 1 ? '>' : ' ';
        char A2 = Menu_Pointer == 2 ? '>' : ' ';
        char A3 = Menu_Pointer == 3 ? '>' : ' ';
        printf("  _______________________________  \n");
        printf(" /_______________________________\\ \n");
        printf("/ |          | CS 25 |          | \\ \n");
        printf("  |          ‾‾‾‾‾‾‾‾‾          |   \n");
        printf("  |                             |   \n");
        printf("  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾   \n");
        printf("\n");
        printf("\n");
        printf("%s\n", GAME_NAME);
        printf("%s\n", GAME_VER);
        printf("\n");
        printf("%c 게임 시작하기\n", A1);
        printf("%c 종합 키 설명\n", A2);
        printf("%c 게임 종료하기\n", A3);
        break;
    }
    case SC_HELP:
        printf("종합 키 설명\n");
        printf("\n");
        printf("\n");
        printf("w: 위로\n");
        printf("s: 아래로\n");
        printf("Enter: 선택\n");
        printf("BackSpace: 취소/전 화면으로 돌아가기\n");
        printf("\n");
        printf("\n");
        printf("아무 키를 누르면 전 화면으로 돌아갑니다.\n");
        break;
    default:
        break;
    }
}

//[---Main---]
int main(void)
{
    /* 기본 화면 설정 */
    SetConsoleOutputCP(65001);

    /* 게임 루프 */
    while (Program_Flag)
    {
        switch (Current_Screen)
        {
        case SC_EXIT:
            Program_Flag = false;
            break;
        case SC_MAIN:
            DrawScreen();
            if (Point(3, &Menu_Pointer))
            {
                if (Menu_Pointer == 3)
                    MoveScreen(SC_EXIT);
                else if (Menu_Pointer == 2)
                    MoveScreen(SC_HELP);
                else
                    MoveScreen(SC_GAME);
            }
            break;
        case SC_HELP:
            DrawScreen();
            getch();
            MoveScreen(Before_Screen);
            break;
        case SC_GAME:
            MoveScreen(SC_EXIT); //[DEV] 임시 확인
            break;
        default:
            MoveScreen(SC_EXIT); // 버그 예방
        }
    }
    /* 프로그램 종료*/
    return 0;
}