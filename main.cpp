#include "Rng.hpp"
#include "Timer.hpp"
#include "World.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <bullet3/btBulletDynamicsCommon.h>

#include <iostream>
#include <thread>

using namespace Needle;

extern "C" int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    World world;
    world.awake();

    btTransform trans;
    trans.setIdentity();

    trans.setRotation({10, 0, 0});
    auto needle = world.addRb(0,
                              trans,
                              new btCapsuleShapeX(.1, 3));
    //needle->setActivationState(DISABLE_DEACTIVATION);

    Timer timer;
    timer.reset();

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

        world.step(fixedDeltaTime);
        world.pushDrawData();
        world.render();
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}
