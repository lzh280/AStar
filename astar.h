#ifndef ASTAR_H
#define ASTAR_H

#include <QWidget>
#include <QDebug>

#define ROW 10 // 最大行数，常用于x，数组前下标
#define COL 20 // 最大列数，常用于y，数组后下标
#define CELL_COST 10 // 行走一格的代价
#define WALKED  (1) // 判断是否已经走过，

// 定义屏幕坐标方向，前面坐标控制左右（x），后面坐标控制上下（y）
#define DIR_UP      QPoint(0,-1)
#define DIR_DOWN    QPoint(0,1)
#define DIR_LEFT    QPoint(-1,0)
#define DIR_RIGHT   QPoint(1,0)

// F = G + H
// gCost为当前已走的代价
// hCost表示预估代价

// 设计一个节点模式，
typedef struct Cell_Struct{
    QPoint pos; // 当前格子的坐标
    int gCost;
    int hCost;
    struct Cell_Struct* parent; // 走到当前格子的父格子
    struct Cell_Struct* Childs[4]; // 此格子往4个方向，走向4个不同的格子
}Cell_t;

extern QPoint *pnowPos;
extern Cell_t* prootCell;
extern Cell_t* pnowCell;
// 辅助地图，用于记录是否走过
extern QPoint route[ROW * COL];
extern char walkMark[ROW * COL];
extern QPoint walkDir[4];


void AStarInit(QPoint _startPos,QPoint _endPos);
void AStarLoadMap(char* map, int w, int l);
char AStarSearch();
void AStarSave();
void PushCostCell(Cell_t* pCell);
Cell_t* PopMinCostCell(void);

#endif // ASTAR_H
