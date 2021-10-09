#include "WindowMain.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <Corrade/Containers/Pointer.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/ImGuiIntegration/Widgets.h>
#include "../Renderer.h"
extern Corrade::Containers::Pointer<Renderer> g_Renderer;

void WindowMain::show(WindowMain::Param param) {
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if(dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    // DockSpace
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        static auto first_time = true;
        if(first_time) {
            first_time = false;

            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            // split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
            //   window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
            //                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
            auto dock_id_scene_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
            auto dock_id_scene_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.5f, nullptr, &dockspace_id);

            // we now dock our windows into the docking node we made above
            ImGui::DockBuilderDockWindow("SceneUp", dockspace_id);
            ImGui::DockBuilderDockWindow("Right", dock_id_scene_right);
            ImGui::DockBuilderDockWindow("SceneDown", dock_id_scene_down);
            ImGui::DockBuilderFinish(dockspace_id);
        }
    }

    ImGui::End();

    ImGui::Begin("SceneUp");
    {
        static float lastX, lastY;
        param.sceneHovered = ImGui::IsWindowHovered();
        if(param.sceneHovered) {
            ImGui::GetIO().WantCaptureMouse = false;
        } else {
            ImGui::GetIO().WantCaptureMouse = true;
        }
        auto maxSize = ImGui::GetWindowContentRegionMax();
        auto minSize = ImGui::GetWindowContentRegionMin();
        auto x = maxSize.x - minSize.x;
        auto y = maxSize.y - minSize.y;
        if(x != lastX)
            lastX = x;
        if(y != lastY)
            lastY = y;
        Magnum::ImGuiIntegration::image(g_Renderer->getTexture(), Magnum::Vector2{x, y});
    }
    ImGui::End();

    _windowJobs.show("Right");

    ImGui::Begin("SceneDown");
    ImGui::Text("Hello, down!");
    ImGui::End();
}
