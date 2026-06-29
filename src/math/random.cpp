#include <math/random.h>

glm::vec3 randomPosition(float min, float max) {
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_real_distribution<float> dist(min, max);

    return glm::vec3(
        dist(gen),
        dist(gen),
        dist(gen)
    );
}

std::random_device Random::random_device {};
std::mt19937       Random::random_engine (Random::random_device());

void Random::init() {
    // random_engine.seed(std::random_device()());
}

int Random::Int(const int min, const int max) {

    std::uniform_int_distribution<int> dist(min, max);
    return dist(random_engine);
}

float Random::Float(const float min, const float max) {

    std::uniform_real_distribution<float> dist(min, max);
    return dist(random_engine);
}

glm::vec3 Random::vec3(const float min, const float max) {

    std::uniform_real_distribution<float> dist(min, max);

    return glm::vec3(
        dist(random_engine),
        dist(random_engine),
        dist(random_engine)
    );
}