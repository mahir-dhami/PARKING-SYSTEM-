#include <stdio.h>
#include <stdlib.h>

#define PARKING 20
#define WAITING 20
#define PLACES 6
#define HSIZE 23

int stack[PARKING], tempStack[PARKING], top=-1, tempTop=-1;
int queue[WAITING], front=0, rear=-1, waitingCount=0;

struct HistoryNode { int vehicle; struct HistoryNode *next; };
struct HistoryNode *history=NULL;

struct TreeNode {
    int vehicle, height;
    struct TreeNode *left, *right;
};
struct TreeNode *root=NULL;

const char *place[PLACES]={"Gate","Zone A","Zone B","Zone C","Zone D","Exit"};
int graph[PLACES][PLACES]={
    {0,1,1,0,0,0}, {1,0,0,1,1,0}, {1,0,0,0,1,0},
    {0,1,0,0,0,1}, {0,1,1,0,0,1}, {0,0,0,1,1,0}
};

int readNumber(void) {
    int n, ch;
    while (scanf("%d",&n)!=1) {
        if (feof(stdin)) exit(0);
        while ((ch=getchar())!='\n' && ch!=EOF) {}
        printf("Enter a valid number: ");
    }
    return n;
}

int maxInt(int a,int b) { return a>b?a:b; }
int height(struct TreeNode *n) { return n?n->height:0; }

void updateHeight(struct TreeNode *n) {
    if(n) n->height=1+maxInt(height(n->left),height(n->right));
}

int balanceFactor(struct TreeNode *n) {
    return n?height(n->left)-height(n->right):0;
}

struct TreeNode *newTreeNode(int car) {
    struct TreeNode *n=malloc(sizeof(struct TreeNode));
    if(!n) { printf("Memory allocation failed.\n"); exit(1); }
    n->vehicle=car; n->height=1; n->left=n->right=NULL;
    return n;
}

struct TreeNode *rotateRight(struct TreeNode *y) {
    struct TreeNode *x=y->left, *t=x->right;
    x->right=y; y->left=t;
    updateHeight(y); updateHeight(x);
    return x;
}

struct TreeNode *rotateLeft(struct TreeNode *x) {
    struct TreeNode *y=x->right, *t=y->left;
    y->left=x; x->right=t;
    updateHeight(x); updateHeight(y);
    return y;
}

struct TreeNode *rebalance(struct TreeNode *n) {
    int bf;
    if(!n) return NULL;
    updateHeight(n); bf=balanceFactor(n);
    if(bf>1) {
        if(balanceFactor(n->left)<0) n->left=rotateLeft(n->left);
        return rotateRight(n);
    }
    if(bf< -1) {
        if(balanceFactor(n->right)>0) n->right=rotateRight(n->right);
        return rotateLeft(n);
    }
    return n;
}

struct TreeNode *insertAVL(struct TreeNode *n,int car) {
    if(!n) return newTreeNode(car);
    if(car<n->vehicle) n->left=insertAVL(n->left,car);
    else if(car>n->vehicle) n->right=insertAVL(n->right,car);
    else return n;
    return rebalance(n);
}

struct TreeNode *findMin(struct TreeNode *n) {
    while(n && n->left) n=n->left;
    return n;
}

struct TreeNode *deleteAVL(struct TreeNode *n,int car) {
    struct TreeNode *t;
    if(!n) return NULL;
    if(car<n->vehicle) n->left=deleteAVL(n->left,car);
    else if(car>n->vehicle) n->right=deleteAVL(n->right,car);
    else {
        if(!n->left || !n->right) {
            t=n->left?n->left:n->right;
            free(n); return t;
        }
        t=findMin(n->right); n->vehicle=t->vehicle;
        n->right=deleteAVL(n->right,t->vehicle);
    }
    return rebalance(n);
}

