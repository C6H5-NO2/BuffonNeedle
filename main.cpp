#include "Timer.hpp"
#include "World.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <bullet3/btBulletDynamicsCommon.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <random>
#include <ctime>

using namespace Needle;

struct Info final {
    static constexpr btScalar a = 3.55f;
    static constexpr btScalar l = 1.13f;
    static constexpr unsigned round = 1000;
    static constexpr unsigned lineNum = 15;
    static constexpr btScalar len = 25; // x
    static constexpr btScalar wid = 15; // z
    static constexpr std::pair<btScalar, btScalar> h = {5, 15};
    static constexpr std::pair<btScalar, btScalar> fz = {0, 250};
    static constexpr std::pair<btScalar, btScalar> fy = {0, 100};
    static constexpr std::pair<btScalar, btScalar> fx = {0, 450};
};

auto addObjs(World& _world) {
    btTransform trans;

    trans.setIdentity();
    trans.setOrigin({0, -.5, 0});
    auto ground = _world.addRb(0, trans, new btBoxShape({Info::len, .5, Info::wid}));

    auto x = -(Info::lineNum / 2 * Info::a);
    trans.setIdentity();
    for(auto i = 0u; i < Info::lineNum; ++i) {
        trans.setOrigin({x + i * Info::a, .22f, 0});
        auto line = _world.addRb(0, trans, new btBoxShape({.005f, .2f, Info::wid}));
        line->setCollisionFlags(btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);
        line->setCustomDebugColor({0, 0, 1});
    }

    trans.setIdentity();
    trans.setOrigin({0, -10, 0});
    auto needle = _world.addRb(1, trans, new btBoxShape({Info::l / 2, .1, .1})); // new btCapsuleShapeX(.1, Info::l - 2 * .1)
    needle->setFriction(.1);
    //needle->setRollingFriction(100);
    //needle->setSleepingThresholds(.2, .5);
    needle->setActivationState(DISABLE_DEACTIVATION);

    return needle;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    World world;
    world.awake();

    std::mt19937 rng(time(nullptr));
    std::uniform_real_distribution<float> hDist(Info::h.first, Info::h.second);
    std::uniform_real_distribution<float> xDist(-Info::l / 2, Info::l / 2);
    std::uniform_real_distribution<float> fzDist(Info::fz.first, Info::fz.second);
    std::uniform_real_distribution<float> fyDist(Info::fy.first, Info::fy.second);
    std::uniform_real_distribution<float> fxDist(Info::fx.first, Info::fx.second);
    std::uniform_int_distribution<int> signDist(0, 1);
    auto randSign = [&signDist = signDist](auto& _rng) { return signDist(_rng) ? 1 : -1; };

    auto needle = addObjs(world);

    Timer timer;
    timer.reset();

    std::ofstream file("result.csv");
    unsigned round = 0, totalCross = 0;
    bool nextRound = true;
    bool quit = false;
    while(!quit) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                {
                    quit = true;
                    break;
                }
                case SDL_WINDOWEVENT:
                {
                    if(event.window.event == SDL_WINDOWEVENT_CLOSE)
                        quit = true;
                    break;
                }
                default:
                    break;
            }
        }

        timer.tick();

        const float fixedDeltaTime = 1.f / 60.f;
        // clamp frame rate (dirty code)
        if(timer.deltaTime() < fixedDeltaTime)
            std::this_thread::sleep_for(std::chrono::duration<float>(fixedDeltaTime - timer.deltaTime()));

        if(nextRound) {
            ++round;
            nextRound = false;
            needle->setLinearVelocity({0, 0, 0});
            needle->setAngularVelocity({0, 0, 0});
            btTransform trans;
            trans.setIdentity();
            trans.setOrigin({0, hDist(rng), 0});
            needle->setWorldTransform(trans);
            needle->applyForce({randSign(rng) * fxDist(rng), randSign(rng) * fyDist(rng), randSign(rng) * fzDist(rng)},
                               {xDist(rng), 0, 0});
        }
        if(round > Info::round)
            break;

        world.step(fixedDeltaTime);
        world.pushDrawData();
        world.render();

        btVector3 aabbMin, aabbMax;
        needle->getAabb(aabbMin, aabbMax);
        //                                            magic number
        if(needle->getWorldTransform().getOrigin().y() < (.15)) {
            bool isCross = static_cast<int>(ceil(aabbMin.x() / Info::a)) == static_cast<int>(floor(aabbMax.x() / Info::a));
            if(isCross)
                ++totalCross;
            std::cout << round << ',' << isCross << ',' << totalCross << ','
                << (2 * round * Info::l / (Info::a * totalCross)) << std::endl;
            file << round << ',' << isCross << ',' << totalCross << ','
                << (2 * round * Info::l / (Info::a * totalCross)) << std::endl;
            nextRound = true;
        }
    }

    file.close();

    SDL_Quit();
    return EXIT_SUCCESS;
}
