#include "ui/Widgets/Spinner.hpp"

#include <imgui.h>

#include <cmath>

namespace activator::ui {

void DrawSpinner(float radius, float thickness, int segments) {
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  const ImVec2 pos = ImGui::GetCursorScreenPos();
  const float time = static_cast<float>(ImGui::GetTime());
  const float start = time * 8.0F;
  const float end = start + IM_PI * 1.5F;

  drawList->PathClear();
  for (int i = 0; i < segments; ++i) {
    const float t = static_cast<float>(i) / static_cast<float>(segments - 1);
    const float a = start + (end - start) * t;
    drawList->PathLineTo(
        ImVec2(pos.x + radius + std::cos(a) * radius, pos.y + radius + std::sin(a) * radius));
  }
  drawList->PathStroke(IM_COL32(90, 170, 255, 255), false, thickness);
  ImGui::Dummy(ImVec2(radius * 2, radius * 2));
}

}  // namespace activator::ui
