#pragma once

#include "Trail.hpp"

class Car
{
private:
    TrailManager m_trails;

    float m_maxVelocity;
    float m_minVelocity;

    float m_accelerationSpeed;
    float m_decelerationSpeed;
    float m_turnSpeed;

    float m_rollFriction;
    float m_airFriction;

    bool m_handBrake{false};

    float m_normalGrip;
    float m_handbrakeGrip;
    float m_grip;

    const float m_staticGripSpeed{150.f};
    const float m_staticGripFactor{300.f};
    const float m_gripFactor{50.f};

    float m_driftTimer{0};

    float m_throttle{0};
    float m_steering{0};

    float m_rotation{0};

    Vector2 m_vel{0, 0};
    Vector2 m_pos{0, 0};
    Vector2 m_size{0, 0};

    Color m_color{RED};

public:
    Car(
        float trailTime,
        size_t maxTrails,
        float maxVelocity, 
        float minVelocity, 
        float accelerationSpeed, 
        float decelerationSpeed, 
        float turnSpeed, 
        float rollFriction,
        float airFriction,
        float grip,
        Vector2 startPos, 
        Vector2 size) 
        : m_trails(trailTime, maxTrails)
        , m_maxVelocity(maxVelocity)
        , m_minVelocity(minVelocity)
        , m_accelerationSpeed(accelerationSpeed)
        , m_decelerationSpeed(decelerationSpeed)
        , m_turnSpeed(turnSpeed)
        , m_rollFriction(rollFriction)
        , m_airFriction(airFriction)
        , m_normalGrip(grip)
        , m_handbrakeGrip(grip/4)
        , m_grip(grip)
        , m_pos(startPos)
        , m_size(size) 
    {}

    Car(
        float trailTime,
        size_t maxTrails,
        float maxVelocity, 
        float minVelocity, 
        float accelerationSpeed, 
        float decelerationSpeed, 
        float turnSpeed, 
        float rollFriction,
        float airFriction,
        float grip,
        Vector2 startPos, 
        Vector2 size,
        Color color) 
        : m_trails(trailTime, maxTrails)
        , m_maxVelocity(maxVelocity)
        , m_minVelocity(minVelocity)
        , m_accelerationSpeed(accelerationSpeed)
        , m_decelerationSpeed(decelerationSpeed)
        , m_turnSpeed(turnSpeed)
        , m_rollFriction(rollFriction)
        , m_airFriction(airFriction)
        , m_normalGrip(grip)
        , m_handbrakeGrip(grip/4)
        , m_grip(grip)
        , m_pos(startPos)
        , m_size(size)
        , m_color(color) 
    {}

    ~Car() = default;

    void input()
    {
        m_throttle = 0.f;
        m_steering = 0.f;

        if (IsKeyDown(KEY_D)) m_steering += m_turnSpeed;
        if (IsKeyDown(KEY_A)) m_steering -= m_turnSpeed;

        if (IsKeyDown(KEY_W)) m_throttle += m_accelerationSpeed;
        if (IsKeyDown(KEY_S)) m_throttle -= m_decelerationSpeed;

        m_handBrake = IsKeyDown(KEY_SPACE);

        if (IsKeyDown(KEY_LEFT_SHIFT)) m_throttle *= 1.5f;
    }

    void update(const float dt)
    {   
        // calculate rotation from deg in rad and set rotation always in between 0 an 360

        float rad = m_rotation * (PI / 180.f);

        // creation of normalized direction vectors

        Vector2 forward = {sinf(rad), -cosf(rad)};
        Vector2 sideways  = {-forward.y, forward.x};

        // get new velocity, without scaled speeds

        m_vel.x += m_throttle * forward.x * dt;
        m_vel.y += m_throttle * forward.y * dt;

        // get direction speeds with the dot product of velocity and direction vectors

        float forwardSpeed = m_vel.x * forward.x + m_vel.y * forward.y;
        float sidewaysSpeed = m_vel.x * sideways.x + m_vel.y * sideways.y;

        // calculate rotation with forward speed (if 0 no rotation possible)

        float drift = fminf(fabsf(sidewaysSpeed)/fabsf(forwardSpeed) + 1e-3f, 1.f)
        ;
        float driftFactor = 1.f + drift * (m_normalGrip/m_grip - 1.f) * 0.05f;
        float speedFactor = forwardSpeed * 0.6f * fmaxf(150.f / (fabsf(forwardSpeed) + 150.f), 0.2f);

        m_rotation += m_steering * speedFactor * driftFactor * dt;

        // now you can scale the forward and sideways speed, first scale forward down when handbrake activated

        if (m_handBrake)
        {
            m_grip = m_handbrakeGrip;
            if (forwardSpeed < 0) forwardSpeed = fminf(forwardSpeed + m_decelerationSpeed * dt, 0);
            else forwardSpeed = fmaxf(forwardSpeed - m_decelerationSpeed * dt, 0);
        }
        else m_grip = m_normalGrip;

        // then scale forward down with friction

        float friction = (m_airFriction * fabsf(forwardSpeed) * m_rollFriction * dt);
        forwardSpeed /= 1.f + friction;
        
        if (forwardSpeed > m_maxVelocity) forwardSpeed = m_maxVelocity;
        if (forwardSpeed < m_minVelocity) forwardSpeed = m_minVelocity;

        // and finally scale sideway down, the more grip, the less sideway speed (drift)

        sidewaysSpeed *= expf(-m_grip * dt);

        float scaledGrip = m_grip * m_gripFactor;
        if (forwardSpeed < m_staticGripSpeed) scaledGrip = m_grip * m_staticGripFactor;
        
        if (sidewaysSpeed < 0) sidewaysSpeed = fminf(sidewaysSpeed + m_grip * scaledGrip * dt, 0);
        else sidewaysSpeed = fmaxf(sidewaysSpeed - m_grip * scaledGrip * dt, 0);

        // add a trail when drifting

        m_trails.addTrail(forward, sideways, forwardSpeed, sidewaysSpeed, m_size, m_pos, m_rotation, m_handBrake, dt);

        // get time of drift 

        if (fabsf(sidewaysSpeed) > fabsf(forwardSpeed)/2) 
        {
            m_driftTimer += dt;
        }
        else if (m_driftTimer > 0)
        {   
            if (m_driftTimer > 1) std::cout << "Drift: " << std::fixed << std::setprecision(1) << m_driftTimer << " Sekunden!" << std::endl;
            m_driftTimer = 0.f;
        }

        // add forward vector and sideway vector back, to create new velocity and update position

        forward = Vector2Scale(forward, forwardSpeed);
        sideways = Vector2Scale(sideways, sidewaysSpeed);

        m_vel = Vector2Add(forward, sideways);
        m_pos += Vector2Scale(m_vel, dt);
    }

    void render()
    {
        for (auto& trail : *m_trails.getTrails())
        {
            DrawRectanglePro(trail.rectangle, {trail.rectangle.width/2, trail.rectangle.height/2}, trail.rotation, {80, 80, 80, 150});
        }
        DrawRectanglePro({m_pos.x, m_pos.y, m_size.x, m_size.y}, {m_size.x * 0.5f, m_size.y * 0.7f}, m_rotation, m_color);
    }

    const float getRotation() const {return m_rotation;}
    const float getThrottle() const {return m_throttle;}

    const Vector2 getVel() const {return m_vel;}
    const Vector2 getPos() const {return m_pos;}
};