#include "ui/Theme.hpp"

#include <imgui.h>

namespace activator::ui {

void ApplyTheme() {
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 8.0F;
  style.FrameRounding = 6.0F;
  style.ChildRounding = 8.0F;
  style.GrabRounding = 6.0F;
  style.ScrollbarRounding = 8.0F;
  style.ItemSpacing = ImVec2(10.0F, 10.0F);
  style.WindowPadding = ImVec2(16.0F, 16.0F);

  ImVec4* colors = style.Colors;
  colors[ImGuiCol_WindowBg] = ImVec4(0.09F, 0.1F, 0.12F, 1.0F);
  colors[ImGuiCol_ChildBg] = ImVec4(0.12F, 0.13F, 0.15F, 1.0F);
  colors[ImGuiCol_FrameBg] = ImVec4(0.16F, 0.17F, 0.2F, 1.0F);
  colors[ImGuiCol_Button] = ImVec4(0.2F, 0.35F, 0.65F, 1.0F);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.23F, 0.4F, 0.72F, 1.0F);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.16F, 0.29F, 0.55F, 1.0F);
  colors[ImGuiCol_Header] = ImVec4(0.17F, 0.25F, 0.38F, 1.0F);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.2F, 0.3F, 0.45F, 1.0F);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.15F, 0.2F, 0.32F, 1.0F);
}

}  // namespace activator::ui
