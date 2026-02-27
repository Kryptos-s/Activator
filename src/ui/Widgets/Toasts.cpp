#include "ui/Widgets/Toasts.hpp"

#include <imgui.h>

namespace activator::ui {

void DrawToasts(const std::vector<ToastMessage>& toasts) {
  ImVec2 pos = ImVec2(ImGui::GetIO().DisplaySize.x - 320.0F, 20.0F);
  for (std::size_t i = 0; i < toasts.size(); ++i) {
    ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + static_cast<float>(i) * 72.0F));
    ImGui::SetNextWindowSize(ImVec2(300.0F, 60.0F));
    ImGui::Begin(("Toast##" + std::to_string(i)).c_str(), nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::TextWrapped("%s", toasts[i].text.c_str());
    ImGui::End();
  }
}

}  // namespace activator::ui
