
struct iNodeData{
    int num;
};
typedef struct iNodeData iNodeData;

struct Data {
    char name[30];
    iNodeData *myData;
};
typedef struct Data Data;

struct myTree {
    Data data;
    struct myTree *kid;
    struct myTree *next;
};
typedef struct myTree myTree;

// functions
myTree* newNode(Data data);
myTree* addNext(myTree* node, Data data);
myTree* addKid(myTree* node, Data data);
myTree *finderKids(myTree *toCheck, char *name);
myTree* search(myTree* root, Data data);