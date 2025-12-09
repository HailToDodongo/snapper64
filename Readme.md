# Snapper64
Self-testing, self-contained, snapshot based RDP test-ROM.<br>

<img src="docs/menu00.png" width="320">
<img src="docs/test00.png" width="320">
<img src="docs/test01.png" width="320">
<img src="docs/test02.png" width="320">

# Usage

Get the latest ROM from the releases page, and start it in your emulator or on console.<br>
To run all tests, just press `A` in the menu.<br>
Once finished, you can view the individual and total results in the menu.<br>

In the menu you can also select individual tests to run using the `C-Up` and `C-Down` buttons.<br>
If you do so, it will save the selected test in SRAM, so the next time you boot the ROM, it will automatically be on that test.<br>

While running a test, you can press `Start` to abort and return to the menu.<br>

## Settings

By pressing `L/R` you can toggle into the options tab.<br>
Here you can change the following settings:

<img src="docs/settings00.png" width="320">

- Advance-Tests: either auto-run through all tests in a group, or manually advance.
- Dump-Method: when dumping tests, choose between SD-Card or USB-Log / ISViewer.

Settings chosen here are also saved.

## Inspecting Tests and Errors

When a test fails, you may want to inspect the exact differences.<br>
For that first enable the `L/R to Step` option in the settings.<br>
Then go to the test and start it as usual with `A`.<br>

You will now see a different screen and stay on a test:

<img src="docs/test02.png" width="320">

Pressing `L` or `R` will go to the previous or next test.<br>
Holding `Z` will quickly advance the tests.<br>
The small letters above the rectangle containing the test shows the internal name of the dumped file.<br>

The way buffers are shown on screen can be changed in this mode by pressing `C-Left` or `C-Right`.<br>
Available modes are:
- Actual (Col)
- Diff (Col)
- Ref (Col)
- Actual (CVG)
- Diff (CVG)
- Ref (CVG)

As the names suggest, this lets you view the drawn image, a diff-view, or the reference image.<br>
Both with either color or coverage values.

While in diff view, wrong pixels are highlighted in white and red, whereas black pixels are correct.<br>
- White: reference pixel was black, drawn pixel is not (suggest extra pixels drawn)
- Red: all other wrong pixel (suggest missing/wrong pixels)

# Building

Make sure you have the [libdragon SDK](https://github.com/DragonMinded/libdragon) setup, using the latest `preview` branch.<br>
You also need 7zip installed, and the `7z` command available.<br>

After that, checkout this repo and run make, ideally with multiple jobs:
```bash
make -j10
```

Note that the initial build can take a long time, due to heavy use of compression for the test assets.<br>
Subsequent builds will be much faster, as only changed assets will be recompressed.<br>

For context: the raw uncompressed files at the time of writing are around 670MB, while the final ROM is around 4MB.<br>
The repo stores those as `.7z`, but in order to be usable they need to be recompressed during the build.<br>
On my laptop with an `AMD Ryzen 7 7435HS` at `-j10`, this can take ~40min.

Once the build has finished, a ROM named `snapper64.z64` will be created in the root of the repo.<br>

# Writing Tests

@TODO