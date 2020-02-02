# Using ImGui(with docking feature) with Magnum graphic library

1. edit `vcpkg/ports/imgui/profile.cmake`

		vcpkg_from_github(
		    OUT_SOURCE_PATH SOURCE_PATH
		    REPO ocornut/imgui
		    HEAD_REF docking
		)

2. install dependencies

		vcpkg install sdl2 --triplet x86-windows
		vcpkg install --head imgui --triplet x86-windows
		vcpkg install corrade[*] magnum[sdl2application] magnum-integration[imgui] --triplet x86-windows
		vcpkg install magnum-plugins[stbimageimporter] --triplet x86-windows
		vcpkg install stb:x86-windows-static
		
3. copy `imgui_internal.h` `imstb_textedit.h` under `vcpkg\buildtrees\imgui\src\v1.72b-bd3ca337ab` to `vcpkg/installed/x86-windows/include`

4. generate VS project with cmake(using vcpkg building toolchain)