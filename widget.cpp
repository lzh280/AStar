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
    startPos = QPoint(3,7);
    endPos = QPoint(5,13);

    Cell_t* prootCell = (Cell_t*)malloc(sizeof(Cell_t));
    memset(prootCell,0,sizeof(Cell_t));
    prootCell->pos = startPos;
    prootCell->gCost = 0 * CELL_COST;
    prootCell->hCost = (endPos - prootCell->pos).manhattanLength() * CELL_COST;

    // 父节点，默认为起点
    Cell_t* pnowCell = prootCell;
    //while (0)
    while (pnowCell->pos != endPos)
    {
        walkMark[pnowCell->pos.x()][pnowCell->pos.y()] = WALKED;
        // 计算4个方向的代价，并使用最小代价进行行走，默认最小代价为当前代价的10倍
        int minCost = (pnowCell->gCost + pnowCell->hCost) * 10;
        int minCostDirIdx = -1;
        for (int dirIdex = 0; dirIdex < 4; ++dirIdex) {
            // 计算坐标，判断是否已经被探索过，如果探索过，就不再探索
            if (WALKED == walkMark[(pnowCell->pos + walkDir[dirIdex]).x()]
                    [(pnowCell->pos + walkDir[dirIdex]).y()])
            {
                continue ;
            }
            // 如果是障碍，那么就跳过
            if (snakeOccupy[(pnowCell->pos + walkDir[dirIdex]).x()]
                    [(pnowCell->pos + walkDir[dirIdex]).y()])
            {
                continue ;
            }
            // 先申请节点，，，，等找到最终路径，再一次性全部释放
            Cell_t* pchildCell = (Cell_t*)malloc(sizeof(Cell_t));
            memset(pchildCell,0,sizeof(Cell_t));
            // 建立指向父节点，建立父节点指向本节点，进行记录，便于以后释放
            pchildCell->parent = pnowCell;
            pnowCell->Childs[dirIdex] = pchildCell;
            // 计算坐标，计算代价，单位距离的代价是CELL_COST
            pchildCell->pos = pnowCell->pos + walkDir[dirIdex];
            pchildCell->gCost = pnowCell->gCost + CELL_COST;
            pchildCell->hCost = (endPos - pchildCell->pos).manhattanLength() * CELL_COST;
            if (pchildCell->gCost + pchildCell->hCost < minCost) {
                minCost = pchildCell->gCost + pchildCell->hCost;
                minCostDirIdx = dirIdex;
            }
            qDebug() << "可选坐标：" << pchildCell->pos << "，及其代价：" << pchildCell->gCost << "," << pchildCell->hCost ;
        }
        // 存在走入死胡同，那么删除其父节点对本节点的链接，
        // 恢复父节点的路过状态，，，并将当前坐标忽略为墙体
        // 试试忽略成已走过，，因为不想改变真实地图
        //
        if (NULL == pnowCell->Childs[minCostDirIdx]
                || -1 == minCostDirIdx)
        {
            qDebug() << "死胡同，退出";
            walkMark[pnowCell->pos.x()][pnowCell->pos.y()] = 1;// 忽略为已经走过，返回
            pnowCell = pnowCell->parent;
            continue;
        }
        // 如果发现有更低代价的路径，就使用更低代价的路径
        // 当前子节点的代价，并非所有子节点中最低的代价，
        // 那么需要重新遍历所有子节点，然后寻找最低代价？
        {


        }
        qDebug() << "所选方向" << walkDir[minCostDirIdx] << ",所到达的坐标" << pnowCell->Childs[minCostDirIdx]->pos;
        // 使用当前最小代价节点，当作当前节点，，再次计算,
        pnowCell = pnowCell->Childs[minCostDirIdx];
    }
    // 找到节点了，将路径输出，检查是否正确
    QPoint* proute = route;
    while (pnowCell->parent != NULL) {
        *proute = pnowCell->pos;
        pnowCell = pnowCell->parent;
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
    dstPos = dstPos * 20;
    return  dstPos;
}


void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(qRgb(255,255,255));
    // 黑色为墙，白色为空地
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            QPoint pos(row,col);
            snakeOccupy[row][col] ?
                        painter.setBrush(QBrush(qRgb(0,0,0)))
                      :  painter.setBrush(QBrush(qRgb(255,255,255)));
            pos = Change2Paint(pos);
            painter.drawRect(QRect(pos,pos + QPoint(20,20)));
        }
    }

    // 蓝色为路径
    QPoint pos;
    QPoint* proute = route;
    painter.setBrush(QBrush(qRgb(0,255,0)));
    while (QPoint(0,0) != *proute) {
        pos = Change2Paint(*proute);
        painter.drawRect(QRect(pos,pos + QPoint(20,20)));
        ++proute;
    }
    // 红色为起点，绿色为终点
    painter.setBrush(QBrush(qRgb(255,0,0)));
    pos = Change2Paint(startPos);
    painter.drawRect(QRect(pos,pos + QPoint(20,20)));
    painter.setBrush(QBrush(qRgb(0,0,255)));
    pos = Change2Paint(endPos);
    painter.drawRect(QRect(pos,pos + QPoint(20,20)));

}

