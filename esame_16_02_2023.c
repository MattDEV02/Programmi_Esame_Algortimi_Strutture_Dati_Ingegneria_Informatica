/*
   FUNZ(T)
      L.head = NULL
      FUNZ_RIC(T.root, L)
      return L

   FUNZ_RIC(v, L)
      if(v == NULL) return
      if(LUNGHEZZA(l) <= 10)
         AGGIUNGI_IN_TESTA(L)
         FUNZ_RIC(v.left, L)
      else
         AGGIUNGI_IN_TESTA(L)
         FUNZ_RIC(v.right, L)
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct elem_lista_nodi elem_nodi;   // forward declaration
typedef struct elem_lista_archi elem_archi; // forward declaration

typedef struct nodo_struct
{
   elem_nodi *pos;
   elem_archi *archi;
   int id;
   int value;
   int color;
} nodo;

typedef struct elem_lista_nodi
{
   struct elem_lista_nodi *prev;
   struct elem_lista_nodi *next;
   nodo *info;
} elem_nodi;

typedef struct arco_struct
{
   int id;
   int weight;
   elem_archi *pos;
   nodo *from;
   nodo *to;
   elem_archi *frompos;
   elem_archi *topos;
} arco;

typedef struct elem_lista_archi
{
   struct elem_lista_archi *prev;
   struct elem_lista_archi *next;
   arco *info;
} elem_archi;

typedef struct grafo_struct
{
   int numero_nodi;
   int numero_archi;
   elem_nodi *nodi;
   elem_archi *archi;
} grafo;

typedef elem_nodi *lista_nodi;

typedef elem_archi *lista_archi;

typedef struct nodo_albero
{
   struct nodo_albero *parent;
   struct nodo_albero *right_sibling; // primo figlio (sinistro)
   struct nodo_albero *left;          // fratello destro
   int info;
} nodo_albero;

typedef nodo_albero *albero;

nodo_albero *aggiungi_radice(albero a, int value)
{
   if (a == NULL)
   {
      a = (nodo_albero *)malloc(sizeof(nodo_albero));
      a->info = value;
      a->parent = NULL;
      a->left = NULL;
      a->right_sibling = NULL;
   }
}

nodo_albero *aggiungi_figlio(nodo_albero *n, int value)
{
   if (n != NULL)
   {
      nodo_albero *nuovo = (nodo_albero *)malloc(sizeof(nodo_albero)); // Nodo da aggiungere come figlio (sx) di n (parent)
      nuovo->parent = n;
      nuovo->left = NULL;             // è il più sx
      nuovo->right_sibling = n->left; // quello che prima era il primo figlio sx adesso diventa il secondo figlio
      nuovo->info = value;
      n->left = nuovo;
      return nuovo;
   }
}

grafo *new_grafo_oggetti()
{
   grafo *output = (grafo *)malloc(sizeof(grafo));
   output->numero_nodi = 0;
   output->numero_archi = 0;
   output->nodi = NULL;
   output->archi = NULL;
   return output;
}

nodo *aggiungi_nodo(grafo *g, int value)
{
   nodo *output = (nodo *)malloc(sizeof(nodo));
   output->pos = (elem_nodi *)malloc(sizeof(elem_nodi));
   output->archi = NULL;
   output->color = 0;
   output->pos->info = output;
   output->pos->next = g->nodi; // g->nodi che prima era primo diventa secondo
   output->pos->prev = NULL;    // sono il primo della lista

   output->id = g->numero_nodi + 1;

   output->value = value;

   if (g->nodi != NULL) // c'era almeno un nodo nella lista nodi del grafo
   {
      g->nodi->prev = output->pos;
   }

   g->nodi = output->pos;
   g->numero_nodi++;
   return output;
}

arco *aggiungi_arco(grafo *g, nodo *from, nodo *to, int weight)
{

   arco *output = (arco *)malloc(sizeof(arco));
   output->pos = (elem_archi *)malloc(sizeof(elem_archi));
   output->frompos = (elem_archi *)malloc(sizeof(elem_archi));
   output->topos = (elem_archi *)malloc(sizeof(elem_archi));

   if (weight <= 0)
      output->weight = 1;
   else
      output->weight = weight;

   output->id = g->numero_archi + 1;

   output->from = from;
   output->to = to;

   output->pos->prev = NULL; // sara' il primo della lista del grafo
   output->pos->next = g->archi;
   output->pos->info = output;

   output->frompos->prev = NULL; // sara' il primo della lista del nodo from
   output->frompos->next = from->archi;
   output->frompos->info = output;

   output->topos->prev = NULL; // sara' il primo della lista del nodo from
   output->topos->next = to->archi;
   output->topos->info = output;

   if (g->archi != NULL)
   { // la lista degli archi del grafo non era vuota
      g->archi->prev = output->pos;
   }

   if (from->archi != NULL)
   { // la lista degli archi del nodo from non era vuota
      from->archi->prev = output->frompos;
   }

   if (to->archi != NULL)
   { // la lista degli archi del nodo to non era vuota
      to->archi->prev = output->topos;
   }

   g->archi = output->pos;
   from->archi = output->frompos;
   to->archi = output->topos;

   g->numero_archi++;
   return output;
}

nodo *nodo_opposto(arco *a, nodo *n)
{
   return a->from == n ? a->to : a->from;
}

int conta_foglie(albero a)
{
   if (a == NULL)
      return 0;
   int count = 0;
   nodo_albero *x = a;
   while (x != NULL)
   {
      if (x->left == NULL)
         count++;
      count += conta_foglie(x->left);
      x = x->right_sibling;
   }
   return count;
}

void dfs_marcatore(nodo *n, int mark)
{
   n->color = mark;
   elem_archi *la = n->archi;
   while (la != NULL)
   {
      nodo *altro_nodo = nodo_opposto(la->info, n);
      if (altro_nodo->color == 0) // se non l'ho marcato...
      {
         dfs_marcatore(altro_nodo, mark); // lo marco
      }
      la = la->next;
   }
}

void smarcatore_non_dfs(grafo *g, int mark)
{
   elem_nodi *ln = g->nodi;
   while (ln != NULL)
   {
      ln->info->color = 0;
      ln = ln->next;
   }
}

int visita_componenti(grafo *g)
{
   smarcatore_non_dfs(g, 0);
   lista_nodi ln = g->nodi;
   int mark = 0;
   while (ln != NULL)
   {
      if (ln->info->color == 0) // trovato un nodo non marcato
      {
         mark++;                        // mi darà il numero delle componenti
         dfs_marcatore(ln->info, mark); // assegno la marcatura mark all'i-esimo nodo
      }
      ln = ln->next;
   }
   return mark;
}

int nodi_componente(grafo *g, int mark) // dimensione componente
{
   int count = 0;
   elem_nodi *ln = g->nodi;
   while (ln != NULL)
   {
      if (ln->info->color == mark) // trovato un nodo marcato mark, quindi è un nodo della componente che stiamo analizzando che va contato
      {
         count++;
      }
      ln = ln->next;
   }
   return count;
}

int is_foglia(nodo_albero *foglia)
{
   return foglia->left == NULL;
}

int esiste_comp_conn_con_dim_value(grafo *g, int value)
{
   int mark = visita_componenti(g);
   int i;
   for (i = 1; i <= mark; i++)
   {
      if (nodi_componente(g, i) == value)
         return 1;
   }
   return 0;
}

/*
   La funzione restituisce 1 (cioè true) se esiste una
   foglia dell’albero la cui profondità è uguale al numero di nodi della componente connessa del grafo g che
   ha più nodi, altrimenti la funzione restituisce 0 (cioè false). Se uno (o entrambi) tra grafo e albero è vuoto
   (cioè uguale a NULL) la funzione ritorna 0 (false).
*/
int verifica(nodo_albero *a, grafo *g)
{
   if (a == NULL || g == NULL)
      return 0;
   return esiste_comp_conn_con_dim_value(g, conta_foglie(a));
}

