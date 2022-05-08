#include "astar.h"

// 地图及其长和宽，外部传入，确定矩形边框
char *wallMap = NULL;
int mapWidth = 0;
int mapHeight = 0;

char astarStatus = 0;
QPoint startPos = QPoint(3,7);
QPoint endPos = QPoint(5,13);
QPoint popPos = QPoint(-1,-1);
QPoint* pnowPos = NULL;
Cell_t* prootCell = NULL;
Cell_t* pnowCell = NULL;
Cell_t* pbackCell = NULL;
// 辅助地图，用于记录是否走过
QPoint route[ASTAR_WIDTH * ASTAR_HEIGHT] = {QPoint(0,0)};
char walkMark[ASTAR_WIDTH * ASTAR_HEIGHT] = {0};
Cell_Struct *cellCost[ASTAR_WIDTH * ASTAR_HEIGHT] = {0};
int cellCostCnt = 0;
QPoint walkDir[4] = {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT};

void AStarLoadMap(char* map, int w,int h)
{
    wallMap = map;
    mapWidth = w;
    mapHeight = h;
}
void AStarFree(Cell_t* prootCell)
{
    for (int i = 0; i < 4; ++i) {
        if (NULL != prootCell->Childs[i]) {
            AStarFree(prootCell->Childs[i]);
        }
    }
    delete prootCell;
}

void AStarInit(QPoint _startPos,QPoint _endPos)
{
//    qDebug() << "现在清空所有数据，重新开始寻路。";
//    // 先判断，判断是否存在历史残留的路径，需要释放内存
    if (NULL != prootCell)
    {
        AStarFree(prootCell);
    }
    pnowPos = route;
    for (int i = 0; i< ASTAR_WIDTH * ASTAR_HEIGHT; ++i) {
        route[i] = QPoint(-1,-1);
        cellCost[i] = NULL;
        walkMark[i] = 0;
    }
    pbackCell = NULL;
    cellCostCnt = 0;

    // 初始化起点与终点
    startPos = _startPos;
    endPos = _endPos;
    prootCell = (Cell_t*)malloc(sizeof(Cell_t));
    memset(prootCell,0,sizeof(Cell_t));
    prootCell->pos = startPos;
    prootCell->gCost = 0 * CELL_COST;
    prootCell->hCost = (endPos - prootCell->pos).manhattanLength() * CELL_COST;
    // 将根节点（起点）塞入可寻址buffer内，然后开始寻路
    PushCostCell(prootCell);
}

char AStarSave(void)
{
    if (NULL == pbackCell) {
        pbackCell = pnowCell;
    }
    //while (pbackCell->pos != startPos) {
    if (pbackCell->pos != startPos) {
        *pnowPos = pbackCell->pos;
        ++pnowPos;
        pbackCell = pbackCell->parent;
        astarStatus = ASTAR_GOBACK;
        return astarStatus;
    }
    pbackCell = NULL;
    astarStatus = ASTAR_SUCCEED;
    return astarStatus;
}

void PushCostCell(Cell_t* pCell)
{
    int existCellCnt = 0;
    int emptyCellIdx = -1;
    // 找到空元素就记录，找到自身，直接覆盖载入
    for (int cellIdx = 0; cellIdx < mapWidth * mapHeight && existCellCnt <= cellCostCnt; ++cellIdx) {
        // 找到一个可以插入地方，就直接结束查找
        // 不进行插入，先记录下来，需要查找是否存在自身
        if (NULL == cellCost[cellIdx]) {
            if (-1 == emptyCellIdx) {
                emptyCellIdx = cellIdx;
            }
        }
        if(NULL != cellCost[cellIdx]) {
            ++existCellCnt;
            // 如果存在自身，记录，结束循环，再塞入
            if (pCell->pos == cellCost[cellIdx]->pos) {
                emptyCellIdx = cellIdx;
                break;
            }
        }
    }
    // 只有插入到了不存在的地方，才认为有效个数增加
    if (NULL == cellCost[emptyCellIdx]) {
        ++cellCostCnt;
        cellCost[emptyCellIdx] = pCell;
    }
    // 只有当新节点代价更小时，才执行塞入操作,这样子的路径才是最优路径
    if (NULL != cellCost[emptyCellIdx]
             && pCell->gCost + pCell->hCost
             < cellCost[emptyCellIdx]->gCost + cellCost[emptyCellIdx]->hCost)
    {
        cellCost[emptyCellIdx] = pCell;
    }
    qDebug() << "入buff[" << emptyCellIdx << "]："
             << cellCost[emptyCellIdx]->pos << "，及其代价："
             << cellCost[emptyCellIdx]->gCost << "," << cellCost[emptyCellIdx]->hCost ;
}

