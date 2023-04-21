///////////////////////////////////////////////////////////////////////////
// Precompilled headers
///////////////////////////////////////////////////////////////////////////
#include <pch.hpp>

///////////////////////////////////////////////////////////////////////////
// Headers
///////////////////////////////////////////////////////////////////////////
#include <xrn/BoidsSim/Scene.hpp>
#include "xrn/BoidsSim/System/BoidBehavior.hpp"



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Constructors
//
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
::xrn::bsim::Scene::Scene()
    : ::xrn::engine::AScene::AScene{ true }
{
    // no light will be rendered, so make ambient light at max
    this->getFrameInfo().ubo.intensity() = 1;

    auto& registry{ this->getRegistry() };
    { // camera
        auto entity{ this->getCameraId() };
        registry.emplace<Scene::Control>(entity);
        registry.get<Scene::Control>(entity).setSpeed(2500);
        registry.emplace<Scene::Position>(entity, ::glm::vec3{
            0, 0, -this->mapSize.z * 2
        });
        registry.emplace<Scene::Rotation>(entity, ::glm::vec3{ 90.0f, .0f, .0f });
    }

    for (auto i{ 0uz }; i < Scene::BoidBehavior::numberOfBoids; ++i) {
        this->createBoid({ .0f, .0f, .0f });
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Rule of 5
//
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
::xrn::bsim::Scene::~Scene() = default;



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// events
//
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
auto ::xrn::bsim::Scene::onUpdate()
    -> bool
{
    return true;
}

///////////////////////////////////////////////////////////////////////////
auto ::xrn::bsim::Scene::onPostUpdate()
    -> bool
{

    auto boids{ this->getRegistry().view<Scene::Position, Scene::BoidBehavior::Boid>() };
    for (auto [ boid, position ] : boids.each()) {
        auto& pos{ position.get() };
        if (pos.x > this->mapSize.x / 2) {
            position.setX(-this->mapSize.x / 2);
        } else if (pos.x < -this->mapSize.x / 2) {
            position.setX(this->mapSize.x / 2);
        }
        if (pos.y > this->mapSize.y / 2) {
            position.setY(-this->mapSize.y / 2);
        } else if (pos.y < -this->mapSize.y / 2) {
            position.setY(this->mapSize.y / 2);
        }
        if (pos.z > this->mapSize.z / 2) {
            position.setZ(-this->mapSize.z / 2);
        } else if (pos.z < -this->mapSize.z / 2) {
            position.setZ(this->mapSize.z / 2);
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////
auto ::xrn::bsim::Scene::onTick(
    ::xrn::Time deltaTime [[ maybe_unused ]]
) -> bool
{

    // auto boids{ this->getRegistry().view<
        // Scene::Position, Scene::Rotation, Scene::Control, Scene::BoidBehavior::Boid
    // >() };
    // for (auto [ boid, position, rotation, control ] : boids.each()) {
        // m_boidBehavior.update(boid, position, rotation, control, boids);
    // }
    return true;
}

///////////////////////////////////////////////////////////////////////////
// void ::xrn::bsim::Scene::onKeyPressed(
    // ::std::int16_t keyCode [[ maybe_unused ]]
// )
// {}

///////////////////////////////////////////////////////////////////////////
// void ::xrn::bsim::Scene::onKeyReleased(
    // ::std::int16_t keyCode [[ maybe_unused ]]
// )
// {}

///////////////////////////////////////////////////////////////////////////
// void ::xrn::bsim::Scene::onMouseMoved(
    // ::glm::vec2 _ [[ maybe_unused ]]
// )
// {}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Healers
//
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::Scene::createLight(
    const ::glm::vec3& pos
)
{
    // show map limits
    // this->createLight({ this->mapSize.x / 2, this->mapSize.y / 2, this->mapSize.z / 2 });
    // this->createLight({ this->mapSize.x / -2, this->mapSize.y / 2, this->mapSize.z / 2 });
    // this->createLight({ this->mapSize.x / 2, this->mapSize.y / -2, this->mapSize.z / 2 });
    // this->createLight({ this->mapSize.x / 2, this->mapSize.y / 2, this->mapSize.z / -2 });
    // this->createLight({ this->mapSize.x / 2, this->mapSize.y / -2, this->mapSize.z / -2 });
    // this->createLight({ this->mapSize.x / -2, this->mapSize.y / 2, this->mapSize.z / -2 });
    // this->createLight({ this->mapSize.x / -2, this->mapSize.y / -2, this->mapSize.z / 2 });
    // this->createLight({ this->mapSize.x / -2, this->mapSize.y / -2, this->mapSize.z / -2 });
    // this->createLight({ .0f, .0f, .0f }); // show map center

    auto& registry{ this->getRegistry() };
    auto entity{ registry.create() };

    registry.emplace<Scene::PointLight>(
        entity, glm::vec3{ .25f, 1.f, 1.f }, 4.f, .2f
    );
    registry.emplace<Scene::Position>(entity, pos);
}

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::Scene::createBoid(
    const ::glm::vec3& pos
)
{
    auto& registry{ this->getRegistry() };
    auto entity{ registry.create() };

    registry.emplace<Scene::Transform3d>(
        entity
        , ::xrn::engine::vulkan::Model::createFromFile(this->getVulkanDevice(), "Cube")
    );

    registry.emplace<Scene::BoidBehavior::Boid>(entity);
    registry.emplace<Scene::Position>(
        entity
        , ::xrn::rng(static_cast<int>(-mapSize.x / 2), static_cast<int>(mapSize.x / 2))
        , ::xrn::rng(static_cast<int>(-mapSize.y / 2), static_cast<int>(mapSize.y / 2))
        , ::xrn::rng(static_cast<int>(-mapSize.z / 2), static_cast<int>(mapSize.z / 2))
    );
    registry.emplace<Scene::Rotation>(
        entity
        , ::xrn::rng(0, 360)
        , ::xrn::rng(0, 360)
        , ::xrn::rng(0, 360)
    );
    auto r{ registry.get<Scene::Rotation>(entity) };

    registry.emplace<Scene::Scale>(
        entity, Scene::BoidBehavior::defaultScale
    );

    registry.emplace<Scene::Control>(entity, true);
    registry.get<Scene::Control>(entity).setSpeed(
        Scene::BoidBehavior::defaultSpeed
    ).startMovingForward();
}