int searchAVL(struct TreeNode *n,int car) {
    if(!n) return 0;
    if(n->vehicle==car) return 1;
    if(car<n->vehicle) return searchAVL(n->left,car);
    return searchAVL(n->right,car);
}

void inorder(struct TreeNode *n) {
    if(!n) return;
    inorder(n->left); printf("%d ",n->vehicle); inorder(n->right);
}

void freeTree(struct TreeNode *n) {
    if(!n) return;
    freeTree(n->left); freeTree(n->right); free(n);
}

int vehicleExists(int car) { return searchAVL(root,car); }

void addToHistory(int car) {
    struct HistoryNode *n=malloc(sizeof(struct HistoryNode));
    if(!n) { printf("Could not save history.\n"); return; }
    n->vehicle=car; n->next=history; history=n;
}

void displayHistory(void) {
    struct HistoryNode *p=history;
    printf("Removed vehicle history: ");
    if(!p) printf("None");
    while(p) { printf("%d ",p->vehicle); p=p->next; }
    printf("\n");
}

void displayVehicles(void) {
    int i;
    printf("Parked vehicles: ");
    if(top<0) printf("None");
    for(i=top;i>=0;i--) printf("%d ",stack[i]);
    printf("\nWaiting vehicles: ");
    if(!waitingCount) printf("None");
    for(i=0;i<waitingCount;i++) printf("%d ",queue[(front+i)%WAITING]);
    printf("\n");
}

void parkVehicle(void) {
    int car;
    printf("Vehicle number: "); car=readNumber();
    if(vehicleExists(car)) { printf("Vehicle already exists.\n"); return; }
    if(top<PARKING-1) {
        stack[++top]=car;
        printf("Vehicle parked.\n");
    } else if(waitingCount<WAITING) {
        rear=(rear+1)%WAITING; queue[rear]=car; waitingCount++;
        printf("Parking full; vehicle added to waiting queue.\n");
    } else { printf("Parking and waiting area are full.\n"); return; }
    root=insertAVL(root,car);
}

void addWaitingVehicle(void) {
    int car;
    printf("Vehicle number: "); car=readNumber();
    if(vehicleExists(car)) { printf("Vehicle already exists.\n"); return; }
    if(waitingCount==WAITING) { printf("Waiting queue is full.\n"); return; }
    rear=(rear+1)%WAITING; queue[rear]=car; waitingCount++;
    root=insertAVL(root,car);
    printf("Vehicle added to waiting queue.\n");
}

void removeVehicle(void) {
    int car,found=0;
    if(top<0) { printf("Parking is empty.\n"); return; }
    printf("Vehicle number to remove: "); car=readNumber();
    while(top>=0) {
        if(stack[top]==car) { top--; found=1; break; }
        tempStack[++tempTop]=stack[top--];
    }
    while(tempTop>=0) stack[++top]=tempStack[tempTop--];
    if(!found) { printf("Vehicle is not in the parking stack.\n"); return; }
    root=deleteAVL(root,car); addToHistory(car);
    printf("Vehicle removed.\n");
    if(waitingCount>0) {
        int next=queue[front];
        stack[++top]=next; front=(front+1)%WAITING; waitingCount--;
        printf("Waiting vehicle %d moved into parking.\n",next);
    }
}

void searchVehicle(void) {
    int car;
    printf("Vehicle number to search: "); car=readNumber();
    if(searchAVL(root,car)) printf("Vehicle found in the system.\n");
    else printf("Vehicle not found.\n");
}

void preorder(struct TreeNode *n) {
    if(!n) return;
    printf("%d ",n->vehicle);
    preorder(n->left);
    preorder(n->right);
}

void postorder(struct TreeNode *n) {
    if(!n) return;
    postorder(n->left);
    postorder(n->right);
    printf("%d ",n->vehicle);
}

