///////////////////////////////////////////////////////////////////////////
// Precompilled headers
///////////////////////////////////////////////////////////////////////////
#include <glm/geometric.hpp>
#include <pch.hpp>

///////////////////////////////////////////////////////////////////////////
// Headers
///////////////////////////////////////////////////////////////////////////
#include <xrn/BoidsSim/Scene.hpp>
#include "xrn/BoidsSim/System/BoidBehavior.hpp"

#define FORCE_KEEP_IN_MAP



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
        registry.emplace<Scene::Position>(entity, ::glm::vec3{ 0, 0, -this->mapSize.z * 2 });
        registry.emplace<Scene::Rotation>(entity, ::glm::vec3{ 90.0f, .0f, .0f });
        // registry.emplace<Scene::Acceleration>(entity);
        registry.emplace<Scene::Velocity>(entity).setMaximumSpeed(1000.f);
        // registry.emplace<Scene::Mass>(entity, 1);
        registry.emplace<Scene::Direction>(entity, ::glm::vec3{ 90.0f, .0f, .0f });
    }

    // show map limits
    this->createLight({ this->mapSize.x / 2, this->mapSize.y / 2, this->mapSize.z / 2 });
    this->createLight({ this->mapSize.x / -2, this->mapSize.y / 2, this->mapSize.z / 2 });
    this->createLight({ this->mapSize.x / 2, this->mapSize.y / -2, this->mapSize.z / 2 });
    this->createLight({ this->mapSize.x / -2, this->mapSize.y / -2, this->mapSize.z / 2 });
    this->createLight({ this->mapSize.x / 2, this->mapSize.y / 2, this->mapSize.z / -2 });
    this->createLight({ this->mapSize.x / 2, this->mapSize.y / -2, this->mapSize.z / -2 });
    this->createLight({ this->mapSize.x / -2, this->mapSize.y / 2, this->mapSize.z / -2 });
    this->createLight({ this->mapSize.x / -2, this->mapSize.y / -2, this->mapSize.z / -2 });
    // this->createLight({ .0f, .0f, .0f }); // show map center

    // this->createBoid();
    this->getRegistry().reserve(Scene::BoidBehavior::numberOfBoids);
    for (auto i{ 0uz }; i < Scene::BoidBehavior::numberOfBoids; ++i) {
        if (i % 1'000 == 0 && i) {
            XRN_INFO("{} boids created", i)
        }
        this->createBoid();
    }
}



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
#ifdef FORCE_KEEP_IN_MAP
    auto boids{ this->getRegistry().view<Scene::Position, Scene::BoidBehavior::Boid>() };
    for (auto [ boid, position ] : boids.each()) {
        if (position.getX() > this->mapSize.x / 2) {
            position.addX(-this->mapSize.x);
        } else if (position.getX() < -this->mapSize.x / 2) {
            position.addX(this->mapSize.x);
        }
        if (position.getY() > this->mapSize.y / 2) {
            position.addY(-this->mapSize.y);
        } else if (position.getY() < -this->mapSize.y / 2) {
            position.addY(this->mapSize.y);
        }
        if (position.getZ() > this->mapSize.z / 2) {
            position.addZ(-this->mapSize.z);
        } else if (position.getZ() < -this->mapSize.z / 2) {
            position.addZ(this->mapSize.z);
        }
    }
#endif // FORCE_KEEP_IN_MAP
    return true;
}

///////////////////////////////////////////////////////////////////////////
// auto ::xrn::bsim::Scene::onTick(
    // ::xrn::Time deltaTime [[ maybe_unused ]]
// ) -> bool
// {
    // auto boids{ this->getRegistry().view<
        // Scene::Position
        // , Scene::Velocity
        // , Scene::Acceleration
        // , Scene::BoidBehavior::Boid
    // >() };

    // for (auto [ boid, position, velocity, acceleration ] : boids.each()) {
        // m_boidBehavior.update(boid, position, velocity, acceleration, boids);
    // }

    // return true;
// }

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::Scene::onKeyPressed(
    ::std::int16_t keyCode [[ maybe_unused ]]
)
{
    // this->createBoid();
}

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
    auto& registry{ this->getRegistry() };
    auto entity{ registry.create() };

    registry.emplace<Scene::PointLight>(
        entity, glm::vec3{ .25f, 1.f, 1.f }, 4.f, 1.f
    );
    registry.emplace<Scene::Position>(entity, pos);
}

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::Scene::createBoid()
{
    auto& registry{ this->getRegistry() };

    if (m_entities.size() < Scene::numberrOfThread) {
        m_entities.emplace_back();
    }

    m_entities[m_entityIndex].push_back(registry.create());
    auto entity{ m_entities[m_entityIndex].back() };

    ++m_entityIndex;
    if (m_entityIndex >= Scene::numberrOfThread) {
        m_entityIndex = 0;
    }

    registry.emplace<Scene::Transform3d>(
        entity
        , ::xrn::engine::vulkan::Model::createFromFile(this->getVulkanDevice(), "Cube")
    );

    registry.emplace<Scene::Position>(
        entity
        // , 0.f, 0.f, -this->mapSize.z / 2
        // , -50, 50, 0
        , ::xrn::rng(static_cast<int>(-mapSize.x / 2), static_cast<int>(mapSize.x / 2))
        , ::xrn::rng(static_cast<int>(-mapSize.y / 2), static_cast<int>(mapSize.y / 2))
        , ::xrn::rng(static_cast<int>(-mapSize.z / 2), static_cast<int>(mapSize.z / 2))
        // , -this->mapSize.z / 2
    );

    registry.emplace<Scene::Velocity>(
        entity
        // , 100.f, 0.f, 0.f
        // , Scene::BoidBehavior::maxSpeed
        , ::xrn::rng(-100, 100)
        , ::xrn::rng(-100, 100)
        , ::xrn::rng(-100, 100)
        // , 0
    ).setMagnitude(static_cast<float>(::xrn::rng(
            static_cast<int>(Scene::BoidBehavior::minSpeed) * 1000
            , static_cast<int>(Scene::BoidBehavior::maxSpeed) * 1000
        )) / 1000.f
    ).setMaximumSpeed(Scene::BoidBehavior::maxSpeed);
    // velocity.setMagnitude(10);

    registry.emplace<Scene::Acceleration>(
        entity
        // , 1.f
    );

    // registry.emplace<Scene::Mass>(
        // entity
        // , 1.f
    // );

    // registry.emplace<Scene::Scale>(
        // entity
        // , 5.f
    // );

    // registry.emplace<Scene::Control>(entity).setSpeed(
        // Scene::BoidBehavior::defaultSpeed
    // ).startMovingForward();

    registry.emplace<Scene::BoidBehavior::Boid>(entity);
}
