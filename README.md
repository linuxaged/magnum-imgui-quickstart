# Using ImGui(with docking feature) with Magnum graphic library

## Build

* clone

		git clone https://github.com/linuxaged/magnum-imgui-quickstart.git
		git submodule update --init --recursive

* install dependencies

		vcpkg install glfw3 --triplet x64-windows

* generate VS project with cmake(using vcpkg building toolchain)
