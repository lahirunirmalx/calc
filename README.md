# calc

Simple four-function calculators in C: two GTK+ 3 front ends (Glade UI) and one SDL2 front end, sharing [`calc_core.c`](calc_core.c) / [`calc_core.h`](calc_core.h) for numeric parsing and calculator phase state (`CalcPhase`).

| Binary      | Source        | UI                          |
|------------|---------------|-----------------------------|
| `calc`     | `calc.c`      | GTK+ 3 + `builder.ui`       |
| `calc2`    | `calc2.c`     | GTK+ 3 + `builder.ui` (state in a struct) |
| `calc_sdl` | `calc_sdl.c`  | SDL2 + SDL2_ttf (no UI file) |

This is a small learning project, not a production calculator (no claim of full numerical robustness or memory-safety hardening).

## Dependencies

**Debian / Ubuntu (typical):**

```sh
sudo apt install build-essential libgtk-3-dev libsdl2-dev libsdl2-ttf-dev
```

- **GTK builds:** need `libgtk-3-dev` and a working `pkg-config` for `gtk+-3.0`.
- **SDL build:** need `libsdl2-dev`, `libsdl2-ttf-dev`, and fonts such as DejaVu or Liberation under `/usr/share/fonts/...` (TTF); the program tries a few common paths at startup.

## Build

From the project root:

```sh
make
```

Produces `calc`, `calc2`, and `calc_sdl`. To remove the binaries:

```sh
make clean
```

## Run

- **GTK:** run from the directory that contains `builder.ui`, or the UI file will not load (paths are relative).

  ```sh
  ./calc
  # or
  ./calc2
  ```

- **SDL:** no extra assets besides fonts on the system.

  ```sh
  ./calc_sdl
  ```

## Clone

```sh
git clone git@github.com:lahirunirmalx/calc.git
cd calc
```

## License

See [LICENSE](LICENSE).
