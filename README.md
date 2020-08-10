# Using ImGui(with docking feature) with Magnum graphic library

1. edit `vcpkg/ports/imgui/profile.cmake`

		vcpkg_from_github(
		    OUT_SOURCE_PATH SOURCE_PATH
		    REPO linuxaged/imgui
		    HEAD_REF docking+tables
		)

2. install dependencies

		vcpkg install glfw3 --triplet x64-windows
		vcpkg install --head imgui --triplet x64-windows
		vcpkg install --head corrade[*] magnum[glfwapplication] magnum-integration[imgui] --triplet x64-windows
		vcpkg install --head magnum-plugins[stlimporter,stbimageimporter] --triplet x64-windows
		vcpkg install stb:x64-windows-static

3. generate VS project with cmake(using vcpkg building toolchain)
