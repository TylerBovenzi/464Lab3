
typedef struct Node{
    int socket;
    char handle[101];
    struct Node* next;
} Node;

typedef struct linkedList{
    Node* head;
    Node* tail;
    int size;
} linkedList;

char* getHandleAtIndex(linkedList *ll, int index);

int getSocketAtIndex(linkedList *ll, int index);

int copyHandle(linkedList *ll, char *newlocation,int index);

void initList(struct linkedList* ll);

void appendNode(struct linkedList* ll, int socket, char* handle, int handleLen);

void removeNode(struct linkedList* ll, int index);

void printList(struct linkedList* ll);

void setHandle(struct linkedList* ll, int index, char* handle, int handleLen);

int getHandle(struct linkedList* ll, char* handle, int handleLen);

int getSocket(struct linkedList* ll, int socket);
