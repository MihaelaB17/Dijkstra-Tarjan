#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1000 

///definesc structurile pe care ulterior o sa le folosesc in retinerea stivei
typedef struct node {
    int data;
    struct node *next;
    struct node *prev;
} node_t;

typedef struct stack {
    node_t *head;
    node_t *tail;
    int len;
} stack_t;

///mai jos sunt cateva functii utile in lucrul cu stiva, cum ar fi initializarea acesteia, initializare unui nod, scoaterea unui element din stiva, adaugarea unui element in
///stiva, dar si eliberarea memoriei utilizate de stiva

node_t *initNode(int value)
{
    node_t *node = malloc(sizeof(node_t));

    node->data = value;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

stack_t *initStack() 
{
    stack_t *q = malloc(sizeof(stack_t));

    q->head = NULL;
    q->tail = NULL;
    q->len = 0;

    return q;
}

void destroyStack(stack_t *stack) 
{
    node_t *aux;

    while (stack->len > 0) {
        aux = stack->head;
        stack->head = aux->next;
        free(aux);
        stack->len--;
    }
    free(stack);
}

void push(stack_t *stack, int value) 
{
    node_t *nod;
    nod=initNode(value);
    if(stack->len == 0){
        stack->head=nod;
        stack->tail=nod;
        stack->len++;
    }
    else{
        nod->next=stack->head;
        stack->head->prev=nod;
        stack->head=nod;
        stack->len++;
    }
}

int pop(stack_t *stack) 
{ 
    node_t *aux;

    if(stack->len == 1){
        aux=stack->head;
        stack->head=NULL;
        stack->tail=NULL;
        stack->len--;
    }
    else{
        aux=stack->head;
        stack->head=stack->head->next;
        stack->head->prev=NULL;
        stack->len--;
    }
    free(aux);
}

///functia de mai jos initializeza cu -1 doi vectoru ce vor fi folositi pentru algoritmul de la task ul 2

void initializare(int *index, int *lowlink, int n)
{
    int i;
    for(i=0; i<n; i++){
        index[i]=-1;
        lowlink[i]=-1;
    }
}

///functia tarjan ajuta la afiserea componentelor conexe. Avestea vor fi retinute intr-o stiva, iar apoi se va extrage element cu element si vor fi salvate nodurile respective
///intr-un vector. Dupa fiecare componenta conexa gasita se va afisa 0, pentru la o ulterioara parcurgere a vectorului in main sa se stie exact ce noduri formeaza fiecare
///componenta conexa. Pentru a introduce nodul in stiva se va pleca dintr-un nod ales u,car va fi pus in stiva si intializam valoarea sa in vectorul disc ce retine daca un nod a fost vizitat sau nu
///si in vectorul lowlink ce reprezinta nodul cu cea mai mica valoare la care se poate ajunge din nodul u folosind DFS, iar apoi se vor parcurge toate nodurile care se 
///leaga direct de u si se va apela functia recursiv. Dupa ce s-a terminat recursivitatea se vor actualiza valorile vectorilor lowlink cu valoarea minima in functie daca nodul 
///la care am ajuns se afla sau nu deja in stiva. La valori de lowlink si disc egale inseamna ca am gasit o componenta coneta si se incepe parcurgerea ei si eliminarea nodurilor 
//care o formeaza din stiva si introducerea acestora in vectorul rez ce va retine toate componentele conexe. Pentru a evita depozitele declaram un vector noduri care retine valoare 1
///daca nodul respectiv nu este depozit si 0 altfel  

void tarjan( int u, int * disc, int *lowlink, stack_t *stack, int *onstack, int n, float **mat, int d, int *dep, int *rez, int *k)
{
    static int time=0;
    int v, ok, i, *noduri;
    disc[u]=time;
    lowlink[u]=time;
    time+=1;
    push(stack, u);
    onstack[u]=1;
    noduri=malloc(n*sizeof(int));
    for(i=0; i<n; i++)
        noduri[i]=1;
    for(i=0; i<d; i++)
        noduri[dep[i]]=0;

    for(v=0; v<n; v++){
        if(noduri[v] == 1)
            if(mat[u][v] != 0){
                if(disc[v] == -1){
                    tarjan(v, disc, lowlink, stack, onstack, n, mat, d, dep, rez, k);
                    if(lowlink[v] < lowlink[u])
                        lowlink[u]=lowlink[v];
                }
                else
                    if(onstack[v])
                        if(disc[v] < lowlink[u])
                            lowlink[u]=disc[v];
            }
    }
    
    if(lowlink[u] == disc[u] && noduri[u] == 1){
        while(stack->head->data != u){
            rez[*k]=stack->head->data;
            (*k)++;
            onstack[stack->head->data]=0;
            pop(stack);
        }
        rez[*k]=stack->head->data;
        (*k)++;
        rez[*k]=0;
        (*k)++;
        onstack[stack->head->data]=0;
        pop(stack);
        
    }
    free(noduri);
}

///functia Dijkstra calculeaza costul minim dintre 2 noduri. Aceasta ia 2 cazuri: daca nodul de inceput(init) este mai mic decat nodul final(fin) va parcurge nodurile de la 0 la n-1
///si cazul in care init > fin cand va parcurge nodurile de la n-1 la 0. Pentru fiecare caz se va crez matricea m, care retine o valoare maxima preinitializata MAX, daca intre
///doua noduri i si j nu exista legatura, altfel va retine exact costul drumului respectiv. Vectorl d[i] va retine costurile de pe linia vectorului de inceput init (dupa terminarea
///algoritmului in acest vector se vor afla costurile minime de la nodul de inceput <<init>> pana la fiecare nod din graf), iar vectorul p va si initializat cu valoarea nodului
///de inceput(la final in acest vector se vor retine parintele fiecarui nod din parcurgerea facuta si va fi ulit la afisarea drumului). Vectorul s retine 0 daca nodul a fost vizitat
///deja, iar in caz contrar 1. Pentru fiecare nod nevizitat. Parcurg toate nodurile si actualizez costul minim din vectorul d, dar si nodul pentru care am gasit aceast cost(jmin)
///Marchez nodul ca vizitat si mai porgurg o data nodurile pentru a modifica alte evntuale distante mai mici pentru toate nodurile
///La final rezultatul caurara se va afla pe pozitia fin din vectorul d
///Acelasi lucru il voi face si pentru celalat mcaz doar ca parcurgand invers nodurile

float Dijkstra(float **mat, int init, int fin, int n, int *p)
{
    int *s, dmin, i, j, jmin;
    float *d, m[n][n], rez;
    d=calloc(n,sizeof(float));
    s=calloc(n,sizeof(int));


    for(i=0; i<n; i++)
        for(j=0; j<n; j++)
            if(mat[i][j] == 0)
                m[i][j]=MAX;
            else
                m[i][j]=mat[i][j];
                
    if(init < fin){
        for(i=0; i<n; i++){
            d[i]=m[init][i];
            p[i]=init;
        }

        s[init]=1;
        for(i=0; i<n; i++){
            dmin=MAX;
            for(j=0; j<n; j++)
                if(s[j]==0 && d[j] < dmin){
                    dmin=d[j];
                    jmin=j;
                }
            s[jmin]=1;
            for(j=0; j<n; j++){
                if(d[jmin] + m[jmin][j] < d[j]){
                    d[j]=d[jmin] + m[jmin][j];
                    p[j]=jmin;
                }
            }
        }
        rez=d[fin];
        free(d);
        free(s);
        return rez;
    }

    else{
        for(i=n-1; i>=0; i--){
            d[i]=m[init][i];
            p[i]=init;
        }
        
        s[init]=1;
        for(i=n-1; i>=0; i--){
            dmin=MAX;
            for(j=n-1; j>=0; j--)
                if(s[j]==0 && d[j] < dmin){
                    dmin=d[j];
                    jmin=j;
                }
            s[jmin]=1;
            for(j=n-1; j>=0; j--){
                if(d[jmin] + m[jmin][j] < d[j]){
                    d[j]=d[jmin] + m[jmin][j];
                    p[j]=jmin;
                }
            }
        }
        rez=d[fin];
        free(d);
        free(s);
        return rez;
    }
}

///functia parcurgr elementele vectorului o recursiv pentru a afisa nodurile ce formeaza drumul cu cost minim

void drum(int *p, int j)
{
    if(p[j]==-1)
        return;

    drum(p, p[j]);

    printf(" %d", j);
}

int main()
{
    ///citesc toate datele si initializez tablourile de care voi avea nevoie in retinerea datelor
    int n, m, d, i, j, u, v, *dep, nr, s, k, r;
    float w;
    scanf("%d%d%d", &n, &m, &d);

    float **mat=calloc(n, sizeof(int *)), rez;
    int **scc=calloc(n, sizeof(int *)), **mat2=calloc(n, sizeof(int *));

    for(i=0; i<n; i++){
        mat[i]=calloc(n, sizeof(int));
        scc[i]=calloc(n, sizeof(int));
        mat2[i]=calloc(n, sizeof(int));
    }

    for(i=1; i<=m; i++){
        scanf("%d%d%f", &u, &v, &w);
        mat[u][v]=w;
    }
    ///vectorul de depozite dep
    dep=malloc(d*sizeof(int));
    for(i=0; i<d; i++)
        scanf("%d", &dep[i]);

    ///citesc numarul de cerinte ce urmeaza a fi rezolvate si intru pe fiecare caz in functie de carinta citita
    scanf("%d",&nr);
    for(i=1; i<=nr; i++){
        char task[4];
        if(i != 1)
            printf("\n");
        scanf("%s", task);
        if(strcmp(task,"e1")==0){
            scanf("%d%d",&s, &k);
    ///citesc magazinele
            int *mag=malloc(k*sizeof(int));
            for(j=0; j<k; j++)
                scanf("%d", &mag[j]);
            rez=0;
            for(r=0; r<k; r++){
                int *p, t=0;
                p=malloc(n*sizeof(int));
                printf("%d\n", mag[r]);
    ///apelez functia Dijkstra pentru acalcula costul drumului minim atat de la depozitul s la magazinul mag[r],cat si invers pentru a costl total
                printf("%.1f ", Dijkstra(mat, s, mag[r], n, p));
                printf("%.1f\n", Dijkstra(mat, mag[r], s, n, p));
                rez=rez + Dijkstra(mat, s, mag[r], n, p);
                p[s]=-1;
                printf("%d", s);
    ///pargurge vectorul p pentru a afisa drumul dus,apoi mai apelez o data functia Dijkstra pentru a afisa drumul intors
                drum(p, mag[r]);
                rez=rez + Dijkstra(mat, mag[r], s, n, p);
                p[mag[r]]=-1;
                drum(p, s);
                printf("\n");
                free(p);
            }
            printf("%.1f", rez);
            free(mag);
        }

        if(strcmp(task, "e2")==0){
    /// initializez stiva si tablourile de care voi avea vevoie in functia tarjan
            stack_t *stack;
            stack=initStack(stack);
            int  k=0, l1=0, l2=0, j, t, ind=0, ok, y, q, nrscc=0, lin[n];
            int *onstack, *index, *lowlink, *rezultat;
            onstack=calloc(n, sizeof(int));
            index=malloc(n*sizeof(int));
            lowlink=malloc(n*sizeof(int));
            rezultat=malloc(n*n*sizeof(int));
    ///initializez vectorii index si lowlink apoi apelez functia pentru fiecare nod
            initializare(index, lowlink, n);
            for(y=0; y<n; y++){
                tarjan(y, index, lowlink, stack, onstack, n, mat, d, dep, rezultat, &k);
            }
    
            int nrc=0, aux, maxdep=n, pp=0;
    ///iau elementele din vectorul "rezultat" si le pun intr-o matrice, pentru a delimita fiecare componenta tare conexa, evitand zerourile(pe fiecare  linie din matrice se va afla o componenta conexa)
            for(y=0; y<=n+2; y++){
                if(l1 != 0)
                    if(mat2[l1][0] == 1 || mat2[l1][0] == 2)
                        break;
                if(rezultat[y]==0){
                    l1++;
                    l2=0;
                    y++;
                }
                mat2[l1][l2]=rezultat[y];
                l2++;
                if(l2 > nrc)
                    nrc=l2;
                
            }
    ///sortez elementele de pe fiecare lininie a matricei pentru a le afisa crescator
            for(y=0; y<l1; y++){
                for(q=0; q<nrc-1; q++)
                    for(t=q+1; t<nrc; t++)
                        if(mat2[y][q] > mat2[y][t] && mat2[y][q] !=0 && mat2[y][t] != 0){
                            aux=mat2[y][q];
                            mat2[y][q]=mat2[y][t];
                            mat2[y][t]=aux;
                        }
            }
    ///sortez si liniile matricei pentru ale afisa de la cele care contin nodurile cu inice mai mic la cele cu inice mai mare
            for(y=0; y<l1-1; y++){
                if(mat2[y][0] > mat2[y+1][0])
                    for(q=0; q<nrc; q++){
                        aux=mat2[y][q];
                        mat2[y][q]=mat2[y+1][q];
                        mat2[y+1][q]=aux;
                    }
            }

            printf("%d\n", l1);

            for(y=0; y<l1; y++){
                printf("%d", mat2[y][0]);
                for(q=1; q<nrc; q++)
                    if(mat2[y][q] != 0){
                        printf(" %d", mat2[y][q]);
                        }
                if(y != l1-1)
                    printf("\n");
            }
    ///eliberez memoria
            destroyStack(stack);
            free(onstack);
            free(index);
            free(lowlink);
            free(rezultat);
        }
    }

    free(dep);
    for(i=0; i<n; i++){
        free(mat[i]);
        free(scc[i]);
        free(mat2[i]);
    }
    free(mat2);
    free(scc);
    free(mat);
}