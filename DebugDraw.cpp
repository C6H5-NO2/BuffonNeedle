#include "DebugDraw.h"
#include "DebugDrawImpl.h"

namespace Needle {
    DebugDraw::DebugDraw() : mDebugMode(DBG_NoDebug),
                             mImpl(nullptr) {}

    DebugDraw::~DebugDraw() = default;

    void DebugDraw::awake() {
        mImpl.reset(new DebugDrawImpl);
        mImpl->awake();
    }

    void DebugDraw::render() {
        mImpl->render();
    }

    void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
        mImpl->pushLine(from, to, color);
    }
}
