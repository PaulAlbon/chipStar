# chipStar

chipStar enables porting HIP and CUDA applications to platforms which support
SPIR-V as the device intermediate representation. It supports
OpenCL and Level Zero as the low-level runtime alternatives.

* [User documentation](docs/Using.md)
* [Developer documentation](docs/Development.md)
* [A list of (un)supported features](docs/Features.md)

chipStar was initially built by combining the prototyping work done in the (now obsolete) [HIPCL](https://github.com/cpc/hipcl) and
[HIPLZ](https://github.com/jz10/anl-gt-gpu/) projects.

If you wish to cite chipStar in academic publications, please refer to the [HIPCL poster abstract](https://dl.acm.org/doi/10.1145/3388333.3388641) when discussing the OpenCL backend and/or the [HIPLZ conference paper](https://link.springer.com/chapter/10.1007/978-3-031-31209-0_15) when mentioning the Level Zero backend. The core developers of chipStar are writing a proper article of the integrated chipStar project, but it is in progress.

The name chipStar comes from `c`uda and `hip` and the word `Star` which means asterisk, a typical shell wildcard, denoting the intention to make "CUDA and HIP applications run everywhere". The project was previously called CHIP-SPV.

## Development Status and Maturity

While chipStar 1.0 can already be used to run various large HPC applications successfully, it is still heavily in development mode with plenty of known issues and unimplemented features. Most importantly, there are various low-hanging performance optimizations that are still to be done; the development work so far has focused on functional correctness, not on performance issues. However, chipStar is ready for wider-range testing and we welcome community contributions in form of reproducible bug reports and good quality pull requests.

## Prerequisites

* Cmake >= 3.20.0
* Clang and LLVM 16 (LLVM 15 might also work)
  * Can be installed, for example, by adding the [LLVM's Debian/Ubuntu repository](https://apt.llvm.org/) and installing packages 'clang-16 llvm-16 clang-tools-16'.
  * For the best results, install it from a chipStar LLVM/Clang [branch](https://github.com/CHIP-SPV/llvm-project/tree/chipStar-llvm-16) which has fixes that are not yet in the LLVM upstream project.
* SPIRV-LLVM-Translator from a branch matching the LLVM major version:
  (e.g. llvm\_release\_160 for LLVM 16)
  [llvm-spirv](https://github.com/KhronosGroup/SPIRV-LLVM-Translator).
  * Make sure the built llvm-spirv binary is installed into the same path as clang binary, otherwise clang might find and use a different llvm-spirv, leading to errors.
  * For the best results, install it from a chipStar [branch](https://github.com/CHIP-SPV/SPIRV-LLVM-Translator/tree/chipStar-llvm-16) which has fixes that are not yet upstreamed.

### OpenCL Backend

  * An OpenCL 2.0 or 3.0 driver with at least the following features supported:
    * Coarse-grained buffer Shared Virtual Memory
    * Generic address space
    * SPIR-V input
    * Program scope variables
  * Further OpenCL extensions or features might be needed depending on the compiled CUDA/HIP application. For example, to support warp-primitives, the OpenCL 3.0 driver should support also some subgroup features such as shuffles, ballots and [cl_intel_required_subgroup_size]( https://registry.khronos.org/OpenCL/extensions/intel/cl_intel_required_subgroup_size.html).

### Level Zero Backend

  * [Intel Compute Runtime](https://github.com/intel/compute-runtime) or [oneAPI](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html)
  * [oneAPI Level Zero Loader](https://github.com/oneapi-src/level-zero/releases)
* For HIP-SYCL and HIP-MKL Interoperability: [oneAPI](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html)

## Compiling Clang, LLVM and SPIRV-LLVM-Translator

It's recommended to use the chipStar forks of LLVM and SPIRV-LLVM-Translator.
```bash
git clone --depth 1 https://github.com/CHIP-SPV/llvm-project.git -b chipStar-llvm-16
cd llvm-project/llvm/projects
git clone --depth 1 https://github.com/CHIP-SPV/SPIRV-LLVM-Translator.git -b chipStar-llvm-16

# DLLVM_ENABLE_PROJECTS="clang;openmp" OpenMP is optional but many apps use it
# DLLVM_TARGETS_TO_BUILD Speed up compilation by building only the necessary CPU host target
# CMAKE_INSTALL_PREFIX Where to install LLVM

cmake -S llvm -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_PROJECTS="clang;openmp" \
  -DLLVM_TARGETS_TO_BUILD=X86 \
  -DCMAKE_INSTALL_PREFIX=$HOME/local/llvm-16
make -C build -j8 all install
```

## Downloading Sources

You can download and unpack the latest release source package or clone the development branch via git. We aim to keep the main development branch stable, but it might have stability issues during the development cycle. To clone the sources from Github:

```bash
git clone https://github.com/CHIP-SPV/chipStar.git
cd chipStar
git submodule update --init --recursive
```

## Building and Installing

```bash
mkdir build && cd build

# LLVM_CONFIG_BIN is optional if LLVM is not installed in PATH or if only a version-sufficed
# binary is available (for example, llvm-config-16)

cmake .. \
    -DLLVM_CONFIG_BIN=/path/to/llvm-config
    -DCMAKE_INSTALL_PREFIX=/path/to/install
make all build_tests install -j8
```

NOTE: If you don't have libOpenCL.so (for example from the `ocl-icd-opencl-dev` package), but only libOpenCL.so.1 installed, CMake fails to find it and disables the OpenCL backend. This [issue](https://github.com/CHIP-SPV/chipStar/issues/542) describes a workaround.

## Running Unit Tests

There's a script `check.py` which can be used to run unit tests and which filters out known failing tests for different platforms. Its usage is as follows.

```bash
# BACKEND={opencl/level0/pocl}   # Which backend/driver you wish to test, "opencl" = Intel OpenCL runtime, "level0" = Intel LevelZero runtime, "pocl" = PoCL OpenCL runtime
# DEVICE={cpu,igpu,dgpu}         # What kind of device to test.
# PARALLEL={N}                   # How many tests to run in parallel.
# export CHIP_PLATFORM=N         # If there are multiple OpenCL platforms present on the system, selects which one to use

python3 $SOURCE_DIR/scripts/check.py $BUILD_DIR $DEVICE $BACKEND $PARALLEL 1
```

Please refer to the [user documentation](docs/Using.md) for instructions on how to use the installed chipStar to build CUDA/HIP programs.

## Troubleshooting

### Clang++ Cannot Find libstdc++ When Building chipStar

This occurs often when the latest installed GCC version doesn't include libstdc++, and Clang++ by default chooses the latest found one regardless, and ends up failing to link C++ programs. The problem is discussed [here](https://discourse.llvm.org/t/add-gcc-install-dir-deprecate-gcc-toolchain-and-remove-gcc-install-prefix/65091/14).

The issue can be resolved by defining a Clang++ [configuration file](https://clang.llvm.org/docs/UsersManual.html#configuration-files) which forces the GCC to what we want. Example:

```bash
echo --gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/11 > ~/local/llvm-16/bin/x86_64-unknown-linux-gnu-clang++.cfg
```

### Missing Double Precision Support

When running the tests on OpenCL devices which do not support double precision floats,
there will be multiple tests that will error out.

It might be possible to enable software emulation of double precision floats for
Intel iGPUs by setting [two environment variables](https://github.com/intel/compute-runtime/blob/master/opencl/doc/FAQ.md#feature-double-precision-emulation-fp64) to make kernels using doubles work but with the major
overhead of software emulation:

```bash
export IGC_EnableDPEmulation=1
export OverrideDefaultFP64Settings=1
```
