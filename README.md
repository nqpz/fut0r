```
    mmmm                         mmmm
   ##"""               ##       ##""##
 #######   ##    ##  #######   ##    ##   ##m####
   ##      ##    ##    ##      ## ## ##   ##"
   ##      ##    ##    ##      ##    ##   ##
   ##      ##mmm###    ##mmm    ##mm##    ##
   ""       """" ""     """"     """"     ""
```

*Port of frei0r to the Futhark programming language*

# fut0r

fut0r is a port of the [frei0r](https://frei0r.dyne.org/) effects
library into the [Futhark](http://futhark-lang.org/) programming
language.

You can use it either as a Futhark package or a drop-in replacement for
frei0r.

Only a few effects have been ported.  Feel free to port some more!  Try
to follow the structure of the existing effect ports.


## Futhark use

Run:

```
futhark-pkg add github.com/nqpz/fut0r
futhark-pkg sync
```

Example use:

```
$ futharki
> import "/futlib/array"
> import "lib/github.com/nqpz/fut0r/effects"
> let (h, s, l) = (0.1f64, 0.7f64, 0.5f64)
  let grey_image = replicate 16 (replicate 16 0xff999999)
  in colorize h s l grey_image
```


## General use

Run:

```
git clone git@github.com:nqpz/fut0r.git
git submodule update --init
(cd frei0r && cmake . && make) # Check frei0r/INSTALL for dependencies
make
```

This will build frei0r.h-compatible .so files for all Futhark effects.
This can be used with programs like ffmpeg and Kdenlive, although it
hasn't really been tested.

This will also build docs for the Futhark filters.

Run `make test` to test that the Futhark ports return the same image as
the original plugin.  It generates a random image with random parameter
values, passes it to both the original frei0r plugin and the new
Futhark-using plugin, and compares the two outputs.


## Original authors

See https://github.com/dyne/frei0r/blob/master/AUTHORS for a list of the
original plugin authors.


## Authors

Niels G. W. Serup, ... (your name here)


## License

The fut0r source code is available under the same terms as that of
frei0r: The GNU General Public License, version 2 or later.
