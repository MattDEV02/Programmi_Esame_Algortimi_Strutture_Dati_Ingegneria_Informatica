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

typedef struct nodo_abr
{
   struct nodo_abr *parent;
   struct nodo_abr *left;
   struct nodo_abr *right;
   int key;
} nodo_abr;

typedef nodo_abr *albero;

void smarcatore_non_dfs(grafo *g)
{
   if (g != NULL)
   {
      elem_nodi *ln = g->nodi;
      while (ln != NULL)
      {
         ln->info->color = 0;
         ln = ln->next;
      }
   }
}

nodo *nodo_opposto(arco *a, nodo *n)
{
   return a->from == n ? a->to : a->from;
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

int visita_componenti(grafo *g)
{
   smarcatore_non_dfs(g);
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

int *dimensioni_componenti_to_array(grafo *g)
{
   int mark = visita_componenti(g);
   int i;
   int *array = (int *)(malloc(mark * sizeof(int)));
   for (i = 1; i <= mark; i++)
   {
      array[i - 1] = nodi_componente(g, i);
   }
   return array;
}

nodo_abr *inserisci_in_abr(albero *a, int key)
{
   nodo_abr *new = (nodo_abr *)malloc(sizeof(nodo_abr)); // da inserire
   new->key = key;
   new->left = NULL;  // lo aggiungo come foglia
   new->right = NULL; // lo aggiungo come foglia
   albero prec = *a;
   albero curr = *a;
   while (curr != NULL)
   {
      if (key < curr->key)
      {
         prec = curr;
         curr = curr->left;
      }
      else if (key > curr->key)
      {
         prec = curr;
         curr = curr->right;
      }
      else // trovata la chiave (c'era gia') (key == curr->key)
      {
         curr->key = key;
         return curr;
      }
   }
   // se sei qui curr == NULL, prec == NULL oppure prec e' genitore del nuovo nodo
   if (prec == NULL)
   {
      *a = new;
      new->parent = NULL;
      return (new);
   }
   else
   {
      if (prec->key > key)
      {
         prec->left = new;
         new->parent = prec;
         return new;
      }
      else
      {
         prec->right = new;
         new->parent = prec;
         return new;
      }
   }
}

void inserisci_array_in_abr(albero *a, int *array, int n)
{
   if (array != NULL)
   {
      int i;
      for (i = 0; i < n; i++)
      {
         inserisci_in_abr(a, array[i]);
      }
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

albero abr_da_grafo(grafo *g)
{
   int *dimensioni_componenti = dimensioni_componenti_to_array(g);
   int n = visita_componenti(g);
   albero a = NULL;
   inserisci_array_in_abr(&a, dimensioni_componenti, n);
   return a;
}

void stampa_albero(albero a)
{
   if (a != NULL)
   {
      printf("%d ", a->key);
      stampa_albero(a->left);
      stampa_albero(a->right);
   }
}

int main(void)
{
   grafo *g = new_grafo_oggetti();
   nodo *n1 = aggiungi_nodo(g, 1);
   nodo *n2 = aggiungi_nodo(g, 2);
   nodo *n3 = aggiungi_nodo(g, 3);
   nodo *n4 = aggiungi_nodo(g, 4);
   nodo *n5 = aggiungi_nodo(g, 5);
   nodo *n6 = aggiungi_nodo(g, 6);
   nodo *n7 = aggiungi_nodo(g, 7);
   nodo *n8 = aggiungi_nodo(g, 8);
   nodo *n9 = aggiungi_nodo(g, 8);
   nodo *n10 = aggiungi_nodo(g, 8);
   nodo *n11 = aggiungi_nodo(g, 8);
   nodo *n12 = aggiungi_nodo(g, 8);
   nodo *n13 = aggiungi_nodo(g, 8);
   arco *a1 = aggiungi_arco(g, n1, n2, 1);
   arco *a2 = aggiungi_arco(g, n3, n4, 1);
   arco *a3 = aggiungi_arco(g, n4, n5, 1);
   arco *a4 = aggiungi_arco(g, n5, n6, 1);
   arco *a5 = aggiungi_arco(g, n8, n9, 1);
   arco *a6 = aggiungi_arco(g, n9, n10, 1);
   arco *a7 = aggiungi_arco(g, n9, n11, 1);
   arco *a8 = aggiungi_arco(g, n10, n11, 1);
   arco *a9 = aggiungi_arco(g, n10, n12, 1);
   arco *a10 = aggiungi_arco(g, n12, n13, 1);

   albero a = abr_da_grafo(g);
   stampa_albero(a);
   return EXIT_SUCCESS;
}
