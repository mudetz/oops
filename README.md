# OOPS
Optimistic Orienteering Problem Solver for the Arc Orienteering Problem using Particle Swarm Optimization.

## Requirements

- GNU Make
- GNU G++ capable of compiling C++17 standard
- Boost Libs

## Compilation

```
# Compile to 'oops' executable
make

# Remove intermediate .obj files
make clean

# Remove final executable 'oops'
make distclean
```

## Execution
```
./oops < instance.txt
```
Test instances are included in `test/`

### Parameters

See available parameters with

```
./oops --help
```

### Example execution
```
./oops --verbose \
       --mst \
       --swarm-size 100 \
       --iterations 100 \
       --cognitive-factor 0.7 \
       --social-factor 0.3 \
       --threads 4
```

## Instances structure

- First line contains `Cmin` (Minimum solution cost)
- Second line contains `Cmax` (Maximum solution cost)
- Third line contains `V` (Number of vertices, which are implicitly identified with numbers from 1 to `V` inclusive)
- Fourth line contains `E` (Number of edges, or arcs)
- Fifth line contains `S0` (Starting and vertex id)
- Then E lines follow, containing each arc with the following values, separated by a tab `\t`
    - Arc id
    - Arc starting vertex (from)
    - Arc ending vertex (to)
    - Arc cost
    - Arc profit

## Licence
This program is licensed & distributed under the GNU GPL3.0 license. See [`COPYING`](COPYING) for details.
