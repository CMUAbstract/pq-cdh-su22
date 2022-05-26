# Notes for KiCad

* Install the correct version.

```bash
sudo add-apt-repository ppa:kicad/kicad-6.0-releases
sudo apt update
sudo apt install kicad
```

* Open KiCad. If prompted, start with default settings.

* Choose `File > New Project from Template...`. Select the project location.
  **Un-check "Create a new directory for the project" (bottom left)** and type
  the project name in the top text box (here: `pq-cdh`). Choose "Save."

* Launch the Schematic Editor. If prompted, accept "Copy default global symbol
  library table (recommended)." Choose `File > Page Settings...` and select
  "Size: USLetter 8.5x11in" if not already selected. Choose `File > Save`.

* Open the Symbol Editor by choosing `Tools > Symbol Editor` from the Schematic
  Editor.

* Using the Symbol Editor, choose `File > New Library...`. Choose the "Project"
  scope when prompted. Use the project name (here: `pq-cdh`), and choose "Save."

* Import the BOM symbols provided by `componentsearchengine.com` (free account
  required). Select `File > Import Symbol...` and save in the project library
  (be sure to select the project library for symbol import). Copies of the
  `.lib` files should be placed in the `components/` directory.

* Open the Footprint Editor. Accept "Copy default global footprint library table
  (recommended)" if prompted. Choose `File > New Library...`. Choose the
  "Project" scope when prompted. Use the project name (here: `pq-cdh`), and
  choose "Save."

* Import the BOM footprints from `componentsearchengine.com` into the project
  footprint library. Right-click on the footprint library, and select
  `Import Footprint...`. Copies of the `.kicad_mod` files should be placed in
  the `footprints/` directory. Choose `File > Save`, select the footprint
  library, and choose "Save."

* For each footprint, associate the corresponding 3D model from the `3d-models/`
  directory. Choose `File > Footprint Properties...` and specify the path to the
  3D model file under the 3D Settings tab using the folder icon. Save changes.

* Lay out the schematic in the Schematic Editor. Set the appropriate label
  values.

* In the Schematic Editor, select `Tools > Annotate Schematic...` and choose
  "Entire schematic;" "Keep existing annotations;" "Sort symbols by X position;"
  and "Use first free number after 0."

* Also in the Schematic Editor, choose `Tools > Assign Footprints...` and set
  the footprints.

* Specify footprints that cannot be set automatically, e.g.:
  `Conn_01x04 : pq:PinSocket_4x1_Pocketqube`, and
  `Conn_01x16 : pq:PinSocket_1x16_Pocketqube`, and
  `Conn_01x04 : pq:MountingHoles_Pocketqube`

* Open the PCB Editor and choose `File > Page Settings...` to select
  "Size: USLetter 8.5x11in" if not already selected. Choose `File > Save`.

* Choose `File > Board Setup...` and under "Layers," choose "Four layers, parts
  on Front & Back." Enable the following layers: `F.CrtYd`, `F.Fab`, `F.Paste`,
  `F.SilkS`, `F.Mask`, `F.Cu`, `In1.Cu`, `In2.Cu`, `B.Cu`, `B.Mask`, `B.SilkS`,
  `B.Paste`, `B.Fab`, `B.CrtYd`, `Edge.Cuts`, and `Margin`. For simplicity, set
  `In1.Cu` and `In2.Cu` to power layers and keep the other two layers as signal
  layers. Set the PCB thickness to 1.742 mm.

* Choose `Preferences > Preferences...`. Under `Pcbnew`, set Units to Inches.

* Choose `File > Board Setup...`. Under `Design Rules`, choose
  `Solder Mask/Paste`. Set "Solder mask clearance" to 0.0005 in (i.e. 0.5 mils
  or 0.0127 mm), and set all other values to zero.

