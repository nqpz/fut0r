#include "base.h"
#include "tbf.h"

void tbf_init(tbf_t* tbf) {
  tbf->nodes = NULL;
}

void tbf_add(tbf_t* tbf, void* obj, free_fun_t f) {
  tbf_node_t *node = (tbf_node_t*) malloc(sizeof(tbf_node_t));
  node->obj = obj;
  node->f = f;
  node->next = tbf->nodes;
  tbf->nodes = node;
}

void tbf_free_all(tbf_t* tbf) {
  tbf_node_t *node = tbf->nodes;
  while (node != NULL) {
    if (node->obj == NULL) {
      ((free_fun0_t) node->f)();
    } else {
      node->f(node->obj);
    }
    tbf_node_t *next = node->next;
    free(node);
    node = next;
  }
}
