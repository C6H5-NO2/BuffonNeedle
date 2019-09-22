#include "DebugDrawImpl.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <bullet3/LinearMath/btVector3.h>
#include <SDL2/SDL_video.h>

namespace Needle {
    DebugDrawImpl::DebugDrawImpl() : mWindow(nullptr, [](SDL_Window* _w) { if(_w) SDL_DestroyWindow(_w); }),
                                     mProgram(nullptr),
                                     mVAO(nullptr),
                                     mVBO(nullptr) {}

    DebugDrawImpl::~DebugDrawImpl() = default;

    void DebugDrawImpl::pushLine(const btVector3& _from, const btVector3& _to, const btVector3& _color) {
        // btVector3::m_floats[4]
        mNewData.insert(mNewData.end(), _from.m_floats, _from.m_floats + 3);
        mNewData.insert(mNewData.end(), _color.m_floats, _color.m_floats + 3);
        mNewData.insert(mNewData.end(), _to.m_floats, _to.m_floats + 3);
        mNewData.insert(mNewData.end(), _color.m_floats, _color.m_floats + 3);
    }

    void DebugDrawImpl::awake() {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        mWindow.reset(SDL_CreateWindow("Buffon's Needle Simulation    by C6H5-NO2",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       800,
                                       600,
                                       SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL));

        SDL_GL_CreateContext(mWindow.get());

        gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress));

        std::pair<int, int> windowSize;
        SDL_GL_GetDrawableSize(mWindow.get(), &windowSize.first, &windowSize.second);
        glViewport(0, 0, windowSize.first, windowSize.second);

        glEnable(GL_DEPTH_TEST);

        puts((char*)glGetString(GL_VERSION));
        puts((char*)glGetString(GL_RENDERER));

        mProgram.reset(new glUtils::Shader::Program);
        {
            using namespace glUtils;
            Shader::Source vert(Shader::vertShader, GL_VERTEX_SHADER);
            vert.compile();
            Shader::Source frag(Shader::fragShader, GL_FRAGMENT_SHADER);
            frag.compile();
            mProgram->attachShader(vert);
            mProgram->attachShader(frag);
            mProgram->link();
        }

        mVAO.reset(new glUtils::VertexArray);
        mVBO.reset(new glUtils::VertexBuffer);
        mVAO->bind();
        mVBO->bind();
        glUtils::linkVertexAttributes(0, 3, 6, 0); // position
        glUtils::linkVertexAttributes(1, 3, 6, 3); // color
        mVBO->unbind();
        mVAO->unbind();

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glClearColor(.3f, 0, .4f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void DebugDrawImpl::render() {
        if(mNewData.empty()) return;

        glClearColor(.08f, .58f, .87f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mProgram->useSelf();

        {
            using namespace glm;
            auto identity = mat4x4(1),
                 model = identity,
                 view = lookAt(vec3(0, 23, 30), {0, 0, 0}, {0, 1, 0}),
                 projection = perspective(radians(75.f), 800.f / 600.f, .1f, 100.f);
            mProgram->setUniform("model", model);
            mProgram->setUniform("view", view);
            mProgram->setUniform("projection", projection);
        }

        mVAO->bind();
        mVBO->fillData(mNewData, GL_DYNAMIC_DRAW);
        //glUtils::linkVertexAttributes(0, 3, 6, 0);
        //glUtils::linkVertexAttributes(1, 3, 6, 3);
        mVBO->unbind();
        glDrawArrays(GL_LINES, 0, mNewData.size() / 6);
        mVAO->unbind();

        mNewData.clear();

        SDL_GL_SwapWindow(mWindow.get());
    }
}
