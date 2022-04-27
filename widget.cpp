#include "widget.h"
#include "ui_widget.h"

char snakeOccupy[ROW][COL] = {
    {1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1},
    {1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,1},
    {1,0,0,0,0, 0,0,0,0,0, 1,0,0,0,0, 0,0,0,0,1},
    {1,0,0,0,0, 0,0,0,1,1, 1,0,0,0,0, 0,0,0,0,1},
    {1,0,0,0,0, 0,0,0,0,0, 1,0,0,0,0, 0,0,0,0,1},

    {1,0,0,0,0, 0,0,0,0,0, 1,0,0, 0,0,0,0,0,0,1},
    {1,0,0,0,0, 0,0,0,0,0, 1,1,1,1,1, 1,0,0,0,1},
    {1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,1},
    {1,0,0,0,0, 0,0,0,0,0, 1,0,0,0,0, 0,0,0,0,1},
    {1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1},
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    startPos = QPoint(7,3);
    endPos = QPoint(13,5);

    qsrand(QTime::currentTime().msec());

    startBtn.setParent(this);
    startBtn.setText("点击重新开始寻路");
    startBtn.move((COL + 1) * SCALE, 100);
    connect(&startBtn,&QPushButton::clicked,[=](){
        // 清空寻路情况，并重新开始寻路
        Init();
        runTimer.start();
    });

    randMapCheckBox.setParent(this);
    randMapCheckBox.setText("是否启用生成随机地图");
    randMapCheckBox.move((COL + 1) * SCALE, 150);

    runTimer.setInterval(10);
    //runTimer.start();
    connect(&runTimer,&QTimer::timeout,[=](){
        Search();
    });
    updateTimer.setInterval(50);
    updateTimer.start();
    connect(&updateTimer,&QTimer::timeout,[=](){
        update();
    });
}

void Widget::Init()
{
    // 加入选项，可以选择不生成随机地图
    if (randMapCheckBox.isChecked()) {
        startPos = QPoint(rand() % COL,rand() % ROW);
        endPos = QPoint(rand() % COL,rand() % ROW);

        for (int i = 0; i < COL * ROW; i++) {
            if ( rand() % 3 == 0
                 && (i / COL != startPos.x() || i % COL != startPos.y())
                 && (i / COL != endPos.x() || i % COL != endPos.y())) {
                snakeOccupy[i / COL][i % COL] = 1;
            } else {
                snakeOccupy[i / COL][i % COL] = 0;

            }
        }
    }
    qDebug() << "现在清空所有数据，重新开始寻路。";
    // 传入当前地图，并传入起点与终点
    AStarLoadMap((char *)snakeOccupy,ROW,COL);
    AStarInit(startPos,endPos);
}


void Widget::Search(void)
{
    char result = 0;
    result = AStarSearch();
    if (result == 0
            || result == 2) {
        runTimer.stop();
        qDebug() << "寻路结束。";
    }
}

Widget::~Widget()
{
    delete ui;
}

QPoint Change2Paint(QPoint srcPoint)
{
    QPoint dstPos;
    dstPos.setX(srcPoint.x());
    dstPos.setY(srcPoint.y());
    dstPos = dstPos * SCALE;
    return  dstPos;
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    // 需要将鼠标点击坐标转换为二维数组坐标
    mousePos = e->pos();
    // qpoint类的除号重载，使用了浮点运算，且四舍五入
    mousePos.rx() /= SCALE;
    mousePos.ry() /= SCALE;
    // 数组采用yx坐标系
    // y在数组的前面一个坐标，代表第多少行，放置鼠标导致越界
    if (0 <= mousePos.y() && mousePos.y() < ROW
            && 0 <= mousePos.x() && mousePos.x() < COL)
    snakeOccupy[mousePos.y()][mousePos.x()]
            = ! snakeOccupy[mousePos.y()][mousePos.x()];
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawLine(QPoint(0, ROW) * SCALE,QPoint(COL, ROW) * SCALE);
    painter.drawLine(QPoint(COL, 0) * SCALE,QPoint(COL, ROW) * SCALE);
    painter.setPen(qRgb(255,255,255));
    // 黄色小格，代表探索过
    painter.setBrush(QBrush(qRgb(255,255,0)));
    for (int y = 0; y < ASTAR_HEIGHT; y++) {
        for (int x = 0; x < ASTAR_WIDTH; x++) {
            if (walkMark[y * ASTAR_WIDTH + x]) {
                QPoint pos(x, y);
                pos = Change2Paint(pos);
                painter.drawRect(QRect(pos,pos + CELL_SIZE));
            }
        }
    }

    // 绿色为路径
    QPoint pos;
    QPoint* proute = route;
    painter.setBrush(QBrush(qRgb(0,255,0)));
    while (QPoint(-1,-1) != *proute) {
        pos = Change2Paint(*proute);
        painter.drawRect(QRect(pos,pos + CELL_SIZE));
        ++proute;
    }
    // 红色为起点，蓝色为终点
    painter.setBrush(QBrush(qRgb(255,0,0)));
    pos = Change2Paint(startPos);
    painter.drawRect(QRect(pos,pos + CELL_SIZE));
    painter.setBrush(QBrush(qRgb(0,0,255)));
    pos = Change2Paint(endPos);
    painter.drawRect(QRect(pos,pos + CELL_SIZE));

    painter.setBrush(QBrush(qRgb(0,0,0)));
    // 黑色为墙，白色为空地
    for (int y = 0; y < ROW; y++) {
        for (int x = 0; x < COL; x++) {
            if (snakeOccupy[y][x]) {
                QPoint pos(x,y);
                pos = Change2Paint(pos);
                painter.drawRect(QRect(pos,pos + CELL_SIZE));
            }
        }
    }

}

