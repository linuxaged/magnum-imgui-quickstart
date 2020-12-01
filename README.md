# Using ImGui(with docking feature) with Magnum graphic library

0. edit `vcpkg/ports/imgui/profile.cmake`

		vcpkg_from_github(
		    OUT_SOURCE_PATH SOURCE_PATH
		    REPO linuxaged/imgui
		    HEAD_REF docking+tables
		)

1. edit `vcpkg/ports/magnum/portfile.cmake`

		vcpkg_configure_cmake(
		    SOURCE_PATH ${SOURCE_PATH}
		    PREFER_NINJA # Disable this option if project cannot be built with Ninja
		    OPTIONS
		        ${FEATURE_OPTIONS}
		        -DBUILD_STATIC=${BUILD_STATIC}
		        -DBUILD_PLUGINS_STATIC=${BUILD_PLUGINS_STATIC}
		        -DMAGNUM_PLUGINS_DEBUG_DIR=${CURRENT_INSTALLED_DIR}/debug/bin/magnum-d
		        -DMAGNUM_PLUGINS_RELEASE_DIR=${CURRENT_INSTALLED_DIR}/bin/magnum
		    +   -DTARGET_DESKTOP_GLES=ON
		)

2. install dependencies

		vcpkg install glfw3 --triplet x64-windows
		vcpkg install --head imgui --triplet x64-windows
		vcpkg install --head corrade[*] magnum[glfwapplication] magnum-integration[imgui] --triplet x64-windows
		vcpkg install --head magnum-plugins[stlimporter,stbimageimporter] --triplet x64-windows
		vcpkg install stb:x64-windows-static

3. generate VS project with cmake(using vcpkg building toolchain)
