#pragma once

#include "DebugDraw.h"
#include <bullet3/btBulletDynamicsCommon.h>
#include <set>

namespace Needle {
    class World final {
    public:
        World(): mConfig(nullptr),
                 mDispatcher(nullptr),
                 mBroadphase(nullptr),
                 mSolver(nullptr),
                 mWorld(nullptr),
                 mDebugDraw(nullptr),
                 mPaused(false) {}

        ~World() {
            auto& colobjs = mWorld->getCollisionObjectArray();
            for(auto i = 0; i < colobjs.size(); ++i) {
                auto rb = btRigidBody::upcast(colobjs[i]);
                mWorld->removeRigidBody(rb);
                delete rb->getMotionState();
                delete rb;
            }
            for(auto collider : mColliders)
                delete collider;
            mColliders.clear();
            mWorld->setDebugDrawer(nullptr);
            delete mDebugDraw;
            delete mWorld;
            delete mSolver;
            delete mBroadphase;
            delete mDispatcher;
            delete mConfig;
        }

        void awake() {
            mConfig = new btDefaultCollisionConfiguration;
            mDispatcher = new btCollisionDispatcher(mConfig);
            mBroadphase = new btDbvtBroadphase;
            mSolver = new btSequentialImpulseConstraintSolver;
            mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mConfig);

            mWorld->setGravity(btVector3(0, -9.8f, 0));

            mDebugDraw = new DebugDraw;
            mDebugDraw->awake();
            mDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
            mWorld->setDebugDrawer(mDebugDraw);
        }

        void step(const btScalar _fixedDeltaTime) { if(!mPaused) mWorld->stepSimulation(_fixedDeltaTime); }

        void render() { mDebugDraw->render(); }

        void pushDrawData() {
            mDebugDraw->drawLine({0, 0, 0}, {1, 0, 0}, {1, 0, 0});
            mDebugDraw->drawLine({0, 0, 0}, {0, 1, 0}, {0, 1, 0});
            mDebugDraw->drawLine({0, 0, 0}, {0, 0, 1}, {0, 0, 1});
            mWorld->debugDrawWorld();
        }

        auto addRb(btScalar _mass, const btTransform& _trans, btCollisionShape* _collider) {
            if(!_collider) return static_cast<btRigidBody*>(nullptr);
            mColliders.insert(_collider);
            auto motionstate = new btDefaultMotionState(_trans);
            btVector3 inertia(0, 0, 0);
            if(_mass > std::numeric_limits<btScalar>::epsilon() * 100)
                _collider->calculateLocalInertia(_mass, inertia);
            else
                _mass = 0;
            btRigidBody::btRigidBodyConstructionInfo info(_mass, motionstate, _collider, inertia);
            auto rb = new btRigidBody(info);
            mWorld->addRigidBody(rb);
            return rb;
        }

        void theWorld() { mPaused = !mPaused; }

    private:
        btCollisionConfiguration* mConfig;
        btDispatcher* mDispatcher;
        btBroadphaseInterface* mBroadphase;
        btConstraintSolver* mSolver;
        btDiscreteDynamicsWorld* mWorld;
        DebugDraw* mDebugDraw;
        std::set<btCollisionShape*> mColliders;
        bool mPaused;
    };
}
