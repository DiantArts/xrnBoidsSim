///////////////////////////////////////////////////////////////////////////
// Precompilled headers
///////////////////////////////////////////////////////////////////////////
#include <glm/geometric.hpp>
#include <pch.hpp>

///////////////////////////////////////////////////////////////////////////
// Headers
///////////////////////////////////////////////////////////////////////////
#include <xrn/BoidsSim/Scene.hpp>

#define ENABLE_SPACE_PARTITIONING


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

    { // camera
        auto entity{ this->getCameraId() };
        this->getRegistry().emplace<Scene::Control>(entity);
        this->getRegistry().emplace<Scene::Position>(entity, ::glm::vec3{
            Scene::BoidBehavior::mapSize.x / 2.f
            , Scene::BoidBehavior::mapSize.y / 2.f
            , -Scene::BoidBehavior::mapSize.z * 2.f
        });
        this->getRegistry().emplace<Scene::Direction>(entity, ::glm::vec3{ 0.0f, .0f, 1.0f });
        this->getRegistry().emplace<Scene::Velocity>(entity).setMaximumSpeed(1000.f);
    }

    this->createBoids();
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
    if constexpr (Scene::BoidBehavior::enableSpacePartitioning) {
        for (::std::uint16_t x{ 0 }; x < static_cast<::std::uint16_t>(m_partitions.size()); ++x) {
            for (::std::uint16_t y{ 0 }; y < static_cast<::std::uint16_t>(m_partitions[x].size()); ++y) {
                for (::std::uint16_t z{ 0 }; z < static_cast<::std::uint16_t>(m_partitions[x][y].size()); ++z) {
                    for (auto it{ m_partitions[x][y][z].begin() }; it != m_partitions[x][y][z].end();) {
                        const auto entity{ *it };
                        const auto& position{ this->getRegistry().get<Scene::Position>(entity) };
                        const auto index{ this->getPartitionIndex(*position) };
                        if (index.x != x || index.y != y || index.z != z) {
                            it = m_partitions[x][y][z].erase(it);
                            m_partitions[index.x][index.y][index.z].push_back(entity);
                        } else {
                            ++it;
                        }
                    }
                }
            }
        }
    }
    m_threads.runOnce();
    return true;
}

///////////////////////////////////////////////////////////////////////////
// auto ::xrn::bsim::Scene::onPostUpdate()
    // -> bool
// {
    // return true;
// }

///////////////////////////////////////////////////////////////////////////
// void ::xrn::bsim::Scene::onKeyPressed(
    // ::std::int16_t keyCode [[ maybe_unused ]]
// )
// {
    // this->createBoids();
// }

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
void ::xrn::bsim::Scene::createBoids()
{
    // preallocate
    this->getRegistry().reserve(Scene::BoidBehavior::numberOfBoids);
    this->getRegistry().storage<Scene::Transform3d>().reserve(Scene::BoidBehavior::numberOfBoids);
    this->getRegistry().storage<Scene::Position>().reserve(Scene::BoidBehavior::numberOfBoids);
    this->getRegistry().storage<Scene::Velocity>().reserve(Scene::BoidBehavior::numberOfBoids);
    this->getRegistry().storage<Scene::Acceleration>().reserve(Scene::BoidBehavior::numberOfBoids);
    this->getRegistry().storage<Scene::Scale>().reserve(Scene::BoidBehavior::numberOfBoids);
    this->getRegistry().storage<Scene::BoidBehavior::Boid>().reserve(Scene::BoidBehavior::numberOfBoids);

    // find size of each thread vectors
    ::std::size_t containerSize{ (Scene::BoidBehavior::numberOfBoids % Scene::BoidBehavior::numberOfThread == 0)
        ? Scene::BoidBehavior::numberOfBoids / Scene::BoidBehavior::numberOfThread
        : Scene::BoidBehavior::numberOfBoids / Scene::BoidBehavior::numberOfThread + 1
    };
    for (auto& vector : m_entities) {
        vector.reserve(containerSize);
    }

    // create the boids
    auto vectorIndex{ 0uz };
    for (auto i{ 0uz }; i < Scene::BoidBehavior::numberOfBoids; ++i) {
        if (i % 1'000 == 0 && i) {
            XRN_INFO("{} boids created", i)
        }

        if (m_entities[vectorIndex].size() >= containerSize) {
            ++vectorIndex;
        }

        auto boid{ this->getRegistry().create() };
        m_entities[vectorIndex].push_back(boid);

        this->initBoid(boid);
    }

    // auto boids{ this->getRegistry().view<Scene::BoidBehavior::Boid>() };
    // this->initBoids(boids.begin(), boids.end(), modelBuilder);

    // create the threads
    for (auto i{ 0uz }; i < Scene::BoidBehavior::numberOfThread; ++i) {
        m_threads.push(&Scene::updateBoids, this, i);
    }
}

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::Scene::initBoid(
    ::entt::entity boid
)
{
    this->getRegistry().emplace<Scene::Transform3d>(
        boid
        // , ::xrn::engine::vulkan::Model::createFromFile(this->getVulkanDevice(), "Boid")
        , ::xrn::engine::vulkan::Model::createFromFile(this->getVulkanDevice(), "Cube3")
    );

    const auto& position{ this->getRegistry().emplace<Scene::Position>(
        boid
        , ::xrn::rng(0, static_cast<int>(Scene::BoidBehavior::mapSize.x))
        , ::xrn::rng(0, static_cast<int>(Scene::BoidBehavior::mapSize.y))
        , ::xrn::rng(0, static_cast<int>(Scene::BoidBehavior::mapSize.z))
    ) };

    this->getRegistry().emplace<Scene::Velocity>(
        boid
        , ::xrn::rng(-100, 100)
        , ::xrn::rng(-100, 100)
        , ::xrn::rng(-100, 100)
    ).setMagnitude(static_cast<float>(::xrn::rng(
            static_cast<int>(Scene::BoidBehavior::minSpeed) * 1000
            , static_cast<int>(Scene::BoidBehavior::maxSpeed) * 1000
        )) / 1000.f
    ).setMaximumSpeed(Scene::BoidBehavior::maxSpeed);

    this->getRegistry().emplace<Scene::Acceleration>(boid);

    // this->getRegistry().emplace<Scene::Scale>(boid, .12f); // boid scale
    this->getRegistry().emplace<Scene::Direction>(boid);

    if constexpr (Scene::BoidBehavior::enableSpacePartitioning) {
        const auto index{ this->getPartitionIndex(*position) };
        m_partitions[index.x][index.y][index.z].push_back(boid);
    }
}

