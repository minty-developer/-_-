// [---Include---]

/* STD */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
/* TIME */
#include <time.h>
/* IO */
#include <conio.h>
/* OS */
#include <windows.h>

// [---Define---]

/** @brief 게임 이름 */
#define GAME_NAME "편의점에서 살아남기"
/** @brief 게임 버전 */
#define GAME_VER "v26.0.7.0-0626"

// --colors--

#define COLOR_RESET "\x1b[0m"
#define COLOR_RED "\x1b[31m"
#define COLOR_LIGHT_SKYBLUE "\x1b[96m"
#define COLOR_LIGHT_BLUE "\x1b[94m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_LIGHT_YELLOW "\x1b[93m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_GREEN "\x1b[32m"

// [---Enum---]

/** @brief 손님 id */
typedef enum {
    NOMAL
} Customer_id;

/** @brief 화면 id */
typedef enum {
    SC_EXIT,          /**< 게임 종료          */
    SC_MAIN,          /**< 프로그램 시작 화면  */
    SC_HELP,          /**< 도움말 화면        */
    SC_NEXTDAY,       /**< 다음날 대기 화면    */
    SC_GAME,          /**< 게임 메인 화면      */
    SC_GAME_SALES,    /**< 게임 메뉴 화면      */
    SC_GAME_CUSTOMER, /**< 손님 받기 화면      */
    SC_GAME_STATS,    /**< 받기 결과 화면      */
    SC_GAME_RESULT,   /**< 선태 결과 화면      */
    SC_ENDING         /**< 엔딩 화면          */
} Screen;

// [---Struct---]

/**
 * @brief 손님의 요구
 * @details 손님의 요구 데이터 저장
 */
typedef struct {
    int Right_Answer; /**<맞는 대답 */
    char *Answer1;
    char *Answer2;
} Request;

/**
 * @brief 건물
 * @details 플레이어 및 게임에 등장하는 건물의 데이터 저장
 */
typedef struct {
    int Money;  /**<돈*/
    int repute; /**<호감도*/
} Building;

/**
 * @brief 손님
 * @details 손님의 데이터를 저장
 */
typedef struct {
    Customer_id ID;     /**<손님의 ID*/
    int Probability;    /**<이 손님이 나올 확률 (%)*/
    char *Message;      /**<이 손님의 메시지*/
    Request Item;       /**<이 손님의 요구 물건 */
    int Req_Count;      /**<이 손님의 요구 물건의 종류의 개수 */
    int IF_Money;       /**<만약 손님의 요구에 만족할 경우 돈의 증가값*/
    int IF_Repute;      /**<만약 손님의 요구에 만족할 경우 호감도의 증가값*/
    char *IF_Message;   /**<만약 손님의 요구에 만족할 경우 나올 메시지 */
    int ELSE_Money;     /**<만약 손님의 요구에 불만족할 경우 돈의 감소값*/
    int ELSE_Repute;    /**<만약 손님의 요구에 불만족할 경우 돈의 감소값*/
    char *ELSE_Message; /**<만약 손님의 요구에 불만족할 경우 나올 메시지 */
} Customer;

// [---Var---]

/* ----------시스템 변수---------- */
/** @brief 직전 화면 상태를 저장하는 변수 */
Screen Before_Screen = SC_MAIN;
/** @brief 현재 화면 상태를 저장하는 변수 */
Screen Current_Screen;
/** @brief 현재 받고있는 손님의 ID */
Customer NowCustomer;
/** @brief 메뉴에서 현재 가리키고 있는 화살표의 위치 (1부터 시작) */
int Menu_Pointer = 1;
/**
 * @brief 현재 게임의 조건들을 저장하는 변수 (0000 1111)
 * @details 2^3 프로그램의 생명 조건 / 2^2 이전 화면으로 돌아갈 수 있는 조건 / 2^1 화면을 새로 그리는 조건 / 2^0 마지막 요구를 맞춘 플래그
 */
unsigned char conditions = 14;

