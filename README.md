# dynlib

Dynamic library loading for Carp, based on [`dlfcn.h`](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/dlfcn.h.html).

## Usage

Provided there is a function `inc` that increments a number in a library
`libt.so`—and in fact this is exactly the example provided in the `examples`
directory—, you can look up and call this function at runtime as follows:

```clojure
(load "https://github.com/carpentry-org/dynlib@0.0.1")

(defn main []
  (println*
    &(=> (DynLib.open "libt.so")
         (Maybe.to-result @"Couldn’t open libt.so")
         (Result.and-then
           (fn [lib] (Maybe.to-result (DynLib.get lib "inc") @"Couldn’t load symbol inc")))
         (Result.map (fn [f] (Int.str (f 1)))))))
```

We’re using `Result` here to get informative error messages, but this is all
optional. If you want to throw safety out of the window, something like this
could also work—though I wholeheartedly advise against it:

```clojure
(load "https://github.com/carpentry-org/dynlib@0.0.1")

(defn main []
  (let [lib (Maybe.unsafe-from (DynLib.open "libt.so"))
        f (Maybe.unsafe-from (DynLib.get lib "inc"))]
    (println* &(Int.str (f 1)))))
```

This is equivalent to the above, but it will crash and burn if any of the
assumptions cannot be fulfilled. Stay safe, folks!

Both of the examples do not use `dlclose` to clean up the resource after
they are done—mostly for brevity’s sake—, but I’d really hope you’ll do that.
Do as I say, not as I do!

## Limitations

For now, the functions that are returned by `DynLib.get` are all unary,
meaning that they only take one argument. If you have an idea how to encode
function with a variable number of arguments in the type system, hit me up!

<hr/>

Have fun!
