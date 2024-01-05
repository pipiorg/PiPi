# PiPi

A PDF tool to support acroform field fill with textbox/checkbox/image and fatten field for specific field name. The library depend on [PoDoFo](https://github.com/podofo/podofo).

## Requirement

- c++ 17+
- cmake 3.11+

## Configure

Configure the podofo dependency under CMakePresets.json - configurePresets/cacheVariables. You can find the needed dependency binary file from [PoDoPo-Deps](https://github.com/podofo/podofo-deps).

## Build

```bash
# Generate project
mkdir build
cd build
cmake ..
```

## Planing

- Clean up unused font feature
- Rename field feature

## License

PiPi Based on MIT license, licensed under [LICENSE](LICENSE)
