#include "render_context.hpp"

bool RenderContext::init() { return true; }

RenderDrawList& RenderContext::getRenderDrawList() { return renderDrawList; }

RenderCameraInfo& RenderContext::getRenderCameraInfo() {
    return renderCameraInfo;
}

RenderCommandList& RenderContext::getRenderCommandList() {
    return renderCommandList;
}