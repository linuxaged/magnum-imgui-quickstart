#pragma once

#include "WindowJobs.h"
// TODO: 很奇怪，这个引用必须放在头文件里
#include <Magnum/GL/Texture.h>

class WindowMain {
public:
	struct Param {
		bool& sceneHovered;
	};
	void show(Param param);
private:
	WindowJobs _windowJobs;
};