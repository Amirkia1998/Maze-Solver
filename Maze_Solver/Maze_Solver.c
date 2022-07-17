#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <WinCon.h>
#define N 255

//--------------------------------------------------STRUCTURES
typedef struct node {
    int vertex;
    struct node* next;
    int x;
    int y;
    int apple;
}NODE;

typedef struct graph {
    int numVertices;
    NODE** adjLists;
    int* visited;
}GRAPH;

//--------------------------------------------------GLOBAL VARIABLES
GRAPH* mazeGraph;                   //graph of maze
int mazeMatrixBinary[N][N];       // binary matrix of maze
char mazeMatrixChar[N][N];               // holds maze like the way we see it in the text file
int row_no=0;
int col_no=0;
int row_cell_no=0;
int col_cell_no=0;

int startVertex;            
int startX;
int startY;
int exitVertex;
int exitX;
int exitY;

int found=0;
int score=0;

//----------------------------------------------------PROTOTYPES
void readMaze();
void readMazeBinary();
void printMatrix(int matrix[N][N], int row_no, int col_no);
void printMatrixChar(char matrix[N][N], int row_no, int col_no);
NODE* createNode(int v, int y, int x);
GRAPH* createAGraph(int vertices);
void addEdge(int s, int y1, int x1, int d, int y2, int x2);
void printGraph(GRAPH* graph);
void addAllEdges();
void solveMaze(GRAPH* graph, int vertex);
void addStar(int vertex);
void removeStar(int vertex);
int hasApple(int vertex);
void findStart();
void findExit();


//######################################################################################################
//########################################## MAIN ######################################################
//######################################################################################################
int main(void) {

    printf("Initial Maze:\n");
    readMaze();     // reads maze character by character and stores it in a character matrix
    
    readMazeBinary();   // reads maze as binary and stores it in a integer matrix

    findStart();        // finds starting point of matrix (s)
    findExit();         // finds exiting oint of the matrix (e)
    
    // create graph for maze
    mazeGraph = createAGraph((row_no/2)*(col_no/2));     
    
    // add all the nodes of the maze to the graph (adjacency list)
    addAllEdges();

    // solve maze
    solveMaze(mazeGraph, startVertex);


    printMatrixChar(mazeMatrixChar, row_no, col_no);
    

    printf("\n####################\nFinal Score: %d\n####################\n", score);
    printf("\nEND");

}

//############################################################################################
//#################################### FUNCTIONS #############################################
//############################################################################################
void readMaze() {
    FILE *fptr;
    char c;
    char line[N]; 
    int i, j;
    
    fptr = fopen("maze.txt", "r");
    if (fptr == NULL)
    {
        printf("Error opening file.\n");
    }
    
    // get number of rows and columns of the maze 
    fgets(line, 255, fptr);
    col_no = strlen(line) - 1;        // for this maze 15
    rewind(fptr);
    while (!feof(fptr))
    {
        c = fgetc(fptr);
        if (c == '\n')
        {
            row_no++;
        }
    }
    row_no++;
    
    // initialize number of cells
    row_cell_no = row_no/2;
    col_cell_no = col_no/2;

    rewind(fptr);
    while (!feof(fptr))
    {
        for ( i = 0; i < row_no; i++)
        {
            for ( j = 0; j < col_no; j++)
            {
                c = fgetc(fptr);
                mazeMatrixChar[i][j] = c; 
            }

            c = fgetc(fptr);    // to get rid of '\n' character at the end of each line
        }
    }
    
    fclose(fptr);
}


void readMazeBinary() {
    FILE *fptr;
    char c;
    char line[N]; 
    int i, j;
    
    fptr = fopen("maze.txt", "r");
    if (fptr == NULL)
    {
        printf("Error opening file.\n");
    }

    // initialize walls with 0's and cells with 1's
    rewind(fptr);
    while (!feof(fptr))
    {
        for ( i = 0; i < row_no; i++)
        {
            for ( j = 0; j < col_no; j++)
            {
                c = fgetc(fptr);
                if (c == '+' || c == '-' || c == '|')
                {
                    mazeMatrixBinary[i][j] = 0;
                }
                else mazeMatrixBinary[i][j] = 1;           
            }

            c = fgetc(fptr);    // to get rid of '\n' character at the end of each line
        }
    }
    
    fclose(fptr);
}


