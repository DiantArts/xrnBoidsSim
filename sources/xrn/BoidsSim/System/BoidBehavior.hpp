#pragma once

#include <xrn/BoidsSim/System/Detail/BoidBehavior.impl.hpp>

namespace xrn::bsim::system {

///////////////////////////////////////////////////////////////////////////
/// \brief Scene of the came
/// \ingroup game
///
/// \include Scene.hpp <Game/Scene.hpp>
///
///////////////////////////////////////////////////////////////////////////
class BoidBehavior
{

public:

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Component aliases
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    using Position = ::xrn::engine::component::Position;
    using Velocity = ::xrn::engine::component::Velocity;
    using Acceleration = ::xrn::engine::component::Acceleration;

    ///////////////////////////////////////////////////////////////////////////
    /// Empty signature type
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Boid {};

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Static
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    /// Number of threads running
    ///
    ///////////////////////////////////////////////////////////////////////////
    static inline constexpr const ::std::size_t numberOfThread{ 10 };

    ///////////////////////////////////////////////////////////////////////////
    /// Number of boid running
    ///
    ///////////////////////////////////////////////////////////////////////////
    static inline constexpr const ::std::size_t numberOfBoids{ 25'000 };

    ///////////////////////////////////////////////////////////////////////////
    /// Min speed of a boid
    ///
    ///////////////////////////////////////////////////////////////////////////
    static inline constexpr const float minSpeed{ 75.f };

    ///////////////////////////////////////////////////////////////////////////
    /// Max speed of a boid
    ///
    ///////////////////////////////////////////////////////////////////////////
    static inline constexpr const float maxSpeed{ 125.f };

    ///////////////////////////////////////////////////////////////////////////
    /// Perception radius at which boids interact with each other
    ///
    ///////////////////////////////////////////////////////////////////////////
    static inline constexpr const ::std::size_t perceptionRadius{ 30uz };

    ///////////////////////////////////////////////////////////////////////////
    /// Size of each map axis
    ///
    ///////////////////////////////////////////////////////////////////////////
    static inline constexpr const ::glm::vec3 mapSize{ 1000.0f, 500.0f, 500.0f };

    ///////////////////////////////////////////////////////////////////////////
    /// Enable space partitioning
    ///
    ///////////////////////////////////////////////////////////////////////////
    static inline constexpr const bool enableSpacePartitioning{ true };

    ///////////////////////////////////////////////////////////////////////////
    /// Number of partitions per axis
    ///
    ///////////////////////////////////////////////////////////////////////////
    static inline constexpr const ::xrn::bsim::system::detail::PartitionIndex numberOfPartitions{
        ::xrn::bsim::system::detail::getNumberOfPartition(
            BoidBehavior::mapSize
            , BoidBehavior::perceptionRadius
        )
    };

    ///////////////////////////////////////////////////////////////////////////
    /// Number of partitions per axis
    ///
    ///////////////////////////////////////////////////////////////////////////
    static inline constexpr const ::xrn::bsim::system::detail::PartitionIndex partitionSize{
        static_cast<int>(mapSize.x) / (numberOfPartitions.x - 1)
        , static_cast<int>(mapSize.y) / (numberOfPartitions.y - 1)
        , static_cast<int>(mapSize.z) / (numberOfPartitions.z - 1)
    };

    ///////////////////////////////////////////////////////////////////////////
    /// Array Type
    ///
    ///////////////////////////////////////////////////////////////////////////
    using PartitionArray = ::std::array<
            ::std::array<
                ::std::array<
                    ::std::vector<::entt::entity>
                    , static_cast<::std::size_t>(BoidBehavior::numberOfPartitions.z)
                >, static_cast<::std::size_t>(BoidBehavior::numberOfPartitions.y)
            >, static_cast<::std::size_t>(BoidBehavior::numberOfPartitions.x)
        >;



public:

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Basic
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    ///
    ///////////////////////////////////////////////////////////////////////////
    void update(
        ::entt::entity boid
        , const BoidBehavior::Position& position
        , const BoidBehavior::Velocity& velocity
        , BoidBehavior::Acceleration& acceleration
        , const auto& boids
    ) const;

    ///////////////////////////////////////////////////////////////////////////
    ///
    ///////////////////////////////////////////////////////////////////////////
    void update(
        ::entt::entity boid
        , const BoidBehavior::Position& position
        , const BoidBehavior::Velocity& velocity
        , BoidBehavior::Acceleration& acceleration
        , const BoidBehavior::PartitionArray& boids
        , const ::xrn::bsim::system::detail::PartitionIndex& indexBegin
        , const ::xrn::bsim::system::detail::PartitionIndex& indexEnd
        , const auto& registry
    ) const;



private:

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Member variables
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    struct Information {
        float force;
    };
    static inline constexpr const BoidBehavior::Information m_cohesionInfo{ .force = 35.f };
    static inline constexpr const BoidBehavior::Information m_alignmentInfo{ .force = 100.f };
    static inline constexpr const BoidBehavior::Information m_seperationInfo{ .force = 50.f };
    static inline constexpr const BoidBehavior::Information m_edgeMapForcesInfo{ .force = 100.f };


};

} // namespace xrn::bsim::system

///////////////////////////////////////////////////////////////////////////
// Header-implimentation
///////////////////////////////////////////////////////////////////////////
#include <xrn/BoidsSim/System/BoidBehavior.impl.hpp>