void levelOrder(struct TreeNode *n) {
    struct TreeNode *q[PARKING+WAITING+1];
    int f=0,r=0;
    if(!n) { printf("Empty"); return; }
    q[r++]=n;
    while(f<r) {
        struct TreeNode *cur=q[f++];
        printf("%d ",cur->vehicle);
        if(cur->left) q[r++]=cur->left;
        if(cur->right) q[r++]=cur->right;
    }
}

int countNodes(struct TreeNode *n) {
    if(!n) return 0;
    return 1+countNodes(n->left)+countNodes(n->right);
}

int countLeaves(struct TreeNode *n) {
    if(!n) return 0;
    if(!n->left && !n->right) return 1;
    return countLeaves(n->left)+countLeaves(n->right);
}

void treeMenu(void) {
    int ch;
    do {
        printf("\n--- AVL TREE MENU ---\n");
        printf("1. Inorder (sorted)\n2. Preorder\n3. Postorder\n");
        printf("4. Level order\n5. Tree statistics\n0. Back\nChoice: ");
        ch=readNumber();
        switch(ch) {
            case 1: inorder(root); printf("\n"); break;
            case 2: preorder(root); printf("\n"); break;
            case 3: postorder(root); printf("\n"); break;
            case 4: levelOrder(root); printf("\n"); break;
            case 5:
                printf("Nodes: %d | Leaves: %d | Height: %d\n",
                       countNodes(root),countLeaves(root),height(root));
                break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while(ch!=0);
}

void compareSearchMethods(void) {
    int car, arr[PARKING+WAITING], n=0, i, j, temp, found=0, lo, hi, mid;
    printf("Vehicle number to search: "); car=readNumber();
    for(i=top;i>=0;i--) {
        if(stack[i]==car) { found=1; break; }
    }
    if(!found) {
        for(i=0;i<waitingCount;i++)
            if(queue[(front+i)%WAITING]==car) { found=1; break; }
    }
    printf("Linear search: %s\n",found?"found":"not found");
    for(i=top;i>=0;i--) arr[n++]=stack[i];
    for(i=0;i<waitingCount;i++) arr[n++]=queue[(front+i)%WAITING];
    for(i=0;i<n-1;i++)
        for(j=0;j<n-i-1;j++)
            if(arr[j]>arr[j+1]) { temp=arr[j]; arr[j]=arr[j+1]; arr[j+1]=temp; }
    lo=0; hi=n-1; found=0;
    while(lo<=hi) {
        mid=(lo+hi)/2;
        if(arr[mid]==car) { found=1; break; }
        if(car<arr[mid]) hi=mid-1; else lo=mid+1;
    }
    printf("Binary search on sorted data: %s\n",found?"found":"not found");
}

void showGraph(void) {
    int i,j;
    printf("\nParking map adjacency matrix:\n    ");
    for(i=0;i<PLACES;i++) printf("%2d ",i);
    printf("\n");
    for(i=0;i<PLACES;i++) {
        printf("%2d: ",i);
        for(j=0;j<PLACES;j++) printf("%2d ",graph[i][j]);
        printf(" %s\n",place[i]);
    }
    printf("Locations: ");
    for(i=0;i<PLACES;i++) printf("%d=%s ",i,place[i]);
    printf("\n");
}

void bfs(void) {
    int start,i,visited[PLACES]={0},q[PLACES],f=0,r=0,u;
    printf("Start location (0 Gate, 1 Zone A, 2 Zone B, 3 Zone C, 4 Zone D, 5 Exit): ");
    start=readNumber();
    if(start<0 || start>=PLACES) { printf("Invalid location.\n"); return; }
    q[r++]=start; visited[start]=1;
    printf("BFS: ");
    while(f<r) {
        u=q[f++]; printf("%s ",place[u]);
        for(i=0;i<PLACES;i++)
            if(graph[u][i] && !visited[i]) { visited[i]=1; q[r++]=i; }
    }
    printf("\n");
}

void dfsVisit(int u,int visited[]) {
    int i;
    visited[u]=1; printf("%s ",place[u]);
    for(i=0;i<PLACES;i++)
        if(graph[u][i] && !visited[i]) dfsVisit(i,visited);
}

void dfs(void) {
    int start,visited[PLACES]={0};
    printf("Start location (0 Gate, 1 Zone A, 2 Zone B, 3 Zone C, 4 Zone D, 5 Exit): ");
    start=readNumber();
    if(start<0 || start>=PLACES) { printf("Invalid location.\n"); return; }
    printf("DFS: "); dfsVisit(start,visited); printf("\n");
}

int hashFunction(int car) {
    int h=car%HSIZE;
    return h<0?h+HSIZE:h;
}

void hashingDemo(void) {
    int table[HSIZE],i,car,h,found=0;
    for(i=0;i<HSIZE;i++) table[i]=-1;
    for(i=0;i<=top;i++) {
        h=hashFunction(stack[i]);
        while(table[h]!=-1) h=(h+1)%HSIZE;
        table[h]=stack[i];
    }
    for(i=0;i<waitingCount;i++) {
        car=queue[(front+i)%WAITING]; h=hashFunction(car);
        while(table[h]!=-1) h=(h+1)%HSIZE;
        table[h]=car;
    }
    printf("Hash table (linear probing):\n");
    for(i=0;i<HSIZE;i++) if(table[i]!=-1) printf("[%d] %d\n",i,table[i]);
    printf("Vehicle number to search: "); car=readNumber(); h=hashFunction(car);
    for(i=0;i<HSIZE;i++) {
        int idx=(h+i)%HSIZE;
        if(table[idx]==-1) break;
        if(table[idx]==car) { printf("Found at index %d.\n",idx); found=1; break; }
    }
    if(!found) printf("Vehicle not found in hash table.\n");
}

void calculateFee(void) {
    int hours,rate,base,discount,total;
    printf("Hours parked: "); hours=readNumber();
    printf("Rate per hour: "); rate=readNumber();
    printf("Base fee: "); base=readNumber();
    printf("Discount: "); discount=readNumber();
    if(hours<0 || rate<0 || base<0 || discount<0) {
        printf("Values cannot be negative.\n"); return;
    }
    total=base+hours*rate-discount;
    if(total<0) total=0;
    printf("Parking fee = %d\n",total);
}

int main(void) {
    int choice;
    do {
        printf("\n===== PARKING MANAGEMENT SYSTEM =====\n");
        printf("Parked: %d/%d | Waiting: %d/%d\n",top+1,PARKING,waitingCount,WAITING);
        printf("1. Park vehicle\n");
        printf("2. Remove vehicle\n");
        printf("3. Add vehicle to waiting queue\n");
        printf("4. Display parking and waiting area\n");
        printf("5. Search vehicle using AVL tree\n");
        printf("6. Display removed vehicle history\n");
        printf("7. Display AVL tree\n");
        printf("8. Show parking map\n");
        printf("9. BFS traversal\n");
        printf("10. DFS traversal\n");
        printf("11. Hashing demonstration\n");
        printf("12. Calculate parking fee\n");
        printf("13. Compare linear and binary search\n");
        printf("0. Exit\nChoice: ");
        choice=readNumber();
        switch(choice) {
            case 1: parkVehicle(); break;
            case 2: removeVehicle(); break;
            case 3: addWaitingVehicle(); break;
            case 4: displayVehicles(); break;
            case 5: searchVehicle(); break;
            case 6: displayHistory(); break;
            case 7: treeMenu(); break;
            case 8: showGraph(); break;
            case 9: bfs(); break;
            case 10: dfs(); break;
            case 11: hashingDemo(); break;
            case 12: calculateFee(); break;
            case 13: compareSearchMethods(); break;
            case 0: printf("Parking system closed.\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while(choice!=0);
    freeTree(root);
    while(history) {
        struct HistoryNode *t=history;
        history=history->next;
        free(t);
    }
    return 0;
}
