#pragma once

#define XRN_ENABLE_SEPERATION
#define XRN_ENABLE_ALIGNMENT
#define XRN_ENABLE_COHESION

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Basic
//
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::system::BoidBehavior::update(
    ::entt::entity boid
    , BoidBehavior::Position& position
    , BoidBehavior::Velocity& velocity
    , BoidBehavior::Acceleration& acceleration
    , const auto& boids
) const
{
    acceleration.set(0);

    auto i{ 0 };
    ::xrn::engine::component::Vector3 alignment;
    ::xrn::engine::component::Vector3 cohesion;
    ::xrn::engine::component::Vector3 seperation;

    for (const auto& [otherBoid, otherPosition, otherVelocity, _ ] : boids.each()) {
        if (otherBoid == boid) continue; // self

        auto distance{ ::glm::distance(*position, *otherPosition) };
        if (distance > m_perceptionRadius) continue; // other boid is out of range
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
        acceleration.add(alignment.get());
    #endif // XRN_ENABLE_ALIGNMENT

    #ifdef XRN_ENABLE_COHESION
        acceleration.add(cohesion.get());
    #endif // XRN_ENABLE_COHESION

    #ifdef XRN_ENABLE_SEPERATION
        acceleration.add(seperation.get());
    #endif // XRN_ENABLE_SEPERATION
}

#undef XRN_ENABLE_SEPERATION
#undef XRN_ENABLE_ALIGNMENT
#undef XRN_ENABLE_COHESION
