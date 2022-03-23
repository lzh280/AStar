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

    prootCell = NULL;
    pnowCell = NULL;
    AStarInit();

    startBtn.setParent(this);
    startBtn.setText("点击重新开始寻路");
    startBtn.move(500,100);
    connect(&startBtn,&QPushButton::clicked,[=](){
        // 清空寻路情况，并重新开始寻路
        AStarInit();
    });
    runTimer.setInterval(100);
    //runTimer.start();
    connect(&runTimer,&QTimer::timeout,[=](){
        AStarSearch();
    });
    updateTimer.setInterval(50);
    updateTimer.start();
    connect(&updateTimer,&QTimer::timeout,[=](){
        update();
    });
}

void Widget::AStarInit()
{
    // 先判断，判断是否存在历史残留的路径，需要释放内存
    if (NULL != pnowCell)
    {
        for (int dirIdex = 0; dirIdex < 4; ++dirIdex) {
            if (NULL != pnowCell->Childs[dirIdex]) {
                free(pnowCell->Childs[dirIdex]);
                //pnowCell->Childs[dirIdex] = NULL;
            }
        }
        // 变成父节点，并且逐层寻找，并
        pnowCell = pnowCell->parent;
    }
    nowPos = route;
    for (int i = 0; i< ROUTE_LEN; ++i) {
        route[i] = QPoint(-1,-1);
    }
    memset(walkMark,0,sizeof(walkMark));
    Cell_t* prootCell = (Cell_t*)malloc(sizeof(Cell_t));
    memset(prootCell,0,sizeof(Cell_t));
    prootCell->pos = startPos;
    prootCell->gCost = 0 * CELL_COST;
    prootCell->hCost = (endPos - prootCell->pos).manhattanLength() * CELL_COST;
    // 父节点，默认为起点
    pnowCell = prootCell;

    runTimer.start();
}


void Widget::AStarSearch(void)
{
    //while (0)
    //while (pnowCell->pos != endPos)
    {
        // 死胡同，回溯到了起点以前，说明并没有可以到达终点路径，直接返回
        if (NULL == pnowCell) {
            runTimer.stop();
            AStarInit();
            return;
        }
        // 解决起点与终点重合时，直接卡死的bug
        if (endPos == pnowCell->pos) {
            runTimer.stop();
            AStarInit();
            return;
        }
        walkMark[pnowCell->pos.x()][pnowCell->pos.y()] = WALKED;
        // 计算4个方向的代价，并使用最小代价进行行走，默认最小代价为当前代价的10倍
        int minCost = (pnowCell->gCost + pnowCell->hCost) * 10;
        int minCostDirIdx = -1;
        for (int dirIdex = 0; dirIdex < 4; ++dirIdex) {
            int childX = (pnowCell->pos + walkDir[dirIdex]).x();
            int childY = (pnowCell->pos + walkDir[dirIdex]).y();

            // 如果即将访问的节点超出范围 或 已探索 或 是墙壁，那么跳过
            if (childX < 0 || ROW <= childX || childY < 0 || COL <= childY
                    || WALKED == walkMark[childX][childY]
                    || snakeOccupy[childX][childY]) {
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
            //qDebug() << "可选坐标：" << pchildCell->pos << "，及其代价：" << pchildCell->gCost << "," << pchildCell->hCost ;
        }
        // 存在走入死胡同，那么删除其父节点对本节点的链接，
        // 恢复父节点的路过状态，，，并将当前坐标忽略为墙体
        // 试试忽略成已走过，，因为不想改变真实地图
        //
        if (NULL == pnowCell->Childs[minCostDirIdx]
                || -1 == minCostDirIdx)
        {
            //qDebug() << "死胡同，退出";
            *nowPos = QPoint(-1,-1);
            --nowPos;
            walkMark[pnowCell->pos.x()][pnowCell->pos.y()] = 1;// 忽略为已经走过，返回
            // 认定当前节点无效，临时存储一下，然后删除
            Cell_t* tempCell = pnowCell;
            pnowCell = pnowCell->parent;
            free(tempCell);
            return;//continue;
        }
        // 如果发现有更低代价的路径，就使用更低代价的路径
        // 当前子节点的代价，并非所有子节点中最低的代价，
        // 那么需要重新遍历所有子节点，然后寻找最低代价？
        {


        }
        *nowPos = pnowCell->Childs[minCostDirIdx]->pos;
        nowPos++;
        //qDebug() << "所选方向" << walkDir[minCostDirIdx] << ",所到达的坐标" << pnowCell->Childs[minCostDirIdx]->pos;
        // 使用当前最小代价节点，当作当前节点，，再次计算,
        pnowCell = pnowCell->Childs[minCostDirIdx];
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
    // 在坐标系中的xy与数组中xy是相反的，切记。
    snakeOccupy[mousePos.y()][mousePos.x()]
            = ! snakeOccupy[mousePos.y()][mousePos.x()];
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(qRgb(255,255,255));
    // 黑色为墙，白色为空地
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            QPoint pos(row,col);
            snakeOccupy[row][col]
                    ? painter.setBrush(QBrush(qRgb(0,0,0)))
                    : painter.setBrush(QBrush(qRgb(255,255,255)));
            pos = Change2Paint(pos);
            painter.drawRect(QRect(pos,pos + QPoint(20,20)));
        }
    }

    // 蓝色为路径
    QPoint pos;
    QPoint* proute = route;
    painter.setBrush(QBrush(qRgb(0,255,0)));
    while (QPoint(-1,-1) != *proute) {
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

    // 左上黄色小格，代表探索过
    painter.setBrush(QBrush(qRgb(255,255,0)));
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            QPoint pos(row,col);
            if (walkMark[row][col]) {
                pos = Change2Paint(pos);
                painter.drawRect(QRect(pos,pos + QPoint(10,10)));
            }
        }
    }

}