///////////////////////////////////////////////////////////////////////////
auto ::xrn::bsim::Scene::getPartitionIndex(
    const ::glm::vec3& position
) -> ::xrn::bsim::system::detail::PartitionIndex
{
    ::xrn::bsim::system::detail::PartitionIndex index;

    if (position.x < 0) {
        index.x = 0;
    } else if (position.x >= Scene::BoidBehavior::mapSize.x) {
        index.x = Scene::BoidBehavior::numberOfPartitions.x - 1;
    } else {
        index.x = static_cast<::std::uint16_t>(position.x) / static_cast<::std::uint16_t>(Scene::BoidBehavior::partitionSize.x);
    }

    if (position.y < 0) {
        index.y = 0;
    } else if (position.y >= Scene::BoidBehavior::mapSize.y) {
        index.y = Scene::BoidBehavior::numberOfPartitions.y - 1;
    } else {
        index.y = static_cast<::std::uint16_t>(position.y) / static_cast<::std::uint16_t>(Scene::BoidBehavior::partitionSize.y);
    }

    if (position.z < 0) {
        index.z = 0;
    } else if (position.z >= Scene::BoidBehavior::mapSize.z) {
        index.z = Scene::BoidBehavior::numberOfPartitions.z - 1;
    } else {
        index.z = static_cast<::std::uint16_t>(position.z) / static_cast<::std::uint16_t>(Scene::BoidBehavior::partitionSize.z);
    }

    return index;
}

///////////////////////////////////////////////////////////////////////////
auto ::xrn::bsim::Scene::getPartitionIndexBegin(
    const ::xrn::bsim::system::detail::PartitionIndex& index
) -> ::xrn::bsim::system::detail::PartitionIndex
{
    return ::xrn::bsim::system::detail::PartitionIndex{
        index.x == 0 ? 0 : index.x - 1
        , index.y == 0 ? 0 : index.y - 1
        , index.z == 0 ? 0 : index.z - 1
    };
}

///////////////////////////////////////////////////////////////////////////
auto ::xrn::bsim::Scene::getPartitionIndexEnd(
    const ::xrn::bsim::system::detail::PartitionIndex& index
) -> ::xrn::bsim::system::detail::PartitionIndex
{
    return ::xrn::bsim::system::detail::PartitionIndex{
        index.x >= Scene::BoidBehavior::numberOfPartitions.x - 1 ? Scene::BoidBehavior::numberOfPartitions.x - 1 : index.x + 1
        , index.y >= Scene::BoidBehavior::numberOfPartitions.y - 1 ? Scene::BoidBehavior::numberOfPartitions.y - 1 : index.y + 1
        , index.z >= Scene::BoidBehavior::numberOfPartitions.z - 1 ? Scene::BoidBehavior::numberOfPartitions.z - 1 : index.z + 1
    };
}

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::Scene::updateBoids(
    ::std::size_t threadIndex
)
{
    auto boids{ this->getRegistry().view<
        Scene::Position
        , Scene::Velocity
        , Scene::Acceleration
        , Scene::BoidBehavior::Boid
    >() };

    for (auto boid : m_entities[threadIndex]) {
        auto& position{ this->getRegistry().get<Scene::Position>(boid) };
        auto& velocity{ this->getRegistry().get<Scene::Velocity>(boid) };
        auto& acceleration{ this->getRegistry().get<Scene::Acceleration>(boid) };

        auto index{ this->getPartitionIndex(*position) };
        auto indexBegin{ this->getPartitionIndexBegin(index) };
        auto indexEnd{ this->getPartitionIndexEnd(index) };

        if constexpr (Scene::BoidBehavior::enableSpacePartitioning) {
            m_boidBehavior.update(boid, position, velocity, acceleration, m_partitions, indexBegin, indexEnd, this->getRegistry());
        } else {
            m_boidBehavior.update(boid, position, velocity, acceleration, boids);
        }
    }
}
