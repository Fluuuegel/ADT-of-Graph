#ifndef _NETWORK_CPP_
#define _NETWORK_CPP_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXV 100
#define INFINITE 10000
typedef enum{DG,DN,UDG,UDN} Graphkind;

//边结构体
typedef struct ArcNode{
    int fromvex;    //该弧指出顶点的位置
    int adjvex; //该弧所指向的顶点的位置
    struct ArcNode *nextarc;
    int weight; //边权值
} Arc;

//顶点结构体
typedef struct{
    int weight; //顶点权值
    int degree; //顶点出度
    Arc *firstarc;
} VNode, AdjList[MAXV];  //通过数组直接确定顶点编号

//图结构体（邻接表 + 邻接矩阵）
typedef struct{
    AdjList vertices;
    int vexnum,arcnum;
    int kind;
    int directional;    //是否有向
    int weighted;   //是否有权
    int E[MAXV][MAXV];  //边数组 索引
} Graph;

//队列（用于BFS）
typedef struct QNode{
    int data;
    struct QNode *next;
}QNode;

typedef struct Queue{
    QNode* front;
    QNode* rear;
}Queue;

Queue *InitQueue(Queue* q){
    q->front = q->rear = (QNode *)malloc(sizeof(QNode));
    return q;
}

void EnQueue(Queue *q, int e){
    QNode * p = (QNode *)malloc(sizeof(QNode));
    p->data = e;
    p->next = NULL;
    q->rear->next = p;
    q->rear = p;
}

void DeQueue(Queue *q, int *e){
    if(q->front == q->rear) return;
    QNode *p = q->front->next;
    *e = p->data;
    q->front->next = p->next;
    if(p == q->rear) q->rear = q->front;
    free(p);
}

int QueueEmpty(Queue q){
    if(q.front == q.rear) return 1;
    return 0;
}

//建立空图
Graph *CreateGraph(Graph * g){
    printf("Please enter the kind of this graph: \n0. Directional Graph\n1. Directional Net\n2. Undirectional Graph\n3. Undirectional Net\n");
    scanf("%d", &g->kind);
    if(g->kind == 0){
        g->directional = 1;
        g->weighted = 0;
    }
    else if(g->kind == 1){
        g->directional = 1;
        g->weighted = 0;
    }
    else if(g->kind == 2){
        g->directional = 0;
        g->weighted = 0;
    }else{
        g->directional = 0;
        g->weighted = 1;
    }
    g->vexnum = 0;
    g->arcnum = 0;
    for(int i = 0; i < MAXV; i++)
        for(int j = 0; j < MAXV; j++)
            g->E[i][j] = INFINITE;      //E[i][j] 表示从i到j的有向边 或无向边
    return g;
}

#include "showgt.h"

//通过度数寻顶点
int LocateVexByD(Graph *g, int d){  
    for(int i = 0; i <= g->vexnum ; i++){
        if(g->vertices[i].degree == d)
            return i;
    }
    printf("Cant find!\n");
    return -1;
}

//通过权值寻顶点
int LocateVexByW(Graph *g, int w){  
    for(int i = 0; i <= g->vexnum; i++){
        if(g->vertices[i].degree == w)
            return i;
    }
    printf("Cant find!\n");
    return -1;
}

//添加边 初始度数为0 w为顶点权值
int AddVertex(Graph *g, int w){  
    if(g->vexnum == MAXV){
        printf("Too many Vexes!\n");
        return -1;
    }
    g->vertices[g->vexnum].degree = 0;
    g->vertices[g->vexnum].weight = w;
    g->vertices[g->vexnum].firstarc = NULL;
    g->vexnum++;
    return g->vexnum - 1;
}

