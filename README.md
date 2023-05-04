# xrnBoidsSim
[![Continous Integration](https://github.com/DiantArts/xrnBoidsSim/actions/workflows/continuous-integration.yml/badge.svg)](https://github.com/DiantArts/xrnBoidsSim/actions/workflows/continuous-integration.yml)

# Video example of 25'000 boids:
[![Video example](https://i3.ytimg.com/vi/v1IAG6-MOts/maxresdefault.jpg)](https://youtu.be/v1IAG6-MOts)

# How to run
Run the following commands to install the dependencies (ubuntu-22.04 example):
```
apt-get install -y python3-pip cppcheck clang-tidy ccache moreutils libxvmc-dev sfml cmake gcovr llvm
```
```
pip install conan==1.59 ninja
```

Launch the simulation
```
./.toolchain/compile
```

Use `-d` to enable debug.
Use `-h` for help.
