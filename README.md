# dynlib

Dynamic library loading for Carp, based on [`dlfcn.h`](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/dlfcn.h.html).

## Typed, ownership-aware bindings

For new code, define a binder with the exact C ABI signature and use either a
pinned or owned library:

```clojure
(load "https://github.com/carpentry-org/dynlib@0.3.0")

(DynLib.defpinned-binder bind-floor (Fn [Double] Double))

(defn main []
  (match (DynLib.open-pinned "libm.so.6")
    (Result.Error error) (IO.errorln &error)
    (Result.Success library)
      (match (bind-floor &library "floor")
        (Result.Error error) (IO.errorln &error)
        (Result.Success floor)
          (println* &(Double.str (DynLibPinnedFn.call1 &floor 3.9))))))
```

`open-pinned` intentionally keeps the loader handle alive for the process. Its
binder only borrows the handle, so one library can supply many functions. This
is the conservative choice for Rust `cdylib`s, callbacks, thread-local state,
and libraries that may retain references to their own code.

`open-owned` and `defbinder` instead transfer the handle into a
`DynLibBoundFn`. Carp closes it when the bound function is dropped, so the
function cannot outlive its library. This scoped form binds one function per
handle; reopening the same path normally reuses the platform loader's existing
image and increments its reference count.

Call bound functions with `DynLibPinnedFn.call0` through `call8`, or the
corresponding `DynLibBoundFn` functions. Lookup returns each `Lambda` by value
and does not allocate a wrapper.

The binder declaration is necessarily an assertion: `dlsym` exposes neither a
portable type nor ABI reflection. Calling a symbol through a signature that
does not exactly match its exported C ABI is undefined behavior. Binding
generators should therefore emit these declarations from an authoritative API
description rather than asking users to write them manually.

## Usage

Provided there is a function `inc` that increments a number in a library
`libt.so`—and in fact this is exactly the C to Carp example provided in the
`examples` directory—, you can look up and call this function at runtime as
follows:

```clojure
(load "https://github.com/carpentry-org/dynlib@0.2.0")

(defn main []
  (println*
    &(=> (DynLib.open "libt.so")
         (Result.and-then
           (fn [lib] (DynLib.get lib "inc")))
         (Result.map (fn [f] (Int.str (f 1)))))))
```

`open` and `get` return `(Result a String)`, so the error branch already
carries the real `dlerror()` message—no need to wrap in `Maybe.to-result`.

If you want to throw safety out of the window, something like this could also
work—though I wholeheartedly advise against it:

```clojure
(load "https://github.com/carpentry-org/dynlib@0.2.0")

(defn main []
  (let [lib (Result.unsafe-from-success (DynLib.open "libt.so"))
        f (Result.unsafe-from-success (DynLib.get lib "inc"))]
    (println* &(Int.str (f 1)))))
```

This is equivalent to the above, but it will crash and burn if any of the
assumptions cannot be fulfilled. Stay safe, folks!

Both of the examples do not use `DynLib.close` to clean up the resource after
they are done—mostly for brevity’s sake—, but I’d really hope you’ll do that.
Do as I say, not as I do!

## Limitations

The compatibility `DynLib.get` API returns functions typed as `a`,
so that we are able to encode multi-arity functions (i.e. functions with
different numbers of arguments). I’m not aware of a better way to encode this
in that API. Prefer the typed binders above, which confine the polymorphic raw
lookup behind a generated concrete function signature.

The compatibility lookup also allocates a `Lambda` wrapper that is not freed.
The ownership-aware API returns the wrapper by value and avoids that leak.

<hr/>

Have fun!
