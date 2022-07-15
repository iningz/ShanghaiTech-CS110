#include "lcd/lcd.h"
#include <string.h>
#include "utils.h"
#include "queue.h"

typedef enum {MAIN, HELP, GAME, BOARD, OPTIONS} MODE;
typedef enum {PLAYSEL, OPTIONSSEL, HELPSEL} MENUSEL;
typedef enum {RETRYSEL, QUITSEL} SCORESEL;
typedef enum {SPEEDSEL, DIFFSEL, RETURNSEL} OPTIONSEL;
typedef enum {UP, LEFT, DOWN, RIGHT} DIRECTION;
typedef enum {STRAIGHT, TOLEFT, TORIGHT} HEADING;
typedef enum {IDLE, BODY, FOOD, POISON} BLOCKSTAT;

const uint32_t dt = 50;
const u16 WIDTH = 4;
uint32_t step = 200;
int foodChance = 80;
#define XBORDER 40
#define YBORDER 20
BLOCKSTAT Block[XBORDER * YBORDER];
DIRECTION currDirection = UP;

static inline void setBlock(Position p, BLOCKSTAT s) {
    Block[p.x * YBORDER + p.y] = s;
}

static inline bool cmpBlock(Position p, BLOCKSTAT s) {
    return Block[p.x * YBORDER + p.y] == s;
}

void DrawBlock(Position p, u16 color) {
    LCD_Fill(p.x * WIDTH, p.y * WIDTH, p.x * WIDTH + WIDTH, p.y * WIDTH + WIDTH, color);
}

void Inp_init(void) {
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

void Adc_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1);
    RCU_CFG0|=(0b10<<14)|(1<<28);
    rcu_periph_clock_enable(RCU_ADC0);
    ADC_CTL1(ADC0)|=ADC_CTL1_ADCON;
}

void IO_init(void)
{
    Inp_init(); // inport init
    Adc_init(); // A/D init
    Lcd_Init(); // LCD init
}

Position updateDir(Queue* q) {
    //Position tail = Queue_front(q);
    Position head = Queue_back(q);
    switch (currDirection) {
    case UP:
        return (Position){head.x, head.y - 1};
    case LEFT:
        return (Position){head.x - 1, head.y};
    case DOWN:
        return (Position){head.x, head.y + 1};
    case RIGHT:
        return (Position){head.x + 1, head.y};
    }
}

void updateFood() {
    Position p;
    BLOCKSTAT foodType;
    do {
        do {
            p.x = rand() % (XBORDER - 2) + 1; p.y = rand() % (YBORDER - 2) + 1;
        } while (!cmpBlock(p, IDLE));

        foodType = Random_percentage(foodChance) ? FOOD : POISON; // possibility = 25%
        setBlock(p, foodType);

        if (foodType == POISON)
            DrawBlock(p, BRRED);
        else
            DrawBlock(p, WHITE);
    } while (foodType != FOOD); // generate a piece of food with some possible poison
}

void gameInit(Queue* q) {
    LCD_Clear(BACK_COLOR);
    Queue_clear(q);
    for (int i = 0; i < XBORDER; i++) {
        for (int j = 0; j < YBORDER; j++) {
            setBlock((Position){i, j}, IDLE);
        }
    }
    currDirection = UP;
    Position head = {XBORDER / 2, YBORDER / 2};
    Position tail = {XBORDER / 2, YBORDER / 2 - 1};
    Queue_push(q, head);
    Queue_push(q, tail);
    DrawBlock(head, WHITE);
    DrawBlock(tail, WHITE);
    setBlock(head, BODY);
    setBlock(tail, BODY);
    srand(get_timer_value());
    updateFood();
}

