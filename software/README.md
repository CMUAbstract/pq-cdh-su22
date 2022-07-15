# Command and Data Handling Board Software

Command and data handling board software

**Usage**

Change directories to one of the software folders, e.g. `blink`, and execute
`make`. The Makefile includes the rules.mk file, as well as libopencm3 and any
common ASM or C files. Be sure to source `sourcefile.txt` first.

```bash
# Example
cd $HOME/git-repos/pq-cdh-su22/scripts/
source sourcefile.txt
cd $HOME/git-repos/pq-cdh-su22/software/blink/
make
```

See the README in each program directory for details.

## Directory Contents

* [blink](blink/README.md): Makes the Tartan Artibeus EXPT board LEDs blink
* [rules.mk](rules.mk): Used by `make`
* [README.md](README.md): This document

## License

Written by Bradley Denby  
Other contributors: None

See the top-level LICENSE file for the license.
