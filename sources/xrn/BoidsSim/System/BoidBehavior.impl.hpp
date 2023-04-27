#pragma once

#define XRN_ENABLE_SEPERATION
#define XRN_ENABLE_ALIGNMENT
#define XRN_ENABLE_COHESION
#define XRN_ENABLE_MAP_EDGE_FORCES

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Basic
//
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::system::BoidBehavior::update(
    ::entt::entity boid
    , const BoidBehavior::Position& position
    , const BoidBehavior::Velocity& velocity
    , BoidBehavior::Acceleration& acceleration
    , const auto& boids
) const
{
    acceleration.set(0);

    auto i{ 0 };

    #ifdef XRN_ENABLE_ALIGNMENT
        ::xrn::engine::component::Vector3 alignment;
    #endif // XRN_ENABLE_ALIGNMENT

    #ifdef XRN_ENABLE_COHESION
        ::xrn::engine::component::Vector3 cohesion;
    #endif // XRN_ENABLE_ALIGNMENT

    #ifdef XRN_ENABLE_SEPERATION
        ::xrn::engine::component::Vector3 seperation;
    #endif // XRN_ENABLE_SEPERATION



    for (const auto& [otherBoid, otherPosition, otherVelocity, _ ] : boids.each()) {
        if (otherBoid == boid) continue; // self

        auto distance{ ::glm::distance(*position, *otherPosition) };
        if (distance > BoidBehavior::perceptionRadius) continue; // other boid is out of range
        ++i;

        #ifdef XRN_ENABLE_ALIGNMENT
            alignment += *otherVelocity;
        #endif // XRN_ENABLE_ALIGNMENT

        #ifdef XRN_ENABLE_COHESION
            cohesion += *otherPosition;
        #endif // XRN_ENABLE_ALIGNMENT

        #ifdef XRN_ENABLE_SEPERATION
            if (distance != 0) {
                seperation += (position - otherPosition) / distance;
            }
        #endif // XRN_ENABLE_SEPERATION
    }



    if (i > 0) {
        #ifdef XRN_ENABLE_ALIGNMENT
            alignment /= static_cast<float>(i);
            alignment.setMagnitude(this->maxSpeed);
            alignment -= *velocity;
            alignment.setLength(m_alignmentInfo.force);
        #endif // XRN_ENABLE_ALIGNMENT

        #ifdef XRN_ENABLE_COHESION
            cohesion /= static_cast<float>(i);
            cohesion -= *position;
            cohesion.setMagnitude(this->maxSpeed);
            cohesion -= *velocity;
            cohesion.setLength(m_cohesionInfo.force);
        #endif // XRN_ENABLE_COHESION

        #ifdef XRN_ENABLE_SEPERATION
            seperation /= static_cast<float>(i);
            seperation.setMagnitude(this->maxSpeed);
            seperation -= *velocity;
            seperation.setLength(m_seperationInfo.force);
        #endif // XRN_ENABLE_SEPERATION
    }



    #ifdef XRN_ENABLE_ALIGNMENT
        acceleration.add(*alignment);
    #endif // XRN_ENABLE_ALIGNMENT

    #ifdef XRN_ENABLE_COHESION
        acceleration.add(*cohesion);
    #endif // XRN_ENABLE_COHESION

    #ifdef XRN_ENABLE_SEPERATION
        acceleration.add(*seperation);
    #endif // XRN_ENABLE_SEPERATION



    // Edge map forces
    #ifdef XRN_ENABLE_MAP_EDGE_FORCES
        ::xrn::engine::component::Vector3 edgeMapForces;
        auto edgeMapForcesCounter{ 0 };

        if (position.getX() > this->mapSize.x) {
            edgeMapForces += ::glm::vec3{ this->mapSize.x, position.getY(), position.getZ() };
            ++edgeMapForcesCounter;
        } else if (position.getX() < 0) {
            edgeMapForces += ::glm::vec3{ 0, position.getY(), position.getZ() };
            ++edgeMapForcesCounter;
        }
        if (position.getY() > this->mapSize.y) {
            edgeMapForces += ::glm::vec3{ position.getX(), this->mapSize.y, position.getZ() };
            ++edgeMapForcesCounter;
        } else if (position.getY() < 0) {
            edgeMapForces += ::glm::vec3{ position.getX(), 0, position.getZ() };
            ++edgeMapForcesCounter;
        }
        if (position.getZ() > this->mapSize.z) {
            edgeMapForces += ::glm::vec3{ position.getX(), position.getY(), this->mapSize.z };
            ++edgeMapForcesCounter;
        } else if (position.getZ() < 0) {
            edgeMapForces += ::glm::vec3{ position.getX(), position.getY(), 0 };
            ++edgeMapForcesCounter;
        }

        if (edgeMapForcesCounter) {
            edgeMapForces /= static_cast<float>(edgeMapForcesCounter);
            edgeMapForces -= *position;
            edgeMapForces.setMagnitude(this->maxSpeed);
            edgeMapForces -= *velocity;
            edgeMapForces.setLength(m_edgeMapForcesInfo.force);
        }

        acceleration.add(*edgeMapForces);
    #endif // XRN_ENABLE_MAP_EDGE_FORCES
}

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::system::BoidBehavior::update(
    ::entt::entity boid
    , const BoidBehavior::Position& position
    , const BoidBehavior::Velocity& velocity
    , BoidBehavior::Acceleration& acceleration
    , const BoidBehavior::PartitionArray& boids
    , const ::xrn::bsim::system::detail::PartitionIndex& indexBegin
    , const ::xrn::bsim::system::detail::PartitionIndex& indexEnd
    , const auto& registry
) const
{
    acceleration.set(0);

    auto i{ 0 };

    #ifdef XRN_ENABLE_ALIGNMENT
        ::xrn::engine::component::Vector3 alignment;
    #endif // XRN_ENABLE_ALIGNMENT

    #ifdef XRN_ENABLE_COHESION
        ::xrn::engine::component::Vector3 cohesion;
    #endif // XRN_ENABLE_ALIGNMENT

    #ifdef XRN_ENABLE_SEPERATION
        ::xrn::engine::component::Vector3 seperation;
    #endif // XRN_ENABLE_SEPERATION



    for (auto x{ indexBegin.x }; x < indexEnd.x; ++x) {
        for (auto y{ indexBegin.y }; y < indexEnd.y; ++y) {
            for (auto z{ indexBegin.z }; z < indexEnd.z; ++z) {
                for (auto otherBoid : boids[x][y][z]) {
                    if (otherBoid == boid) continue; // self

                    auto& otherPosition{ registry.template get<BoidBehavior::Position>(otherBoid) };
                    auto& otherVelocity{ registry.template get<BoidBehavior::Velocity>(otherBoid) };

                    auto distance{ ::glm::distance(*position, *otherPosition) };
                    if (distance > BoidBehavior::perceptionRadius) continue; // other boid is out of range
                    ++i;

                    #ifdef XRN_ENABLE_ALIGNMENT
                        alignment += *otherVelocity;
                    #endif // XRN_ENABLE_ALIGNMENT

                    #ifdef XRN_ENABLE_COHESION
                        cohesion += *otherPosition;
                    #endif // XRN_ENABLE_ALIGNMENT

                    #ifdef XRN_ENABLE_SEPERATION
                        if (distance != 0) {
                            seperation += (position - otherPosition) / distance;
                        }
                    #endif // XRN_ENABLE_SEPERATION
                }
            }
        }
    }



    if (i > 0) {
        #ifdef XRN_ENABLE_ALIGNMENT
            alignment /= static_cast<float>(i);
            alignment.setMagnitude(this->maxSpeed);
            alignment -= *velocity;
            alignment.setLength(m_alignmentInfo.force);
        #endif // XRN_ENABLE_ALIGNMENT

        #ifdef XRN_ENABLE_COHESION
            cohesion /= static_cast<float>(i);
            cohesion -= *position;
            cohesion.setMagnitude(this->maxSpeed);
            cohesion -= *velocity;
            cohesion.setLength(m_cohesionInfo.force);
        #endif // XRN_ENABLE_COHESION

        #ifdef XRN_ENABLE_SEPERATION
            seperation /= static_cast<float>(i);
            seperation.setMagnitude(this->maxSpeed);
            seperation -= *velocity;
            seperation.setLength(m_seperationInfo.force);
        #endif // XRN_ENABLE_SEPERATION
    }



    #ifdef XRN_ENABLE_ALIGNMENT
        acceleration.add(*alignment);
    #endif // XRN_ENABLE_ALIGNMENT

    #ifdef XRN_ENABLE_COHESION
        acceleration.add(*cohesion);
    #endif // XRN_ENABLE_COHESION

    #ifdef XRN_ENABLE_SEPERATION
        acceleration.add(*seperation);
    #endif // XRN_ENABLE_SEPERATION


    // Edge map forces
    #ifdef XRN_ENABLE_MAP_EDGE_FORCES
        ::xrn::engine::component::Vector3 edgeMapForces;
        auto edgeMapForcesCounter{ 0 };

        if (position.getX() > this->mapSize.x) {
            edgeMapForces += ::glm::vec3{ this->mapSize.x, position.getY(), position.getZ() };
            ++edgeMapForcesCounter;
        } else if (position.getX() < 0) {
            edgeMapForces += ::glm::vec3{ 0, position.getY(), position.getZ() };
            ++edgeMapForcesCounter;
        }
        if (position.getY() > this->mapSize.y) {
            edgeMapForces += ::glm::vec3{ position.getX(), this->mapSize.y, position.getZ() };
            ++edgeMapForcesCounter;
        } else if (position.getY() < 0) {
            edgeMapForces += ::glm::vec3{ position.getX(), 0, position.getZ() };
            ++edgeMapForcesCounter;
        }
        if (position.getZ() > this->mapSize.z) {
            edgeMapForces += ::glm::vec3{ position.getX(), position.getY(), this->mapSize.z };
            ++edgeMapForcesCounter;
        } else if (position.getZ() < 0) {
            edgeMapForces += ::glm::vec3{ position.getX(), position.getY(), 0 };
            ++edgeMapForcesCounter;
        }

        if (edgeMapForcesCounter) {
            edgeMapForces /= static_cast<float>(edgeMapForcesCounter);
            edgeMapForces -= *position;
            edgeMapForces.setMagnitude(this->maxSpeed);
            edgeMapForces -= *velocity;
            edgeMapForces.setLength(m_edgeMapForcesInfo.force);
        }

        acceleration.add(*edgeMapForces);
    #endif // XRN_ENABLE_MAP_EDGE_FORCES
}

#undef XRN_ENABLE_SEPERATION
#undef XRN_ENABLE_ALIGNMENT
#undef XRN_ENABLE_COHESION