//fv av为出/入顶点的位置 w为边权值（无权图默认为1）
int AddArc(Graph *g, int fv, int av, int w){   
    if(fv >= g->vexnum || av >= g->vexnum || fv < 0 || av < 0 ){
        printf("Wrong vex number!\n");
        return -1;
    }
    ArcNode * p = (ArcNode *)malloc(sizeof(ArcNode));
    ArcNode * q = (ArcNode *)malloc(sizeof(ArcNode));
    p->adjvex = av; p->fromvex = fv; p->weight = w; p->nextarc = NULL;
    q->adjvex = fv; q->fromvex = av; q->weight = w; q->nextarc = NULL;
    if(g->kind == UDG || g->kind == UDN){
        p->nextarc = g->vertices[fv].firstarc;
        g->vertices[fv].firstarc = p;
        g->vertices[fv].degree++;
        q->nextarc = g->vertices[av].firstarc;
        g->vertices[av].firstarc = q;
        g->vertices[av].degree++;
        g->E[fv][av] = g->E[av][fv] = w;
    }else{
        p->nextarc = g->vertices[fv].firstarc;
        g->vertices[fv].firstarc = p;
        g->vertices[fv].degree++;   //出度
        g->E[fv][av] = w;
    }
    g->arcnum++;
    return 1;
}

//删除从fv到av的边
int DeleteArc(Graph *g, int fv,int av){ 
    int flag = 0;
    if(fv >= g->vexnum || av >= g->vexnum || fv < 0 || av < 0 ){
        printf("Wrong vex number!\n");
        return -1;
    }
    Arc *p = g->vertices[fv].firstarc, *pt = g->vertices[fv].firstarc;
    Arc *q = g->vertices[av].firstarc, *qt = g->vertices[av].firstarc;
    while(p){
        if(p->adjvex == av && p == g->vertices[fv].firstarc){
            g->vertices[fv].firstarc = p ->nextarc;
            free(p);
            flag = 1;
            g->E[fv][av] = INFINITE;
            break;
        }else if(p-> adjvex == av){
            pt->nextarc = p->nextarc;
            free(p);
            flag = 1;
            g->E[fv][av] = INFINITE;
            break;
        }
        pt = p;
        p = p->nextarc;
    }
    if(g->kind == UDG || g->kind == UDN){
        while(q){
            if(q->adjvex == fv && q == g->vertices[av].firstarc){
                g->vertices[av].firstarc = q ->nextarc;
                free(q);
                flag = 1;
                g->E[av][fv] = INFINITE;
                break;
            }else if(q-> adjvex == fv){
                qt->nextarc = q->nextarc;
                free(q);
                flag = 1;
                g->E[av][fv] = INFINITE;
                break;
            }
            qt = q;
            q = q->nextarc;
        }
    }
    if(flag){
        printf("\nDelete successfully.\n");
        g->arcnum --;
    }
    return 0;
}

//删除顶点
int DeleteVex(Graph *g, int num){
    if(g->vexnum <= num || num < 0){
        printf("\nWrong vex number!\n");
        return -1;
    }
    
    Arc *a = g->vertices[num].firstarc;
    while(a){
        DeleteArc(g, a->fromvex, a->adjvex);
        a = a->nextarc;
    }   //删除与顶点相关联的边
    
    for(int i = num; i < g->vexnum - 1; i++){
        g->vertices[i].degree = g->vertices[i+1].degree;
        g->vertices[i].weight = g->vertices[i+1].weight;
        g->vertices[i].firstarc = g->vertices[i+1].firstarc;
    }
    g->vexnum --;
    return 1;
}

//打印邻接表
void PrintGraph(Graph *g){
    printf("Info of The Graph: \n    Kind: %d\n    Number of Vertex: %d\n    Number of Arc: %d\n\n",g->kind, g->vexnum, g->arcnum);
    for(int i = 0; i < g->vexnum; i++){
        ArcNode *p = g->vertices[i].firstarc;
        int j = 1;
        printf("Vex %d : \n", i);
        if(p == NULL)
            printf("    No Arc\n");
        while(p){
            printf("    Arc %d : From Vex: %d  To Vex: %d  Weight: %d\n", j, p->fromvex, p->adjvex, p->weight);
            p = p->nextarc;
            j++;
        }
        printf("\n");
    }
}

