typedef void* Lib;

bool DynLib_isvalid(void* a) {
  return a != NULL;
}

Lambda* DynLib_dlsym(void* l, char* f) {
  void* cb = dlsym(l, f);
  if (!cb) return NULL;
  Lambda* x = malloc(sizeof(Lambda));
  x->callback = cb;
  x->env = NULL;
  x->delete = NULL;
  x->copy = NULL;
  return x;
}
