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
#include "astar.h"

#define ROW 10 // 行，常用于x，数组前下标
#define COL 20 // 列，常用于y，数组后下标

#define SCALE (20)

#define CELL_SIZE   QPoint(SCALE,SCALE)



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
    void Init();
    void Search();
    QPoint startPos;
    QPoint endPos;
    QPoint mousePos;
    QTimer runTimer;
    QTimer updateTimer;
    QPushButton startBtn;
    QCheckBox randMapCheckBox;
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
