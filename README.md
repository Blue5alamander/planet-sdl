# Planet SDL

[![Documentation](https://badgen.net/static/docs/blue5alamander.com)](https://blue5alamander.com/open-source/planet-sdl/)
[![GitHub](https://badgen.net/badge/Github/planet-sdl/green?icon=github)](https://github.com/Blue5alamander/planet-sdl/)
[![License](https://badgen.net/github/license/Blue5alamander/planet-sdl)](https://github.com/Blue5alamander/planet-sdl/blob/main/LICENSE_1_0.txt)
[![Discord](https://badgen.net/badge/icon/discord?icon=discord&label)](https://discord.gg/tKSabUa52v)

Provides some basic C++ wrappers around SDL2 APIs to make them a bit more convenient to use.


## Development requirements

On Linux you'll need some dev packages installed:

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev
```

On Windows it's generally best to add these as submodules to your project and use `add_subdirectory` from cmake to build them.