/*
    复杂操作：Dijkstra算法
    邻接表的特性使得它不易查找边 因此本图的ADT添加了边索引 E[][] 以来完成对两顶点之间边的权值的快速查找 实际上是邻接矩阵
    PrintPath采用回溯法打印最短路径
*/

void PrintPath(Graph *g, int *L, int i, int aim){
    if(i != aim)
        PrintPath(g, L, L[i], aim);
    printf(" %d ", i);
}

int Visited[MAXV] = {0}, TVisited[MAXV] = {0};
int E = 0;

void Dijkstra(Graph *g, int u0){ //Dijkstra算法（求u0到其余所有顶点距离和最短路径）
    int d[g->vexnum], l[g->vexnum], flag[g->vexnum];
    for(int i = 0; i < g->vexnum; i++){
        if(i == u0){
            d[i] = 0;
            l[i] = u0;
            flag[i] = 1;
        }
        else{
            d[i] = INFINITE;
            l[i] = -1;
            flag[i] = 0;    //标志变量，相当于书中的S0
        }
    }   //初始化

    int ut = u0, dmin = INFINITE;
    for(int count = 0; count < g->vexnum; count++){ //d数组保存路径 l数组保存最短路径经过的上一个节点
        for(int i = 0; i < g->vexnum; i++){
            if(flag[i] == 1) continue;
            else{
                if(d[ut] + g->E[ut][i] < d[i]){
                    d[i] = d[ut] + g->E[ut][i];
                    l[i] = ut;
                }
            }
        }
        for(int i = 0; i< g->vexnum; i++){
            if(flag[i] == 1) continue;
            else{
                if(d[i] <= dmin){
                    dmin = d[i];
                    ut = i;
                }
            }
        }
        dmin = INFINITE;
        flag[ut] = 1;
    }

    for(int i = 0; i < g->vexnum; i++){
        printf("Distance to Vertex %d : %d  PreVertex: %d  Path: ",i, d[i], l[i]);
        PrintPath(g, l, i, u0);
        printf("\n");
    }
}

void ExistPathByLength(Graph *g, int u, int v, int length){ //判别无向图中u v顶点之间是否存在一条长为length的简单路径的算法 (有边权图或无边权图均可)
    Visited[u] = 1;
    if(u == v && length == 0){
        E = 1; return ;
    }
    ArcNode *p = g->vertices[u].firstarc;
    while (p != NULL && length > 0){
        int next = p->adjvex;
        if(Visited[next] == 0)
            ExistPathByLength(g, next, v, length - p->weight);
        p = p->nextarc;
    }
    Visited[u] = 0;    //若当前不存在从u到v且长度为length的路径 那么这个点还是可以使用的
    return ;
}

//深度优先遍历
void (* VisitFunc)(Graph *g, int v);
void Visit_Print(Graph *g, int v){
    printf("    V: %d Degree: %d Weight: %d \n",v, g->vertices[v].degree, g->vertices[v].weight);
}

void DFS(Graph *g, int v){
    TVisited[v] = 1;
    VisitFunc(g, v);
    for(ArcNode *a = g->vertices[v].firstarc; a != NULL; a = a->nextarc){
        if(!TVisited[a->adjvex]) DFS(g, a->adjvex);
    }
}

int DFSTraverse(Graph *g, void (* Visit)(Graph *g, int v)){
    VisitFunc = Visit;
    int N_CP = 0;   //连通片数量
    for(int i = 0; i < MAXV; i++)
        TVisited[i] = 0;    //  初始化
    for(int i = 0; i < g->vexnum; i++){
        if(!TVisited[i]){
            DFS(g, i);
            N_CP++;
        }
    }
    return N_CP;
}

