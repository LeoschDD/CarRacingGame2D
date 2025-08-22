#pragma once

#include "Trail.hpp"

class Car
{
private:
    TrailManager m_trails;

    float m_accelerationSpeed;
    float m_decelerationSpeed;
    float m_turnSpeed;

    float m_rollFriction;
    float m_airFriction;

    bool m_handBrake{false};

    float m_normalGrip;
    float m_handbrakeGrip;
    float m_grip;

    const float m_staticGripSpeed{100.f};
    const float m_staticGripFactor{200.f};
    const float m_gripFactor{10.f};

    float m_driftTimer{0};
    bool m_driftBoost{false};

    float m_boostLevel{100.f};

    float m_throttle{0};
    float m_steering{0};

    float m_rotation{0};

    Vector2 m_vel{0, 0};
    Vector2 m_pos{0, 0};
    Vector2 m_size{0, 0};

    Texture2D* m_texture{nullptr};

public:     
    Car(
        float trailTime,
        size_t maxTrails,
        float accelerationSpeed, 
        float decelerationSpeed, 
        float turnSpeed, 
        float rollFriction,
        float airFriction,
        float grip,
        Vector2 startPos, 
        Vector2 size,
        Texture2D* texture) 
        : m_trails(trailTime, maxTrails)
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
        , m_texture(texture)
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

        if (m_driftBoost) m_throttle *= 1.5f;
        if (IsKeyDown(KEY_LEFT_SHIFT) && m_boostLevel > 0.f && m_throttle > 0.f) 
        {
            m_throttle *= 2.5f;
            m_boostLevel -= 0.1f;
        }
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

        float drift = fminf(fabsf(sidewaysSpeed)/fabsf(forwardSpeed) + 1e-3f, 1.f);
        float driftFactor = 1.f + drift * (m_normalGrip/(m_grip + 1e-6f) - 1.f) * 0.2f;
        float speedFactor = forwardSpeed * fmaxf(150.f / (fabsf(forwardSpeed) + 150.f), 0.2f);

        m_rotation += m_steering * speedFactor * driftFactor * 0.1f * dt;
        if (fabsf(m_rotation) > 360.f) m_rotation = 0.f;

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
        
        // and finally scale sideway down, the more grip, the less sideway speed (drift)

        sidewaysSpeed *= expf(-m_grip * dt);

        float scaledGrip = m_grip * m_gripFactor;
        if (forwardSpeed < m_staticGripSpeed) scaledGrip = m_grip * m_staticGripFactor;
        
        if (sidewaysSpeed < 0) sidewaysSpeed = fminf(sidewaysSpeed + m_grip * scaledGrip * dt, 0);
        else sidewaysSpeed = fmaxf(sidewaysSpeed - m_grip * scaledGrip * dt, 0);

        // add a trail when drifting

        m_trails.addTrail(forward, sideways, forwardSpeed, sidewaysSpeed, m_size, m_pos, m_rotation, m_handBrake, dt);

        // get time of drift and update boost logic

        m_driftBoost = false;

        if (drift >= 0.5f && forwardSpeed > 5.f && sidewaysSpeed > 5.f) 
        {
            m_driftTimer += dt;
            m_driftBoost = true;
        }
        else if (m_driftTimer > 0)
        {   
            if (m_driftTimer > 1)
            {
                m_boostLevel += 10;
                if (m_driftTimer > 2) m_boostLevel += 10;
                if (m_driftTimer > 3) m_boostLevel += 10;

                std::cout << "Drift: " << std::fixed << std::setprecision(1) << m_driftTimer << " Sekunden!" << std::endl;
            }
            m_driftTimer = 0.f;
        }

        if (m_boostLevel > 100.f) m_boostLevel = 100.f;
        if (m_boostLevel < 0.f) m_boostLevel = 0.f;

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
        if (m_texture)
        {
            DrawTexturePro(*m_texture, 
                {142.f, 131.f, 71.f, 131.f}, 
                {m_pos.x, m_pos.y, m_size.x, m_size.y}, 
                {m_size.x * 0.5f, m_size.y * 0.8f}, 
                m_rotation, 
                WHITE);
        }
    }

    void tuner()
    {
        static bool open = true;
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        ImGui::Begin("Car", &open, flags);

        ImGui::SetWindowPos({0, 0});
        ImGui::SetWindowSize({400, 1080});

        if (ImGui::CollapsingHeader("Car Info"))
        {
            ImGui::BeginGroup();
            ImGui::Text("Position x: %2.f", m_pos.x);
            ImGui::Text("Position y: %2.f", m_pos.y);

            ImGui::Text("Velocity x: %2.f", m_vel.x);
            ImGui::Text("Velocity y: %2.f", m_vel.y);

            ImGui::Text("Throttle: %2.f", m_throttle);

            ImGui::Text("Rotation: %2.f", m_rotation);
            ImGui::Text("Boost: %2.f", m_boostLevel);  
            ImGui::EndGroup();  
        }
        if (ImGui::CollapsingHeader("Car Tuner"))
        {   
            ImGui::BeginGroup();
            ImGui::SliderFloat("Acceleration Speed", &m_accelerationSpeed, 0.1f, 10000.f, "%2.f\n");
            ImGui::SliderFloat("Deceleration Speed", &m_decelerationSpeed, 0.1f, 10000.f, "%2.f\n");
            ImGui::SliderFloat("Turn Speed", &m_turnSpeed, 0.01f, 50.f, "%3.f\n");
            ImGui::SliderFloat("Normal Grip", &m_normalGrip, 0.01f, 50.f, "%3.f\n");
            ImGui::SliderFloat("Handbrake Grip", &m_handbrakeGrip, 0.01f, 30.f, "%3.f\n");
            ImGui::InputFloat("Roll Friction", &m_rollFriction, 0.f, 0.f, "%4.f\n");
            ImGui::InputFloat("Air Friction", &m_airFriction, 0.f, 0.f, "%4.f\n");
            ImGui::EndGroup();
        }

        ImGui::End();        
    }

    const float getBoostLevel() const {return m_boostLevel;}

    const float getRotation() const {return m_rotation;}
    const float getThrottle() const {return m_throttle;}

    const Vector2 getVel() const {return m_vel;}
    const Vector2 getPos() const {return m_pos;}
};