void printMatrix(int matrix[N][N], int row_no, int col_no) {
    int i, j;
    for ( i = 0; i < row_no; i++)
    {
        for ( j = 0; j < col_no; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}


// prints the maze like the way we see it on txt file
void printMatrixChar(char matrix[N][N], int row_no, int col_no) {
    int i, j;
    for ( i = 0; i < row_no; i++)
    {
        for ( j = 0; j < col_no; j++)
        {
            if (matrix[i][j] == '*' && found)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
                printf("%c ", matrix[i][j]);
            }
            else if (matrix[i][j] == '*')
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
                printf("%c ", matrix[i][j]);
            }
            else if (matrix[i][j] == 'E')
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
                printf("%c ", matrix[i][j]);
            }
            else 
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                printf("%c ", matrix[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}


NODE* createNode(int v, int y, int x) {
    NODE* newNode = malloc(sizeof(NODE));
    newNode->vertex = v;
    newNode->x = x;
    newNode->y = y;
    newNode->apple = 0;
    newNode->next = NULL;
    return newNode;
}


GRAPH* createAGraph(int vertices) {
    GRAPH* graph = (GRAPH*)malloc(sizeof(GRAPH));
    graph->numVertices = vertices;

    graph->adjLists = (NODE**)malloc(vertices * sizeof(NODE*));
    graph->visited = malloc(vertices * sizeof(int));

    int i;
    for (i = 0; i < vertices; i++) {
      graph->adjLists[i] = NULL;
      graph->visited[i] = 0;
    }

    return graph;
}


void addEdge(int s, int y1, int x1, int d, int y2, int x2) {
  // Add edge from s to d
    NODE* newNode = createNode(d, y2, x2);
    newNode->next = mazeGraph->adjLists[s];   // doing addFront()
    mazeGraph->adjLists[s] = newNode;

    /*
    // Add edge from d to s (not needed in this program)
    newNode = createNode(s, x1, y1);
    newNode->next = graph->adjLists[d];
    graph->adjLists[d] = newNode;
    */
}


void printGraph(GRAPH* graph) {
    int v;
    for (v = 0; v < graph->numVertices; v++) {
      NODE* temp = graph->adjLists[v];
      printf("\n Vertex %d\n: ", v);
      while (temp) {
        printf("%d -> ", temp->vertex);
        temp = temp->next;
      }
      printf("\n");
    }
}


// iterates the binary matrix of maze and adds all the edges to the graph
void addAllEdges() {
    int i=1, j=1, v=0;

    while (i < row_no)
    {
        j = 1;
        while (j < col_no)
        {
            if (mazeMatrixBinary[i][j+1] == 1)
            {
                addEdge(v, i, j, v+1, i, j+2);
            }
            if (mazeMatrixBinary[i][j-1] == 1)
            {
                addEdge(v, i, j, v-1, i, j-2);
            }
            if (mazeMatrixBinary[i+1][j] == 1)
            {
                addEdge(v, i, j, v+col_cell_no, i+2, j);
            }
            if (mazeMatrixBinary[i-1][j] == 1)
            {
                addEdge(v, i, j, v-col_cell_no, i-2, j);
            }
            j += 2;
            v++;
        }

        i += 2;
    }
    
}


void solveMaze(GRAPH* graph, int vertex) {

    NODE* adjList = graph->adjLists[vertex];      // or NODE* row = graph->adjLists[vertex]
    NODE* temp = adjList;

    graph->visited[vertex] = 1;
    if (hasApple(vertex))
    {
        score += 10;
    }
    //printf("Visited %d \n", vertex);

    sleep(1);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    printf("Score: %d\n", score);
    addStar(vertex);
    printMatrixChar(mazeMatrixChar, row_no, col_no);

    while (temp != NULL && !found) {
        int connectedVertex = temp->vertex;

        if (temp->vertex == exitVertex) {   
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
            printf("\n\nFound!\n");    
            found = 1;       
            addStar(vertex);
        }
        else if (graph->visited[connectedVertex] == 0) {   // or graph->visited[tmp->vertex]
            solveMaze(graph, connectedVertex);                // solveMaze(graph, tmp->vertex)
        }
    
        temp = temp->next;
    }

    if (!found)
    {
        sleep(1);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        printf("Score: %d\n", score);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        printMatrixChar(mazeMatrixChar, row_no, col_no);
        score -= 5;             
        removeStar(vertex);
    }
    
}


// adds star character to the current cell(vertex) of the character matrix 
void addStar(int vertex) {
    int i=1, j=1, v=0;

    while (i < row_no)
    {
        j = 1;
        while (j < col_no)
        {
            if (v == vertex)
            {
                mazeMatrixChar[i][j] = '*';
            }
            

            j += 2;
            v++;
        }

        i += 2;
    }

}


// removes star character from the current cell(vertex) of the character matrix 
void removeStar(int vertex) {
    int i=1, j=1, v=0;

    while (i < row_no)
    {
        j = 1;
        while (j < col_no)
        {
            if (v == vertex)
            {
                mazeMatrixChar[i][j] = ' ';
            }

            j += 2;
            v++;
        }

        i += 2;
    }

}


// checks if the currrent cell(vertex) has apple
int hasApple(int vertex) {
    int i=1, j=1, v=0;

    while (i < row_no)
    {
        j = 1;
        while (j < col_no)
        {
            if (v == vertex && mazeMatrixChar[i][j] == 'O')
            {
                return 1;
            }

            j += 2;
            v++;
        }

        i += 2;
    }

    return 0;
}


// finds the starting point of the maze
void findStart() {
int i=1, j=1, v=0;

    while (i < row_no)
    {
        j = 1;
        while (j < col_no)
        {
            if (mazeMatrixChar[i][j] == 'S')
            {
                startVertex = v;
                startX = j;
                startY = i;
                return;
            }

            j += 2;
            v++;
        }

        i += 2;
    }
}


// finds the exiting point of the maze
void findExit() {
int i=1, j=1, v=0;

    while (i < row_no)
    {
        j = 1;
        while (j < col_no)
        {
            if (mazeMatrixChar[i][j] == 'E')
            {
                exitVertex = v;
                exitX = j;
                exitY = i;
                return;
            }

            j += 2;
            v++;
        }

        i += 2;
    }

}