/* ----------환경 변수---------- */
/** @brief 게임을 진행한 날짜 수 */
int Day = 1;
/** @brief 인게임 시간 */
int Hour = 0;

/* ----------게임 엔티티---------- */
/** @brief 플레이어의 편의점 건물 데이터 */
Building CS = {.Money = 100, .repute = 10};

/** @brief 손님들의 데이터 */
Customer Customer_Book[] = {
    {
        .ID = NOMAL,
        .Probability = 100,
        .Message = "안녕하세요! 빵 하나 주실래요?",
        .Item = {.Right_Answer = 1, .Answer1 = "준다", .Answer2 = "안 준다"},
        .IF_Money = 10,
        .IF_Repute = 1,
        .IF_Message = "감사합니다!!",
        .ELSE_Money = 0,
        .ELSE_Repute = 2,
        .ELSE_Message = "히이잉ㅠㅠ"
    }
};

/** @brief 모든 손님의 수 */
#define ALL_CUSTOMER_COUNT (sizeof(Customer_Book) / sizeof(Customer_Book[0]))

// [---Function---]

/**
 * @brief 화면에 텍스트를 출력하는 함수
 * 
 * @param Format 출력할 내용
 * @param ... 인자
 */
void print(const char *Format, ...)
{
    // 1. 가변 인자 리스트 준비
    va_list args;

    // 2. 가변 인자 시작점 설정 (Format 변수 바로 뒤부터 인자로 인식)
    va_start(args, Format);

    // 3. printf 계열의 가변 인자 스트림을 그대로 받아 출력하는 함수
    vprintf(Format, args);

    // 4. 가변 인자 정리
    va_end(args);
    printf("\033[K" COLOR_RESET "\n");
}

/**
 * @brief 랜덤 손님을 설정해 두는 함수
 * @details 변수 NowCustomer에 가중치 확률을 적용해 손님을 설정합니다.
 */
static inline void GetRandomCustomer(void) {
    // 1부터 100 사이의 난수 생성
    int RN = rand() % 100 + 1;

    int accumulated_probability = 0; // 확률을 누적해서 더해나갈 변수

    for (int i = 0; i < (int)ALL_CUSTOMER_COUNT; i++)
    {
        // 현재 손님의 확률을 더해서 범위를 넓혀나감
        accumulated_probability += Customer_Book[i].Probability;

        // 뽑힌 난수가 누적 범위 안으로 쏙 들어왔다면?
        if (RN <= accumulated_probability)
        {
            NowCustomer = Customer_Book[i];
            return;
        }
    }
}

/**
 * @brief 화면을 전환하는 함수
 * @details 목표 화면으로 화면 변수를 전환합니다.
 * * @param Target_SC 목표 화면
 */
void MoveScreen(Screen Target_SC)
{
    if(Current_Screen == SC_HELP || Current_Screen >= SC_GAME_STATS) getch();
    if(Current_Screen == SC_GAME_SALES) GetRandomCustomer();
    if(Current_Screen == SC_GAME_CUSTOMER) Hour++;
    Screen Temp_SC = Current_Screen;
    Current_Screen = Target_SC;
    Before_Screen = Temp_SC;
    Menu_Pointer = 1;
    conditions |= 1 << 1;
}

/**
 * @brief 다음 날로
 * @details 다음 날로 넘어갑니다.
 */
void NextDay(void)
{
    Day++;
    Hour = 7;
    conditions |= 1 << 1;

    // 31일이 되면 즉시 엔딩으로 가고 함수 종료 (화면 꼬임 방지)
    if (Day >= 31)
    {
        Hour = 0;
        MoveScreen(SC_ENDING);
        return;
    }

    // 31일이 아니면 다음날 연출 화면으로 이동
    MoveScreen(SC_NEXTDAY);
}

/**
 * @brief 게임을 초기화하는 함수
 * 
 */
