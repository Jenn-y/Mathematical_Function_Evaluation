/* Task is to minimize	f(x) or maximize f(x) (depending on the value of minmax variable)
   using intersection points of quadratic equations, in the given interval lb-ub.

   Input:
        lb,ub (real)	     - an interval containing a global minimizer/maximizer
		gamma (real)	     - an upper bound for |f''(x)| for all x
		tolerance (real)	 - the tolerance for the accuracy of the computed
				               globally minimal/maximal value of f
		display (integer)	 - displays iterations if display = 0 (default)
							   otherwise doesn't display
		minmax (integer)	 - minimize f(x) if minmax = 0 (default)
							   otherwise maximize f(x)
        row, col             - number of rows and column of a matrix

   Output:
		f (real)	         - computed globally minimal/maximal value of f(x)
                               on [lb,ub]; differs from the exact solution
                               by no more than allowed by tolerance variable
		z (real)	         - computed global minimizer/maximizer

This program contains simple function minimization example.
Example input values: lb = -2, ub = 2, gamma = 20, tolerance = 0.1
For matrix computations, appropriate matrix can be provided in
a text file, and the according function evaluations must then be specified.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct FunctionData {
    int id;
    double data;
};

//using linked lists to keep track of data for
//function intersection points
struct LinkedList{
    struct FunctionData value;
    struct LinkedList *next;
};

struct LinkedList* createNode(int id, double data){
    struct LinkedList *temp = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    temp->next = NULL;
    temp->value.id = id;
    temp->value.data = data;
    return temp;
}

void addToEnd (struct NODE *head, int id, double data){

    struct LinkedList *p = createNode(id, data), *q = head;
    while(q->next!=NULL){
        q = q->next;
    }
    q->next = p;
}

void insertIntoList (struct LinkedList *a, int id, double data){
    struct LinkedList *p = createNode(id, data);
    p->next = a->next;
    a->next = p;
    //inserting at a specific place in a list requires
    //updating id-s by 1 after that place
        while (a->next != NULL){
            if (a->value.id == id){
                while (a->next != NULL){
                    a = a->next;
                    a->value.id = a->value.id+1;
                }
            break;
            }
        else a = a->next;
        }
}

struct LinkedList* searchForNode(struct LinkedList *head, int index){
    if (index == 0) return head;
    else {
        while (head->next != NULL){
            head = head->next;
            if (head->value.id == index)return head;
        }
    }
    return NULL;
}

void updateValue (struct LinkedList *a, int id, double newValue){
    while (a->next != NULL){
        if (a->value.id == id){
            a->value.data = newValue;
            return;
        }
        else a = a->next;
    }
}

void freeList(struct LinkedList *head){
    struct LinkedList *tmp;

    while (head != NULL){
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

//Function for dynamic matrix allocation
//and initialization from file matrix.txt
//if matrix is needed for computation
double **createMatrix(int row, int col){

    double **matrix = malloc(row * sizeof(double*));
    for (int i = 0; i < row; i++) matrix[i] = malloc(col * sizeof(double));
    if(!matrix) {
        fputs(stderr, "Error allocating memory!");
        exit(-1);
    }

    FILE *file = fopen("matrix.txt", "r");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
           if (!fscanf(file, "%lf", &matrix[i][j])) break;
        }
    }
    fclose(file);
    return matrix;
}

//Function example
const double PI = atan(1) * 4;
void feval(double x, double ***mat, double *f, double *g){

    *f = log(PI) + log(1 + pow((3 - x), 2));
    *g = 0.49714987269 - 2 / (3-x);

    /* Example on matrix syntax, as it is triple pointed
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
        printf("%lf ", (*mat)[i][j]);
        }
        printf("\n");
    }
    */
}

