#include "astar.h"

// 地图及其长和宽，外部传入，确定矩形边框
char *wallMap = NULL;
int mapWidth = 0;
int mapHeight = 0;

QPoint startPos = QPoint(3,7);
QPoint endPos = QPoint(5,13);
QPoint *pnowPos = NULL;
Cell_t* prootCell = NULL;
Cell_t* pnowCell = NULL;
// 辅助地图，用于记录是否走过
QPoint route[ROW * COL] = {QPoint(0,0)};
char walkMark[ROW * COL] = {0};
Cell_Struct *cellCost[ROW * COL] = {0};
QPoint walkDir[4] = {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT};

void AStarLoadMap(char* map, int w,int h)
{
    wallMap = map;
    mapWidth = w;
    mapHeight = h;
}

void AStarInit(QPoint _startPos,QPoint _endPos)
{
//    qDebug() << "现在清空所有数据，重新开始寻路。";
//    // 先判断，判断是否存在历史残留的路径，需要释放内存
//    if (NULL != pnowCell)
//    {
//        for (int dirIdex = 0; dirIdex < 4; ++dirIdex) {
//            if (NULL != pnowCell->Childs[dirIdex]) {
//                free(pnowCell->Childs[dirIdex]);
//                //pnowCell->Childs[dirIdex] = NULL;
//            }
//        }
//        // 变成父节点，并且逐层寻找，并
//        pnowCell = pnowCell->parent;
//    }
    pnowPos = route;
    for (int i = 0; i< ROW * COL; ++i) {
        route[i] = QPoint(-1,-1);
    }
    memset(cellCost,0,sizeof(cellCost));
    memset(walkMark,0,sizeof(walkMark));

    // 初始化起点与终点
    startPos = _startPos;
    endPos = _endPos;
    Cell_t* prootCell = (Cell_t*)malloc(sizeof(Cell_t));
    memset(prootCell,0,sizeof(Cell_t));
    prootCell->pos = startPos;
    prootCell->gCost = 0 * CELL_COST;
    prootCell->hCost = (endPos - prootCell->pos).manhattanLength() * CELL_COST;
    // 将根节点（起点）塞入可寻址buffer内，然后开始寻路
    PushCostCell(prootCell);
}

void AStarSave(void)
{
    pnowPos = route;
    Cell_t *pbackCell = pnowCell;
    while (pbackCell->pos != startPos) {
        *pnowPos = pbackCell->pos;
        ++pnowPos;
        pbackCell = pbackCell->parent;
    }
}

void PushCostCell(Cell_t* pCell)
{
    int emptyCellIdx = 0;
    // 找到空元素，直接载入
    for (int cellIdx = 0; cellIdx < ROW * COL; ++cellIdx) {
        if (NULL == cellCost[cellIdx]) {
            emptyCellIdx = cellIdx;
            break;
        }
    }
    cellCost[emptyCellIdx] = pCell;
    qDebug() << "入buff[" << emptyCellIdx << "]："
             << cellCost[emptyCellIdx]->pos << "，及其代价："
             << cellCost[emptyCellIdx]->gCost << "," << cellCost[emptyCellIdx]->hCost ;
}

Cell_t* PopMinCostCell()
{
    // 从内存内寻找最小代价的cell
    Cell_t* pminCostCell = NULL;
    int minCellIdx = 0;
    for (int cellIdx = 0; cellIdx < ROW * COL; ++cellIdx)
    {
        // 如果为空，跳过
        if (NULL == cellCost[cellIdx]) {
            continue ;
        }
        // 确定第一个有效值为初始值，用于比较并存储最小值
        if (NULL == pminCostCell) {
            minCellIdx = cellIdx;
            pminCostCell = cellCost[minCellIdx];
            continue ;
        }
        if (cellCost[minCellIdx]->gCost + cellCost[minCellIdx]->hCost
              > cellCost[cellIdx]->gCost + cellCost[cellIdx]->hCost)
        {
            minCellIdx = cellIdx;
            pminCostCell = cellCost[minCellIdx];
        }

    }
    // 记录，并且从buffer内删除，然后返回最小的元素
    pminCostCell = cellCost[minCellIdx];
    if (NULL != pminCostCell) {
        qDebug() << "出buff[" << minCellIdx << "]：" << pminCostCell->pos
                 << "，及其代价：" << pminCostCell->gCost << "," << pminCostCell->hCost ;
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
        pnowCell = PopMinCostCell();
        if (NULL == pnowCell) {
            //AStarInit();
            return 2;
        }
        // 解决起点与终点重合时，说明寻路结束，且成功
        if (endPos == pnowCell->pos) {
            AStarSave();
            //AStarInit();
            return 0;
        }
        // 将路径记录下来
        walkMark[pnowCell->pos.y() * COL + pnowCell->pos.x()] = WALKED;

        for (int dirIdex = 0; dirIdex < 4; ++dirIdex) {
            int childX = (pnowCell->pos + walkDir[dirIdex]).x();
            int childY = (pnowCell->pos + walkDir[dirIdex]).y();

            // 如果即将访问的节点超出范围 或 已探索 或 是墙壁，那么跳过
            if (childX < 0 || mapWidth <= childX || childY < 0 || mapHeight <= childY
                    || WALKED == walkMark[childY * COL + childX]
                    || wallMap[childY * COL + childX]) {
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
    return 1;
}
