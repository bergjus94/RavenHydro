# RavenHydroFramework (modified fork)

> **This is a modified fork, not the official Raven distribution.** For the
> official version see [the Raven website](https://raven.uwaterloo.ca/Main.html)
> and [CSHS-CWRA/RavenHydroFramework](https://github.com/CSHS-CWRA/RavenHydroFramework).
> The changes made here are listed under [Modifications in this fork](#modifications-in-this-fork).

The code repository for the Raven Hydrological Modelling Framework developed at the University of Waterloo.

Release versions, tutorials, documentation, and more distributed at [the Raven website](https://raven.uwaterloo.ca/Main.html).

Intended for use with Visual Studio Community Edition 2022, but also provided with Windows/linux/unix/MacOS g++ makefile and CMake configuration file.

Note unconventional two-space tabbing conventions.
If you would like to work with the active development of Raven's core, please do so on a branch and coordinate commits to the trunk with the Raven development team.

Please contact us while you're at it - we love to have people helping out.

## Modifications in this fork

Raven is distributed under the Artistic License 2.0, which this fork retains
(see `LICENSE`); the modifications below are identified as that licence
requires. Branched from upstream commit `9d82ff8` (`__RAVEN_VERSION__` 3.8.1).

**Lateral exchange between HRUs**
- `LatConnect.{cpp,h}` (new): explicit lateral connections between HRUs
- `.rvh` commands `:LateralConnections` / `:EndLateralConnections` and
  `:CheckConnectionWeights` (`ParseHRUFile.cpp`)
- new lateral flux type `LAT_REDISTRIBUTE` alongside the existing `LAT_FLUSH`
  and `LAT_EQUIL` (`RavenInclude.h`), with supporting changes in
  `Model.{cpp,h}`, `LateralExchangeABC.{cpp,h}` and `Solvers.cpp`

**Snow redistribution**
- `SnowRedistribution.cpp` (new): gravitational redistribution of snow between
  laterally connected HRUs, limited by a maximum snow height
- `.rvi` command `:SnowRedistribute [method] [SV] [Max_snow_height]`
  (`ParseInput.cpp`)

**Glacier representation**
- new HRU type `HRU_MASKED_GLACIER` (`MASKED_GLACIER` in the `.rvh` file), for
  catchments where glacier runoff is prescribed externally — here from GloGEM —
  rather than simulated
- masked-glacier HRUs are excluded from infiltration and behave like rock:
  nothing infiltrates, everything runs off (`Infiltration.cpp`)

**Evaporation and forcings**
- `SoilEvaporation.cpp`: the `stor/tens_stor` ratio is guarded against a zero
  tension storage, which previously produced a non-finite evaporation rate
- `UpdateForcings.cpp`: falls back to interpolating `PET`, `PET_month_ave` and
  `temp_month_ave` from the gauges when they are not otherwise available
- `Evaporation.cpp`: HBV PET temperature correction refactored into named
  locals; the arithmetic is unchanged

### Version used for Berg et al. (HESS)

The tag `paper1-hess` marks the exact model version used for all simulations in
Berg et al. (submitted to *Hydrology and Earth System Sciences*). This was
verified two independent ways:

- a CMake build of that state reproduces the `.text` section of the binary used
  for the published runs byte for byte (md5 `93c11f7475b0848b83e22f4da7dfff95`)
- rerunning catchment 2268 in the coupled / snow-redistribution /
  precipitation-correction configuration over 2000-2020 reproduces the stored
  `2268_HBV_Hydrographs.csv` exactly (md5 `ac8c128c89b388bcbbac3c71302f2980`,
  zero differing lines)

Model setups, calibrated parameters and simulation output are published at
https://doi.org/10.5281/zenodo.21774506; the analysis workflow is at
https://github.com/bergjus94/Raven-Switzerland.

## Building Raven

Raven is built using CMake. A suggested sequence of commands to build Raven executable file is:

```bash
mkdir build
cd build
cmake [OPTIONS] ../
make
```

The `cmake` command can be configured with the following optional arguments (```[OPTIONS]``` above):

* `-DCOMPILE_LIB` to build Raven as a library (default: `OFF`)
* `-DCOMPILE_EXE` to build Raven as an executable (default: `ON`)

So that the ```cmake``` command to build Raven as solely a dynamic library becomes:

```bash
cmake -DCOMPILE_LIB=ON -DCOMPILE_EXE=OFF ../
```
Raven can alternately be bullt in unix/MacOS using the makefile provided with the source code (g++ must be installed on the machine). Lastly, it may be compiled within Visual Studio Community Edition 2022.
