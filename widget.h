#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QMouseEvent>
#include <QPushButton>
#include <QCheckBox>

#define ROW 20 // 行，常用于x，数组前下标
#define COL 30 // 列，常用于y，数组后下标
#define CELL_COST 10 // 行走一格的代价
#define WALKED  (1) // 判断是否已经走过，
#define SCALE (20)

// 定义对数组的方向，前面坐标控制上下，后面坐标控制左右
#define DIR_UP      QPoint(-1,0)
#define DIR_DOWN    QPoint(1,0)
#define DIR_LEFT    QPoint(0,-1)
#define DIR_RIGHT   QPoint(0,1)
#define CELL_SIZE   QPoint(SCALE,SCALE)

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



QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

    ~Widget();
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void AStarInit();
    void AStarSearch();
    void AStarSave();
    void PushCostCell(Cell_t* pCell);
    Cell_t* PopMinCostCell(void);
    QPoint startPos;
    QPoint endPos;
    QPoint route[ROW * COL];
    QPoint *pnowPos;
    QPoint mousePos;
    QTimer runTimer;
    QTimer updateTimer;
    Cell_t* prootCell;
    Cell_t* pnowCell;
    QPushButton startBtn;
    QCheckBox randMapCheckBox;
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