int main(void)
{
   albero a = NULL; // creo un albero vuoto
   a = aggiungi_radice(a, 0);

   nodo_albero *figlio3 = aggiungi_figlio(a, 3);
   nodo_albero *figlio2 = aggiungi_figlio(a, 2);
   nodo_albero *figlio1 = aggiungi_figlio(a, 1);

   grafo *g = new_grafo_oggetti();
   nodo *n1 = aggiungi_nodo(g, 1);
   nodo *n2 = aggiungi_nodo(g, 2);
   nodo *n3 = aggiungi_nodo(g, 3);
   nodo *n4 = aggiungi_nodo(g, 4);
   nodo *n5 = aggiungi_nodo(g, 5);
   nodo *n6 = aggiungi_nodo(g, 6);
   nodo *n7 = aggiungi_nodo(g, 7);
   arco *a1 = aggiungi_arco(g, n1, n2, 1);
   arco *a2 = aggiungi_arco(g, n2, n5, 1);
   arco *a3 = aggiungi_arco(g, n5, n2, 1);
   arco *a4 = aggiungi_arco(g, n3, n4, 1);
   arco *a5 = aggiungi_arco(g, n3, n6, 1);
   arco *a6 = aggiungi_arco(g, n6, n4, 1);
   printf("\nverifica(a, g): %d", verifica(a, g));
   return EXIT_SUCCESS;
}