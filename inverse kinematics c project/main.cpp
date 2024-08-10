#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int FRAMERATE_LIMIT = 144;

const int SEGMENT_AMOUNT = 4;
const int SEGMENT_LENGTH = 125;
const int SEGMENT_WIDTH = 25;

const float PI = 3.14159265358979323846;
const float TARGET_TOLERANCE = 0.001;

struct Segment {
    sf::Vector2f origin;
    sf::Vector2f dir;
};

float magnitude(sf::Vector2f vec) {
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

sf::Vector2f normalize(sf::Vector2f vec) {
    return sf::Vector2f(vec.x / magnitude(vec), vec.y / magnitude(vec));
}

float distance(sf::Vector2f a, sf::Vector2f b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

sf::Vector2f findEnd(Segment seg) {
    return seg.origin - seg.dir * float(SEGMENT_LENGTH);
}

float findAngle(sf::Vector2f vec) {
    return std::atan2(vec.y, vec.x) * (180 / PI);
}

sf::RectangleShape drawSegment(Segment seg) {
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(SEGMENT_WIDTH, SEGMENT_LENGTH));
    rect.setOrigin(SEGMENT_WIDTH / 2, SEGMENT_LENGTH);
    rect.setPosition(seg.origin);
    rect.setRotation(findAngle(seg.dir) - 90);

    return rect;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Lobotomy");
    window.setFramerateLimit(FRAMERATE_LIMIT);

    sf::Vector2f globalRoot(SCREEN_WIDTH / 2, SCREEN_HEIGHT);
    Segment segments[SEGMENT_AMOUNT];
    Segment seg;
    seg.origin = globalRoot;
    seg.dir = sf::Vector2f(0, 1);
    for (int i = 0; i < SEGMENT_AMOUNT; i++) {
        segments[i] = seg;
        seg.origin = findEnd(seg);
    }

    sf::Vector2f mousePos;
    sf::Vector2f currTarget;
    bool thresholdMet;
    int x;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        
        mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
        thresholdMet = false;

        // debugging code
        std::cout << mousePos.x << ", " << mousePos.y << std::endl;
        std::cout << distance(mousePos, globalRoot) << std::endl << std::endl;
        x = 0;

        while (!thresholdMet) {
            x++;

            for (int i = SEGMENT_AMOUNT - 1; i > -1; i--) {
                if (i == SEGMENT_AMOUNT - 1) {
                    currTarget = mousePos;
                }
                else {
                    currTarget = segments[i + 1].origin;
                }

                segments[i].dir = segments[i].origin - currTarget;
                segments[i].dir = segments[i].dir - (normalize(segments[i].dir) * float(SEGMENT_LENGTH));
                segments[i].origin = segments[i].origin - segments[i].dir;
                segments[i].dir = normalize(segments[i].origin - currTarget);
            }
            for (int i = 0; i < SEGMENT_AMOUNT; i++) {
                if (i == 0) {
                    currTarget = globalRoot;
                }
                else {
                    currTarget = findEnd(segments[i - 1]);
                }

                segments[i].dir = -normalize(findEnd(segments[i]) - currTarget);
                segments[i].origin = currTarget;
            }

            // debugging code
            if (x == 1000) {
                std::cout << distance(mousePos, findEnd(segments[SEGMENT_AMOUNT - 1])) << std::endl;
                x = 0;
            }

            if (distance(mousePos, globalRoot) < SEGMENT_LENGTH * SEGMENT_AMOUNT) {
                if (distance(mousePos, findEnd(segments[SEGMENT_AMOUNT - 1])) <= TARGET_TOLERANCE) {
                    thresholdMet = true;
                }
            }
            else {
                if (distance(globalRoot - (normalize(globalRoot - mousePos) * float(SEGMENT_LENGTH * SEGMENT_AMOUNT)), findEnd(segments[SEGMENT_AMOUNT - 1])) <= TARGET_TOLERANCE) {
                    thresholdMet = true;
                }
            }
        }

        for (int i = 0; i < SEGMENT_AMOUNT; i++) {
            window.draw(drawSegment(segments[i]));
        }

        window.display();
    }

    return 0;
}