void init(void) {
    /* 기본 인코딩 설정 */
    SetConsoleOutputCP(65001);
    /* 기본 화면 설정 */
    MoveScreen(SC_MAIN);
    /* 콘솔 커서 숨기기 */
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE; // 커서를 보이지 않게 설정
    SetConsoleCursorInfo(consoleHandle, &info);
    /* 난수 초기화 */
    srand((unsigned int)time(NULL));
    /* Ansi 사용 설정 */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    
    // ENABLE_VIRTUAL_TERMINAL_PROCESSING 상수 값이 정의되어 있지 않다면 0x0004를 사용합니다.
    dwMode |= 0x0004; 
    SetConsoleMode(hOut, dwMode);
}

/**
 * @brief 화면을 그리는 함수
 * @details 현재 화면에 해당하는 화면을 그립니다.
 */
void DrawScreen(void)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){0, 0});
    switch (Current_Screen)
    {
    case SC_MAIN:
        print(COLOR_LIGHT_BLUE "  _______________________________  ");
        print(COLOR_LIGHT_BLUE" /___________" COLOR_RESET "_________" COLOR_LIGHT_BLUE "___________\\ ");
        print(COLOR_LIGHT_BLUE "/" COLOR_RESET " |          | CS 25 |          | " COLOR_LIGHT_BLUE "\\");
        print("  |          ‾‾‾‾‾‾‾‾‾          |  ");
        print("  |                             |  ");
        print("  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾  ");
        print("");
        print("");
        print("%s", GAME_NAME);
        print("%s", GAME_VER);
        print("");
        /* 메뉴 */
        print("%s 종합 키 설명", Menu_Pointer == 1 ? COLOR_LIGHT_YELLOW">" : " ");
        print("%s 게임 시작하기", Menu_Pointer == 2 ? COLOR_LIGHT_YELLOW">" : " ");
        print("%s 게임 종료하기", Menu_Pointer == 3 ? COLOR_LIGHT_YELLOW">" : " ");
        break;
    case SC_HELP:
        print(COLOR_LIGHT_BLUE "종합 키 설명");
        print("");
        print("");
        print("w: 위로");
        print("s: 아래로");
        print(COLOR_GREEN "Enter: 선택");
        print(COLOR_RED "BackSpace: 취소/전 화면으로 돌아가기");
        print("");
        print("");
        print(COLOR_YELLOW "아무 키를 누르면 전 화면으로 돌아갑니다.");
        print("");
        print("");
        print("");
        print("");
        break;
    case SC_NEXTDAY:
        print("다음날로 넘어갑니다...");
        print("");
        print("");
        print("");
        print("");
        print("");
        print("");
        print("");
        print("");
        print("");
        print("");
        print("");
        print("");
        break;
    case SC_GAME:
        print("DAY %d, %d시", Day, Hour);
        print("");
        print("");
        /* 메뉴 */
        print("%s 영업 시작하기",Menu_Pointer == 1 ? COLOR_LIGHT_YELLOW">" : " ");
        print("%s 상태 확인하기", Menu_Pointer == 2 ? COLOR_LIGHT_YELLOW">" : " ");
        print("%s 다음날로", Menu_Pointer == 3 ? COLOR_LIGHT_YELLOW">" : " ");
        print("%s 홈화면으로", Menu_Pointer == 4 ? COLOR_LIGHT_YELLOW">" : " ");
        print("");
        print("");
        print("");
        print("");
        print("");
        print("");
        print("");
        break;
    case SC_GAME_STATS:
        print("STATS");
        print("");
        print("");
        printf("총 버틴 날짜 수: %d(클리어까지 ", Day);
        if ((31 - Day) < 10) printf(COLOR_RED);
        else if ((31 - Day) < 20) printf(COLOR_YELLOW);
        else printf(COLOR_GREEN);
        print("%d" COLOR_RESET "일 남음)", 31 - Day);
        print("현재 가지고 있는 골드 수: " COLOR_YELLOW "%dG", CS.Money);
        printf("현재 편의점의 평판: ");
        if (CS.repute >= 25) printf(COLOR_GREEN);
        else if (CS.repute > 0) printf(COLOR_YELLOW);
        else printf(COLOR_RED);
        print("%d점", CS.repute);
        print("");
        print("아무 키를 누르면 전 화면으로 돌아갑니다.");
        break;
    case SC_GAME_SALES:
        print("영업 시작(%d일차)", Day);
        print("현재시간: %s %d시", Hour > 12 ? "오후" : "오전", Hour);
        print("");
        print("");
        print("%s 손님 받기",Menu_Pointer == 1 ? COLOR_LIGHT_YELLOW">" : " ");
        print("%s 영업 종료하기", Menu_Pointer == 2 ? COLOR_LIGHT_YELLOW">" : " ");
        print("");
        break;
    case SC_GAME_CUSTOMER:
        print("%s", NowCustomer.Message);
        print("");
        print("");
        /* 메뉴 */
        print("%s %s", Menu_Pointer == 1 ? COLOR_LIGHT_YELLOW">" : " ", NowCustomer.Item.Answer1);
        print("%s %s", Menu_Pointer == 2 ? COLOR_LIGHT_YELLOW">" : " ", NowCustomer.Item.Answer2);
        print("");
        break;
    case SC_GAME_RESULT:
        if ((conditions & 1))
        {
            print("%s", NowCustomer.IF_Message);
            print("%c%dG", NowCustomer.IF_Money >= 0 ? '+' : '-', abs(NowCustomer.IF_Money));
            print("%c%d호감도", NowCustomer.IF_Repute >= 0 ? '+' : '-', abs(NowCustomer.IF_Repute));
        }
        else
        {
            print("%s", NowCustomer.ELSE_Message);
            print("%c%d", NowCustomer.ELSE_Money < 0 ? '+' : '-', abs(NowCustomer.ELSE_Money));
            print("%c%d호감도", NowCustomer.ELSE_Repute < 0 ? '+' : '-', abs(NowCustomer.ELSE_Repute));
        }
        print("");
        print("");
        break;
    case SC_ENDING:
        print(COLOR_LIGHT_SKYBLUE "=========================================");
        print(COLOR_LIGHT_SKYBLUE "        🎉 30일간의 영업 종료! 🎉        ");
        print(COLOR_LIGHT_SKYBLUE "=========================================");
        print("");
        print(COLOR_LIGHT_SKYBLUE "최종 결과 -> 번 돈: %dG | 최종 평판: %d점", CS.Money, CS.repute);
        print("");

        // 🏆 조건에 따른 엔딩 분기 출력
        if (CS.Money >= 500 && CS.repute >= 30)
        {
            print(COLOR_GREEN "[ 👑 대박 엔딩: 편의점 황제 ]");
            print(COLOR_GREEN "당신의 편의점은 지역 명물이 되었습니다!");
            print(COLOR_GREEN "본사에서 당신을 최우수 점장으로 선정하고,");
            print(COLOR_GREEN "강남 직영점의 총괄 매니저로 파격 스카우트합니다!");
        }
        else if (CS.Money < 50 || CS.repute <= 0)
        {
            print(COLOR_RED "[ 💸 쪽박 엔딩: 폐업, 그리고 야간 알바... ]");
            print(COLOR_RED "손님들의 원성과 적자를 버티지 못하고");
            print(COLOR_RED "결국 편의점 문을 닫게 되었습니다.");
            print(COLOR_RED "당신은 이제 다른 편의점의 야간 알바로 취업합니다...ㅠㅠ");
        }
        else
        {
            print(COLOR_YELLOW "[ 🏪 평범한 엔딩: 골목길의 터줏대감 ]");
            print(COLOR_YELLOW "비록 엄청난 대박을 치지는 못했지만,");
            print(COLOR_YELLOW "단골손님들과 소소한 정을 나누는");
            print(COLOR_YELLOW "정겨운 동네 편의점으로 자리 잡았습니다.");
        }
        print("");
        print("=========================================");
        print(COLOR_YELLOW "아무 키나 누르면 게임이 종료됩니다.");
        break;
    default:
        break;
    }
    fflush(stdout);
    conditions &= ~(1 << 1);
    return;
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
    if (key == 8 && (conditions >> 2))
        MoveScreen(Before_Screen);
    else if ((key == 'w' || key == 'W') && *pointer != 1)
    {
        *pointer -= 1;
        conditions |= 1 << 1;
    }
    else if ((key == 's' || key == 'S') && *pointer != Max)
    {
        *pointer += 1;
        conditions |= 1 << 1;
    }
    return false;
}

