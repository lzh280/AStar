#ifndef ASTAR_H
#define ASTAR_H

#include <QWidget>
#include <QDebug>

#define ASTAR_HEIGHT 25 // 最大行数，常用于x，数组前下标
#define ASTAR_WIDTH 25 // 最大列数，常用于y，数组后下标
#define CELL_COST 10 // 行走一格的代价
#define WALKED  (1) // 判断是否已经走过，

#define DIR_CNT     (8)
// 定义屏幕坐标方向，前面坐标控制左右（x），后面坐标控制上下（y）
#define DIR_UP      QPoint(0,-1)
#define DIR_DOWN    QPoint(0,1)
#define DIR_LEFT    QPoint(-1,0)
#define DIR_RIGHT   QPoint(1,0)
#define DIR_LEFTUP      QPoint(-1,-1)
#define DIR_LEFTDOWN    QPoint(-1,1)
#define DIR_RIGHTUP    QPoint(1,-1)
#define DIR_RIGHTDOWN   QPoint(1,1)

#define ASTAR_SUCCEED   (0)
#define ASTAR_FAIL      (1)
#define ASTAR_SEARCH    (2)
#define ASTAR_GOBACK    (3)

// F = G + H
// gCost为当前已走的代价
// hCost表示预估代价

// 设计一个节点模式，
typedef struct Cell_Struct{
    QPoint pos; // 当前格子的坐标
    int gCost;
    int hCost;
    struct Cell_Struct* parent; // 走到当前格子的父格子
    struct Cell_Struct* Childs[DIR_CNT]; // 此格子往4个方向，走向4个不同的格子
}Cell_t;

extern QPoint* pnowPos;
extern Cell_t* prootCell;
extern Cell_t* pnowCell;
extern Cell_t* pbackCell;
// 辅助地图，用于记录是否走过
extern QPoint route[ASTAR_WIDTH * ASTAR_HEIGHT];
extern char walkMark[ASTAR_WIDTH * ASTAR_HEIGHT];
extern QPoint walkDir[DIR_CNT];
extern QPoint popPos;


void AStarInit(QPoint _startPos,QPoint _endPos);
void AStarLoadMap(char* map, int w, int h);
char AStarSearch();
char AStarSave();
void PushCostCell(Cell_t* pCell);
Cell_t* PopMinCostCell(void);

#endif // ASTAR_H
