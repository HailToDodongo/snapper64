# Snapper64
Self-testing, self-contained, snapshot based RDP test-ROM.<br>

# Usage

Get the latest ROM from the releases page, and start it in your emulator or on console.<br>
To run all tests, just press `A` in the menu.<br>
Once finished, you can view the individual and total results in the menu.<br>

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