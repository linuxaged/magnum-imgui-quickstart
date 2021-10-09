#include "WindowJobs.h"
#include <imgui.h>
#include <Corrade/Utility/String.h>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/StaticArray.h>
#include "portable-file-dialogs.h"
#include "../Renderer.h"

const char* APP_ID = "110695970";
const char* APP_CODE = "329a1f78b20345ada0683a2fbed620e4";
const wchar_t* HOST = L"seg.deepalignws.com";
const wchar_t* SEGMENT_PATH = L"/segmentation/job";

extern Corrade::Containers::Pointer<Renderer> g_Renderer;

struct Job {
    double timestamp;
    std::string id;
};
std::vector<Job> g_Jobs;
double g_LastQueryTime;

void WindowJobs::show(const char* label) {
    ImGui::Begin(label);
    {
        if(ImGui::Button(u8"选择 STL 文件")) {
            
        }
    }
    ImGui::End();
}