int main()
{
    double lb, ub, gamma, tolerance, upbound = 0, lowbound = 0, z = 0, f = 0;
    int mi = 1, display = 0, minmax = 0, iter = 1, counter = 2, row, col;
    double **matrix = NULL;

    printf("\nEnter lower bound: ");
    scanf("%lf", &lb);
    printf("\nEnter upper bound: ");
    scanf("%lf", &ub);
    printf("\nEnter gamma value: ");
    scanf("%lf", &gamma);
    printf("\nEnter tolerance: ");
    scanf("%lf", &tolerance);
    printf("\nEnter 0 in case you want display: ");
    scanf("%d", &display);
    printf("\nEnter 0 in case you want to minimize: ");
    scanf("%d", &minmax);

    printf("\nEnter number of rows: ");
    scanf("%d", &row);
    printf("\nEnter number columns: ");
    scanf("%d", &col);
    matrix = createMatrix(row, col);

    //Evaluates the function first at the interval points
    double flb, glb, fub, gub;
    feval(lb, &matrix, &flb, &glb);
    feval(ub, &matrix, &fub, &gub);

    double denom = 2*((ub -lb) - glb/gamma + gub/gamma);
    //First intersection point
    double xint = ((2/gamma)*(flb - fub) - pow(lb, 2) + pow(ub,2) - 2*lb*glb/gamma + 2*ub*gub/gamma)/denom;
    double qint = flb + glb*(xint - lb) - (gamma/2)*pow((xint - lb), 2);

    struct LinkedList *xhead = createNode(0, lb);
    struct LinkedList *fhead = createNode(0, flb);
    struct LinkedList *ghead = createNode(0, glb);

    //Initially, lists contain 3 points
    addToEnd(xhead, 1, xint); addToEnd(xhead, 2, ub);
    addToEnd(fhead, 1, qint); addToEnd(fhead, 2, fub);
    addToEnd(ghead, 1, 0); addToEnd(ghead, 2, gub);

    if (flb < fub){
        upbound = flb;
        z = lb;
    }
    else {
        upbound = fub;
	    z = ub;
    }
    lowbound = qint;

    if (display == 0){
        if (minmax != 0) printf("%d. lowbound: %lf upbound: %lf\n", iter, -lowbound, -upbound);
        else printf("%d. lowbound: %lf upbound: %lf\n", iter, lowbound, upbound);
    }

    while ((upbound - lowbound) > tolerance) {

        double x = searchForNode(xhead, mi)->value.data;
        double g;
        feval(x, &matrix, &f, &g);

        updateValue(xhead, mi, x);
        updateValue(fhead, mi, f);
        updateValue(ghead, mi, g);

        //LEFT INTERSECTION
        double xl = searchForNode(xhead, mi-1)->value.data;
        double fl = searchForNode(fhead, mi-1)->value.data;
        double gl = searchForNode(ghead, mi-1)->value.data;

        denom = 2*((x - xl) - gl/gamma + g/gamma);
        double xintl = ((2/gamma)*(fl - f) - pow(xl, 2) + pow(x, 2) - 2*xl*gl/gamma + 2*x*g/gamma)/denom;
        double qintl = fl + gl*(xintl - xl) - (gamma/2)*pow((xintl - xl), 2);
        insertIntoList(searchForNode(xhead, mi-1), mi, xintl);
        insertIntoList(searchForNode(fhead, mi-1), mi, qintl);
        insertIntoList(searchForNode(ghead, mi-1), mi, 0);
        counter++;

        if ((xintl < lb) || (xintl > ub)){
            printf("Warning: The result may not be as accurate as required!");

            if (minmax != 0)
                f = -upbound;
            else
                f = upbound;
            break;
        }

        //RIGHT INTERSECTION
        double xr = searchForNode(xhead, mi+2)->value.data;
        double fr = searchForNode(fhead, mi+2)->value.data;
        double gr = searchForNode(ghead, mi+2)->value.data;

        denom = 2*((xr - x) - g/gamma + gr/gamma);
        double xint2 = ((2/gamma)*(f - fr) - pow(x, 2) + pow(xr, 2) - 2*x*g/gamma + 2*xr*gr/gamma)/denom;
        double qint2 = f + g*(xint2 - x) - (gamma/2)*pow((xint2 - x), 2);
        insertIntoList(searchForNode(xhead, mi+1), mi+2, xint2);
        insertIntoList(searchForNode(fhead, mi+1), mi+2, qint2);
        insertIntoList(searchForNode(ghead, mi+1), mi+2, 0);
        counter++;

        if ((xint2 < lb) || (xint2 > ub)){
            printf("Warning: The result may not be as accurate as required!");

            if (minmax != 0)
                f = -upbound;
            else
                f = upbound;
            break;
        }

        if (f < upbound) {
            upbound = f;
            z = x;
        }

        //Finds minimal value of the intersection points
        //to update the lower bound
        lowbound = searchForNode(fhead, 1)->value.data;
        mi = 1;
        if (lowbound > searchForNode(fhead, counter)->value.data){
            lowbound = searchForNode(fhead, counter)->value.data;
            mi = counter;
        }
        for (int i = 3; i < counter; i += 2){
            if (searchForNode(fhead, i)->value.data < lowbound){
                lowbound = searchForNode(fhead, i)->value.data;
                mi = i;
            }
        }

        iter++;
        if (display == 0){
            if (minmax != 0) printf("%d. lowbound: %lf upbound: %lf\n", iter, -lowbound, -upbound);
            else printf("%d. lowbound: %lf upbound: %lf\n", iter, lowbound, upbound);
        }
}
    if (minmax != 0) f = -upbound;
    else f = upbound;
    printf("\n\nF: %lf \nZ: %lf", f, z);

    for (int i=0; i<row; i++) { free(matrix[i]); }
    free(matrix);
    freeList(xhead);
    freeList(fhead);
    freeList(ghead);
    return 0;
}