//[---Main---]
/**
 * @brief 프로그램의 시작점
 * 
 * @return int 상태값 반환
 */
int main(void) {
    /* 초기화 */
    init();

    /* 게임 루프 */
    while ((conditions >> 3) & 1) {
        // 시간 확인
        if (Current_Screen >= SC_GAME && Current_Screen <= SC_GAME_STATS && Hour >= 20) NextDay();
        // 화면 갱신 확인
        if ((conditions >> 1) & 1) DrawScreen();
        // 게임 화면 무단 침입 방지
        if (Before_Screen == SC_GAME_SALES) Before_Screen = SC_GAME;
        // 화면 분기
        switch (Current_Screen) {
            case SC_EXIT: conditions &= ~(1 << 3); break;
            case SC_MAIN:
                if (!Point(3, &Menu_Pointer)) continue;
                // --선택 했을 시--
                if (Menu_Pointer == 1) MoveScreen(SC_HELP);
                if (Menu_Pointer == 2) MoveScreen(SC_GAME);
                if (Menu_Pointer == 3) MoveScreen(SC_EXIT);
                break;
            case SC_HELP: MoveScreen(Before_Screen); break;
            case SC_NEXTDAY: Sleep(3000); MoveScreen(SC_GAME); break;
            case SC_GAME:
                //버튼을 누르지 않으면 넘어가지 않으므로 미리 증가
                conditions |= 1 << 2;
                //만약 선택을 하지 않았다면 넘어감
                if (!Point(4, &Menu_Pointer)) continue;
                // --선택 했을 시--
                if (Menu_Pointer == 1) MoveScreen(SC_GAME_SALES);
                if (Menu_Pointer == 2) MoveScreen(SC_GAME_STATS);
                if (Menu_Pointer == 3) NextDay();
                if (Menu_Pointer == 4) MoveScreen(SC_MAIN);
                break;
            case SC_GAME_SALES:
                //버튼을 누르지 않으면 넘어가지 않으므로 미리 증가
                conditions &= ~(1 << 2);
                // 만약 선택을 하지 않았다면 넘어감
                if (!Point(2, &Menu_Pointer)) continue;
                // --선택 했을 시--
                if (Menu_Pointer == 1) MoveScreen(SC_GAME_CUSTOMER);
                if (Menu_Pointer == 2) MoveScreen(SC_GAME);
                break;
            case SC_GAME_CUSTOMER:
                // 만약 선택을 하지 않았다면 넘어감
                if (!Point(2, &Menu_Pointer)) continue;
                if (Menu_Pointer == NowCustomer.Item.Right_Answer) {
                    CS.Money += NowCustomer.IF_Money;
                    CS.repute += NowCustomer.IF_Repute;
                    conditions |= 1;
                } else {
                    CS.Money -= NowCustomer.ELSE_Money;
                    CS.repute -= NowCustomer.ELSE_Repute;
                    conditions &= 0;
                }
                MoveScreen(SC_GAME_RESULT);
                break;
            case SC_GAME_RESULT: MoveScreen(SC_GAME_SALES); break;
            case SC_GAME_STATS: MoveScreen(Before_Screen); break;
            case SC_ENDING: MoveScreen(SC_EXIT); break;
            // 버그 예방
            default: MoveScreen(SC_EXIT); break;
        }
        // 입력 버퍼 비우기
        while(_kbhit()) getch();
        // CPU 과열 방지
        Sleep(0);
    }
    /* 프로그램 종료*/
    return 0;
}