#ifndef FUT0R_TBF_HEADER
#define FUT0R_TBF_HEADER

/* TBF: To be freed */

typedef void (*free_fun_t)(void*);
typedef void (*free_fun0_t)();

typedef struct tbf_node {
  void *obj;
  free_fun_t f;
  struct tbf_node *next;
} tbf_node_t;

typedef struct {
  tbf_node_t *nodes;
} tbf_t;

void tbf_init(tbf_t* tbf);

void tbf_add(tbf_t* tbf, void* obj, free_fun_t f);

void tbf_free_all(tbf_t* tbf);

#endif
