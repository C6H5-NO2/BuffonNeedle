#pragma once

#include <bullet3/LinearMath/btIDebugDraw.h>
#include <memory>

namespace Needle {
    class DebugDrawImpl;

    class DebugDraw final : public btIDebugDraw {
    public:
        DebugDraw();
        ~DebugDraw();
        void awake();
        void render();
        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override { drawLine(PointOnB, PointOnB + normalOnB * distance, color); }
        void reportErrorWarning(const char* warningString) override { puts(warningString); }
        void draw3dText(const btVector3& location, const char* textString) override { puts(textString); }
        void setDebugMode(int debugMode) override { mDebugMode = debugMode; }
        int getDebugMode() const override { return mDebugMode; }

    private:
        int mDebugMode;
        std::unique_ptr<DebugDrawImpl> mImpl;
    };
}