int main(void)
{
    IO_init();         // init OLED
    // YOUR CODE HERE
    LCD_Clear(BLACK);
    BACK_COLOR=BLACK;
    u8 buttonDown[2] = {0};
    u8 buttonHold[2] = {0};
    u8 buttonUp[2] = {0};
    int state = MAIN;
    MENUSEL menuSelection = PLAYSEL;
    SCORESEL scoreSelection = RETRYSEL;
    OPTIONSEL optionsSelection = SPEEDSEL;
    u8 currentSpeed = 1;
    u8 currentDifficulty = 1;

    //Init
    Draw_menu();

    int curentScore = 0;
    Queue* q = Queue_create(128);
    //uint32_t lastTime = 0;
    uint32_t timer = 0;
    HEADING turn = STRAIGHT;

    while (1) {
        //Input
        for (int i = 0; i < 2; i++) {
            if (Get_Button(i)) {
                buttonDown[i] = buttonHold[i] ? 0 : 1;
                buttonHold[i] = 1;
                buttonUp[i] = 0;
            } else {
                buttonUp[i] = buttonHold[i] ? 1 : 0;
                buttonHold[i] = 0;
                buttonDown[i] = 0;
            }
        }

        //State behaviour
        switch (state)
        {
        case MAIN:
            //actions:
            if (buttonDown[0]) {
                menuSelection = (menuSelection + 1) % 3;
                Draw_selection(12, 50, 48, 70, menuSelection == PLAYSEL);
                Draw_selection(50, 50, 110, 70, menuSelection == OPTIONSSEL);
                Draw_selection(112, 50, 148, 70, menuSelection == HELPSEL);
            }
            //transitions:
            if (!buttonDown[1]) break;
            if (menuSelection == PLAYSEL) {
                state = GAME;
                gameInit(q);
                curentScore = 2;
                timer = 0;
                break;
            }
            if (menuSelection == HELPSEL) {
                state = HELP;
                LCD_Clear(BACK_COLOR);
                Draw_help();
                break;
            }
            if (menuSelection == OPTIONSSEL) {
                state = OPTIONS;
                LCD_Clear(BACK_COLOR);
                Draw_options();
                optionsSelection = 0;
                LCD_ShowNum(96, 22, currentSpeed + 1, 1, YELLOW);
                LCD_ShowNum(96, 42, currentDifficulty + 1, 1, YELLOW);
                break;
            }
            break;
        case HELP:
            //actions:

            //transitions:
            if (buttonDown[0] || buttonDown[1]) {
                //transition to main menu:
                state = MAIN;

                menuSelection = 0;
                LCD_Clear(BACK_COLOR);
                Draw_menu();
            }
            break;
        case GAME:
            timer += dt;
            if (buttonDown[0]) {
                turn = TOLEFT;
            } else if (buttonDown[1]) {
                turn = TORIGHT;
            }
            if (timer < step) break;
            //lastTime = get_timer_value();
            timer -= step;

            if (turn == TOLEFT) {
                currDirection = (currDirection + 1) % 4; // update counter-clockwise
            } else if (turn == TORIGHT) {
                currDirection = (currDirection + 3) % 4;
            }
            turn = STRAIGHT;

            Position newHead = updateDir(q);
            if (newHead.x < 0 || newHead.x >= XBORDER ||
                newHead.y < 0 || newHead.y >= YBORDER ||
                cmpBlock(newHead, BODY)) {
                state = BOARD;

                LCD_Clear(BACK_COLOR);
                Draw_scoreboard(curentScore);
                scoreSelection = 0;
            } else if (cmpBlock(newHead, FOOD)) {
                setBlock(newHead, BODY);
                updateFood();
                Queue_push(q, newHead);
                //DrawBlock(newHead, WHITE);
                curentScore++;
            } else if (cmpBlock(newHead, POISON)) {
                //updateFood();
                DrawBlock(newHead, BACK_COLOR);
                setBlock(newHead, IDLE);
                DrawBlock(Queue_pop(q), BACK_COLOR);
                curentScore--;
            } else {
                Position toDel = Queue_pop(q);
                DrawBlock(toDel, BACK_COLOR);
                setBlock(toDel, IDLE);
                Queue_push(q, newHead);
                DrawBlock(newHead, WHITE);
                setBlock(newHead, BODY);
            }
            break;
        case BOARD:
            //actions:
            if (buttonDown[0]) {
                scoreSelection = (scoreSelection + 1) % 2;
                Draw_selection(28, 50, 72, 70, scoreSelection == RETRYSEL);
                Draw_selection(92, 50, 128, 70, scoreSelection == QUITSEL);
            }
            //transitions:
            if (!buttonDown[1]) break;
            if (scoreSelection == RETRYSEL) {
                state = GAME;
                gameInit(q);
                curentScore = 2;
                timer = 0;
                break;
            }
            if (scoreSelection == QUITSEL) {
                state = MAIN;

                menuSelection = 0;
                LCD_Clear(BACK_COLOR);
                Draw_menu();
            }
            break;
        case OPTIONS:
            //actions:
            if (buttonDown[0]) {
                optionsSelection = (optionsSelection + 1) % 3;
                Draw_selection(10, 21, 150, 39, optionsSelection == SPEEDSEL);
                Draw_selection(10, 41, 150, 59, optionsSelection == DIFFSEL);
                Draw_selection(10, 61, 150, 79, optionsSelection == RETURNSEL);
            }
            if (!buttonDown[1]) break;
            if (optionsSelection == SPEEDSEL) {
                currentSpeed = (currentSpeed + 1) % 5;
                step = 50 + (4 - currentSpeed) * 50;
                LCD_Fill(96, 22, 104, 38, BACK_COLOR);
                ShowNum_PivotCenter(100, 30, currentSpeed + 1, 1, YELLOW);
                break;
            }
            if (optionsSelection == DIFFSEL) {
                currentDifficulty = (currentDifficulty + 1) % 5;
                foodChance = 100 - currentDifficulty * 20;
                LCD_Fill(96, 42, 104, 58, BACK_COLOR);
                ShowNum_PivotCenter(100, 50, currentDifficulty + 1, 1, YELLOW);
                break;
            }
            if (optionsSelection == RETURNSEL) {
                //transition to main menu:
                state = MAIN;

                menuSelection = 0;
                LCD_Clear(BACK_COLOR);
                Draw_menu();
            }
            break;
        }
        delay_1ms(dt);
    }
}