//广度优先遍历
void BFSTraverse(Graph *g, void (* Visit)(Graph *g, int v)){
    for(int i = 0; i < MAXV; i++)
        TVisited[i] = 0;
    Queue q;
    Queue *qptr = &q;
    int u;
    qptr = InitQueue(qptr);
    for(int i = 0; i < g->vexnum; i++){
        if(!TVisited[i]){
            Visit(g, i);
            TVisited[i] = 1;
            EnQueue(qptr, i);

            while(!QueueEmpty(q)){
                DeQueue(qptr, &u);  //出队列
                //遍访u的所有邻接节点
                for(ArcNode *a = g->vertices[u].firstarc; a != NULL; a = a->nextarc){
                    if(!TVisited[a->adjvex]){
                        Visit(g, a->adjvex);
                        TVisited[a->adjvex] = 1;
                        EnQueue(qptr, a->adjvex);//入队列
                    }
                }
            }
        }
    }
}

//7.22
int DFS_7_22(Graph *g, int v, int aim_v){
    if(v == aim_v) return 1;
    TVisited[v] = 1;
    for(ArcNode *a = g->vertices[v].firstarc; a!= NULL; a = a->nextarc){
        if(!TVisited[a->adjvex]) return DFS_7_22(g, a->adjvex, aim_v);
    }
    return 0;
}

int DFS_7_22_T(Graph *g, int v, int aim_v){
    for(int i = 0; i < MAXV; i++){
        TVisited[i] = 0;
    }
    return DFS_7_22(g, v, aim_v);
}

int main(){
    Graph G;
    Graph *g = &G;
    g = CreateGraph(g); //只需输入图的类型
    int N_CP;

    for(int i = 0; i <= 7; i++)
        AddVertex(g, 1);    //新建8个顶点

    AddArc(g, 0, 1, 7);
    AddArc(g, 0, 4, 8);
    AddArc(g, 0, 6, 2);
    AddArc(g, 0, 7, 4);
    AddArc(g, 1, 2, 2);
    AddArc(g, 1, 3, 3);
    AddArc(g, 1, 4, 1);
    AddArc(g, 2, 3, 1);
    AddArc(g, 2, 4, 2);
    AddArc(g, 2, 5, 5);
    AddArc(g, 3, 5, 3);
    AddArc(g, 3, 7, 6);
    AddArc(g, 4, 5, 7);
    AddArc(g, 4, 6, 4);
    AddArc(g, 5, 6, 3);
    AddArc(g, 5, 7, 4);
    AddArc(g, 6, 7, 6); //新增边 《图论》 P26 图1.17

    AddVertex(g, 1);
    AddVertex(g, 1);
    AddArc(g, 8, 9, 2);
    
    /*
    for(int i = 0; i <= 5; i++)
        AddVertex(g, 1);    //新建6个顶点

    AddArc(g, 0, 1, 1);
    AddArc(g, 1, 2, 1);
    AddArc(g, 2, 3, 1);
    AddArc(g, 3, 4, 1);
    AddArc(g, 4, 0, 1);
    */

    ExistPathByLength(g, 1, 5, 8);
    printf("\nDFS: \n");
    N_CP = DFSTraverse(g, Visit_Print);
    printf("\nBFS: \n");
    BFSTraverse(g, Visit_Print);
    printf("\nPath weighted 8 from 1 to 5: %d\nNumber of connected part(s): %d\n\n", E, N_CP);
    PrintGraph(g);

    saveGraph(g,"sg1.html");

    int Re1 = DFS_7_22_T(g, 1, 4);
    int Re2 = DFS_7_22_T(g, 1, 8);

    printf("PathExist? :\n 1 to 4: %d\n 1 to 8: %d\n", Re1, Re2);
    /*
    Dijkstra(g, 4);
    DeleteArc(g, 4, 0);
    PrintGraph(g);
    DeleteVex(g, 4);
    PrintGraph(g);
    */

    return 0;
}

#endif