Cell_t* PopMinCostCell()
{
    int existCellCnt = 0;
    // 从内存内寻找最小代价的cell
    Cell_t* pminCostCell = NULL;
    int minCellIdx = -1;
    for (int cellIdx = 0; cellIdx < mapWidth * mapHeight && existCellCnt <= cellCostCnt; ++cellIdx)
    {
        // 如果为空，跳过
        if (NULL != cellCost[cellIdx]) {
            // 有效的值增加，如果超过已经存在的值的个数，就不用再冗余比较后面的值了
            ++existCellCnt;

            // 确定第一个有效值为初始值，用于比较并存储最小值,
            // 找到之后跳过此轮，此条件只会成立一次
            if (-1 == minCellIdx) {
                minCellIdx = cellIdx;
                continue ;
            }
            // 寻找代价最小的点
            if (cellCost[cellIdx]->gCost + cellCost[cellIdx]->hCost
                    <= cellCost[minCellIdx]->gCost + cellCost[minCellIdx]->hCost ) {
                    minCellIdx = cellIdx;
            }
        }

    }
    // 记录，并且从buffer内删除，然后返回最小的元素
    if (-1 != minCellIdx) {
        pminCostCell = cellCost[minCellIdx];
        qDebug() << "出buff[" << minCellIdx << "]：" << pminCostCell->pos
                 << "，及其代价：" << pminCostCell->gCost << "," << pminCostCell->hCost ;
        --cellCostCnt;
        cellCost[minCellIdx] = NULL;
    }
    return  pminCostCell;
}

char AStarSearch()
{
    //while (0)
    //while (pnowCell->pos != endPos)
    {
        // 寻找最小代价节点进行弹出，如果找不到，说明寻路结束，且失败
        if (astarStatus != ASTAR_GOBACK) {
            pnowCell = PopMinCostCell();
        }
        if (NULL == pnowCell) {
            //AStarInit();
            return ASTAR_FAIL;
        }
        // 解决起点与终点重合时，说明寻路结束，且成功
        if (endPos == pnowCell->pos) {
            return AStarSave();
            //AStarInit();
            return ASTAR_SUCCEED;
        }
        // 将路径记录下来
        popPos = pnowCell->pos;
        walkMark[pnowCell->pos.y() * ASTAR_WIDTH + pnowCell->pos.x()] = WALKED;

        for (int dirIdex = 0; dirIdex < 4; ++dirIdex) {
            int childX = (pnowCell->pos + walkDir[dirIdex]).x();
            int childY = (pnowCell->pos + walkDir[dirIdex]).y();

            // 如果即将访问的节点超出范围 或 已探索 或 是墙壁，那么跳过
            if (childX < 0 || mapWidth <= childX || childY < 0 || mapHeight <= childY
                    || WALKED == walkMark[childY * ASTAR_WIDTH + childX]
                    || wallMap[childY * mapWidth + childX]) {
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
            // 将节点装入buffer
            PushCostCell(pchildCell);
        }
    }
    return ASTAR_SEARCH;
}
