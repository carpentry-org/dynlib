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

/* Ownership-aware API. Unlike DynLib_dlsym, these helpers return Lambda by
 * value and therefore do not allocate. */
typedef void* DynLibOwned;
typedef void* DynLibPinned;

static DynLibOwned DynLib_open_owned(String* path) {
  dlerror();
  return dlopen(*path, RTLD_NOW | RTLD_LOCAL);
}

static DynLibPinned DynLib_open_pinned(String* path) {
  dlerror();
  return dlopen(*path, RTLD_NOW | RTLD_LOCAL);
}

static void DynLibOwned_delete(DynLibOwned library) {
  if (library != NULL) (void)dlclose(library);
}

Lambda DynLib_symbol_owned(DynLibOwned* library, String* name) {
  Lambda result = {0};
  dlerror();
  result.callback = dlsym(*library, *name);
  return result;
}

Lambda DynLib_symbol_pinned(DynLibPinned* library, String* name) {
  Lambda result = {0};
  dlerror();
  result.callback = dlsym(*library, *name);
  return result;
}

static bool DynLib_valid_handle(void** library) {
  return *library != NULL;
}

bool DynLib_valid_lambda(Lambda* function) {
  return function->callback != NULL;
}

static String DynLib_error_string(void) {
  const char* error = dlerror();
  return String_from_MINUS_cstr(
      (char*)(error == NULL ? "dynamic loader error" : error));
}
