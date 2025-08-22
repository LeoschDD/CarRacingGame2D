#pragma once

#include <raylib.h>
#include <raymath.h>

#include <vector>
#include <deque>
#include <iomanip>

struct Trail
{
    Rectangle rectangle{};
    float rotation{};
};

class TrailManager
{
private:
    std::deque<Trail> m_trails;

    float m_trailTime{};
    float m_trailCounter{};
    size_t m_maxTrails{};

public:
    TrailManager(float trailTime, size_t maxTrails) 
        : m_trailTime(trailTime)
        , m_trailCounter(trailTime)
        , m_maxTrails(maxTrails)
    {}

    ~TrailManager() = default;

    void addTrail(const Vector2& forward,
                const Vector2& sideways,
                float forwardSpeed,
                float sidewaysSpeed,
                const Vector2& carSize,
                const Vector2& carPos,
                float rotation,
                bool handBrake,
                float dt)
    {
        m_trailCounter -= dt;

        const bool drifting = (fabsf(sidewaysSpeed) > fabsf(forwardSpeed) * 0.5f);

        if (!(drifting || (handBrake && forwardSpeed != 0.f)) || m_trailCounter > 0.0f) return;
        
        m_trailCounter = m_trailTime;

        const Vector2 fwdP = Vector2Scale(forward,  carSize.y * 0.8f);
        const Vector2 bwdP = Vector2Scale(forward, -carSize.y * 0.2f);
        const Vector2 rgtP = Vector2Scale(sideways,  carSize.x * 0.5f);
        const Vector2 lftP = Vector2Scale(sideways, -carSize.x * 0.5f);

        std::vector<Vector2> positions;
        positions.reserve(4);
        positions.emplace_back(Vector2Add(Vector2Add(fwdP, rgtP), carPos));
        positions.emplace_back(Vector2Add(Vector2Add(fwdP, lftP), carPos));
        positions.emplace_back(Vector2Add(Vector2Add(bwdP, rgtP), carPos));
        positions.emplace_back(Vector2Add(Vector2Add(bwdP, lftP), carPos));

        for (auto pos : positions)
        {   
            Trail trail;
            trail.rectangle = {pos.x, pos.y, carSize.x/8, carSize.x/8};
            trail.rotation = rotation;
            m_trails.push_back(trail);
        }      

        while (m_trails.size() > m_maxTrails) 
        {
            m_trails.pop_front();         
        }  
    }

    const std::deque<Trail>* getTrails() const {return &m_trails;}
};