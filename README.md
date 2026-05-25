# dynlib

Dynamic library loading for Carp, based on [`dlfcn.h`](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/dlfcn.h.html).

## Usage

Provided there is a function `inc` that increments a number in a library
`libt.so`—and in fact this is exactly the C to Carp example provided in the
`examples` directory—, you can look up and call this function at runtime as
follows:

```clojure
(load "https://github.com/carpentry-org/dynlib@0.0.5")

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
(load "https://github.com/carpentry-org/dynlib@0.0.5")

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

For now, the functions that are returned by `DynLib.get` are all typed as `a`,
so that we are able to encode multi-arity functions (i.e. functions with
different numbers of arguments). I’m not aware of a better way to encode this
in the Carp type system as of yet. If there is, hit me up, because the current
implementation breaks all type-level guarantees!

I’m also pretty sure that the lambdas allocated by `DynLib_dlsym` are never
freed—because they’re returned as references—, and I’m not sure how to get
around that!

<hr/>

Have fun!
