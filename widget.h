#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QTime>
#include <QDebug>

#define ROW 10 // 行，常用于x
#define COL 20 // 列，常用于y
#define CELL_COST 10 // 行走一格的代价
#define WALKED  (1) // 判断是否已经走过，

#define DIR_UP      QPoint(-1,0)
#define DIR_DOWN    QPoint(1,0)
#define DIR_LEFT    QPoint(0,-1)
#define DIR_RIGHT   QPoint(0,1)

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
    void AStarSearch();
    QPoint startPos;
    QPoint endPos;
    QPoint route[100];
    QPoint *nowPos;
    QTimer runTimer;
    QTimer updateTimer;
    Cell_t* prootCell;
    Cell_t* pnowCell;

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
