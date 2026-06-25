# Mi-Ni File System

## Check Out Source Code

Clone MNFS repository from Github:

    git clone https://github.com/MengXi47/MNFS

When `MengXi47/MNFS` has been cloned to a local file system, run the
following commands to check out the submodules:

```bash
cd MNFS
git submodule update --init --recursive
./patches/apply.sh
```

## Build MNFS

Build MNFS in `build` folder:

```bash
cmake -S . -B build \
      -DCMAKE_CXX_COMPILER=clang++-14 -DCMAKE_C_COMPILER=clang-14 \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -DSHUFFLE_METHOD=<method>
cmake --build build -j
```