* Still under `File > Board Setup...`, choose `Design Rules`. Ensure that "Allow
  blind/buried vias" is **not** checked. Ensure that "Allow micro vias (uVias)"
  is **not** checked. Set the following minimum values:
  * Minimum track width: 5 mils
  * Minimum via diameter: 18 mils
  * Minimum via drill: 10 mils
  * Minimum uVia diameter: 18 mils
  * Minimum uVia drill: 10 mils
  * Minimum hole to hole: 5 mils

* Still under `File > Board Setup...`, choose `Net Classes` under
  `Design Rules`. Double-click at the top of the entry area under "Net Classes"
  and edit the Default values:
  * Clearance: 5 mils
  * Track Width: 5 mils
  * Via Size: 18 mils
  * Via Drill: 10 mils
  * uVia Size: 18 mils
  * uVia Drill: 10 mils
  * dPair Width: 5 mils
  * dPair Gap: 5 mils

* Still under `File > Board Setup...`, choose `Tracks & Vias` under
  `Design Rules`. Add an 8 mils Tracks Width (for VDD and GND).

* Choose `Preferences > Preferences...`. Under `Pcbnew`, set Units to
  Millimeters.

* Add the PCB outline with graphic lines and graphic arcs.

**Graphic Lines for PCB Edge**

Create the following in the Edge.Cuts layer:
* Start: (136.04, 86.50); End: (173.96, 86.50)
* Start: (136.04,129.50); End: (173.96,129.50)
* Start: (133.50, 89.04); End: (133.50,126.96)
* Start: (176.50, 89.04); End: (176.50,126.96)

**Graphic Arcs for PCB Edge**

Create the following in the Edge.Cuts layer:
* Center: (136.04, 89.04); Start: (136.04, 86.50); Arc angle -90.0
* Center: (173.96, 89.04); Start: (176.50, 89.04); Arc angle -90.0
* Center: (136.04,126.96); Start: (133.50,126.96); Arc angle -90.0
* Center: (173.96,126.96); Start: (173.96,129.50); Arc angle -90.0

* In the Schematic Editor, choose `File > Export > Netlist...` and select
  "Export Netlist."

* In the PCB Editor, choose `File > Import > Netlist...` and browse to the
  appropriate file.

* Place the components, tracks, ground plane, and silkscreen labels.

* Record component characteristics:
  * J1:   (155.00,  88.95) @   0.0 (Example 1)
  * J2:   (155.00, 127.05) @   0.0 (Example 2)
  * J3:   (135.95, 120.70) @ 180.0 (Example 3)
  * J4:   (170.24, 108.00) @ 180.0 (Example 4)
  * J5:   (155.00, 108.00) @   0.0 (Example 5)

* Record via positions:
  * GND: (137.30, 111.30) (Example)

* Ground and power planes (Add filled zones):
  * Clearance: 0.254 mm
  * Minimum width: 0.127 mm
  * Thermal clearance: 0.254 mm
  * Thermal spoke width: 0.254 mm

* Silkscreen labels (Width: 0.5 mm; Height: 0.5 mm; Thickness: 0.1 mm):

## Generating Gerbers

* Ensure that fill zones have been redrawn, e.g. by running DRC (the bug icon)
* Choose `File > Plot...` from the PCB Editor
  * Output directory: ../gerbers/
  * Included Layers: F.Cu, B.Cu, F.Silkscreen, B.Silkscreen, F.Mask, B.Mask,
    Edge.Cuts
  * General Options: Plot footprint values, Plot reference designators
  * Gerber Options: Generate Gerber job file
* Choose "Plot"
* Choose "Generate Drill File"
  * Output folder: ../gerbers/
  * Drill File Format: Excellon, PTH and NPTH in single file, Oval Holes Drill
    Mode: Use route command (recommended)
  * Map File Format: PostScript
  * Dill Origin: Absolute
  * Drill Units: Inches
  * Zeros Format: Decimal format (recommended)
* Choose "Generate Drill File"
