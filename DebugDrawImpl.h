#pragma once

#include "glUtils.hpp"
#include <memory>
#include <vector>

struct SDL_Window;
class btVector3;

namespace Needle {
    class DebugDrawImpl final {
    public:
        DebugDrawImpl();
        ~DebugDrawImpl();
        void pushLine(const btVector3& _from, const btVector3& _to, const btVector3& _color);
        void awake();
        void render();

    private:
        std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> mWindow;
        std::unique_ptr<glUtils::Shader::Program> mProgram;
        std::unique_ptr<glUtils::VertexArray> mVAO;
        std::unique_ptr<glUtils::VertexBuffer> mVBO;
        std::vector<float> mNewData;
    };
}
