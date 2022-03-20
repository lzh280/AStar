#include "widget.h"
#include "ui_widget.h"

// 辅助地图，用于记录是否走过
char walkMark[ROW][COL] = {0};
QPoint walkDir[4] = {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT};
char snakeOccupy[ROW][COL] = {
    {1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1},
    {1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,1},
    {1,0,0,0,0, 0,0,0,0,0, 1,0,0,0,0, 0,0,0,0,1},
    {1,0,0,0,0, 0, 0,0,1,1,1,0,0,0,0, 0,0,0,0,1},
    {1,0,0,0,0, 0,0,0,0,0, 1,0,0,0,0, 0,0,0,0,1},

    {1,0,0,0,0, 0,0,0,0,0, 1,0,0, 0,0,0,0,0,0,1},
    {1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,1},
    {1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,1},
    {1,0,0,0,0, 0,0,0,0,0, 1,0,0,0,0, 0,0,0,0,1},
    {1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1},
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    startPos = QPoint(3,7);
    endPos = QPoint(5,13);

    Cell_t* pnewCell = (Cell_t*)malloc(sizeof(Cell_t));
    memset(pnewCell,0,sizeof(Cell_t));
    Cell_t* prootCell = pnewCell;
    Cell_t* pparentCell = prootCell;
    pnewCell->pos = startPos;
    pnewCell->gCost = 0 * CELL_COST;
    pnewCell->hCost = (endPos - pnewCell->pos).manhattanLength() * CELL_COST;

    //while (0)
    while (pparentCell->pos != endPos)
    {
        walkMark[pparentCell->pos.x()][pparentCell->pos.y()] = WALKED;
        // 计算4个方向的代价，并使用最小代价进行行走，默认最大代价为当前代价的10倍
        int minCost = (pnewCell->gCost + pnewCell->hCost) * 10;
        int minCostDirIdx = 0;
        for (int dirIdex = 0; dirIdex < 4; ++dirIdex) {
            // 计算坐标，判断是否已经被探索过，如果探索过，就不再探索
            if (WALKED == walkMark[(pparentCell->pos + walkDir[dirIdex]).x()]
                    [(pparentCell->pos + walkDir[dirIdex]).y()])
            {
                continue ;
            }
            // 如果是障碍，那么就跳过
            if (snakeOccupy[(pparentCell->pos + walkDir[dirIdex]).x()]
                    [(pparentCell->pos + walkDir[dirIdex]).y()])
            {
                continue ;
            }
            // 先申请节点，，，，等找到最终路径，再一次性全部释放
            pnewCell = (Cell_t*)malloc(sizeof(Cell_t));
            memset(pnewCell,0,sizeof(Cell_t));
            // 建立指向父节点，建立父节点指向本节点，进行记录，便于以后释放
            pnewCell->parent = pparentCell;
            pparentCell->Childs[dirIdex] = pnewCell;
            // 计算坐标，计算代价，单位距离的代价是CELL_COST
            pnewCell->pos = pparentCell->pos + walkDir[dirIdex];
            pnewCell->gCost = pparentCell->gCost + CELL_COST;
            pnewCell->hCost = (endPos - pnewCell->pos).manhattanLength() * CELL_COST;
            if (pnewCell->gCost + pnewCell->hCost < minCost) {
                minCost = pnewCell->gCost + pnewCell->hCost;
                minCostDirIdx = dirIdex;
            }
            qDebug() << "可选坐标：" << pnewCell->pos << "，及其代价：" << pnewCell->gCost << "," << pnewCell->hCost ;
        }
        // 使用当前最小代价节点，当作父节点，，再次计算,
        pparentCell = pparentCell->Childs[minCostDirIdx];
        qDebug() << "所选方向" << walkDir[minCostDirIdx] << ",所到达的坐标" << pparentCell->pos;
    }
    // 找到节点了，将路径输出，检查是否正确
    QPoint* proute = route;
    while (pparentCell->parent != NULL) {
        *proute = pparentCell->pos;
        pparentCell = pparentCell->parent;
        ++proute;
    }
}

Widget::~Widget()
{
    delete ui;
}

QPoint Change2Paint(QPoint srcPoint)
{
    QPoint dstPos;
    dstPos.setX(srcPoint.y());
    dstPos.setY(srcPoint.x());
    dstPos = dstPos * 20 + QPoint(10,10);
    return  dstPos;
}


void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            QPoint pos(row,col);
            pos = Change2Paint(pos);
            painter.drawText(pos,
                             QString::number(snakeOccupy[row][col]));
        }
    }

    // 突出颜色显示起点与终点
    painter.setPen(qRgb(255,00,0));
    QPoint posPoint = Change2Paint(startPos);
    painter.drawText(posPoint,"+");
    posPoint = Change2Paint(endPos);
    painter.drawText(posPoint,"=");
    QPoint* proute = route;
    while (QPoint(0,0) != *proute) {
        posPoint = Change2Paint(*proute);
        painter.drawText(posPoint,"8");
        ++proute;
    }

}

