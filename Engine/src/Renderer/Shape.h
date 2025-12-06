#pragma once

#include "Core/Core.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace ft {

    /// ===== Transform =====
    struct Transform {
        glm::vec2 position;
        glm::vec2 scale;
        float rotation;
        float zIndex;
		Transform(const glm::vec2& position = glm::vec2(0.0f), 
				  const glm::vec2& scale = glm::vec2(1.0f), 
						float rotation = 0.0f, 
						float zIndex = 0.0f) 
			: position(position), rotation(rotation), scale(scale), zIndex(zIndex) {}
    };

    enum class ShapeType { Rectangle, Ellipse, Triangle, Polygon, Line, Other };

    /// ===== Shape =====
    class Shape {
    public:
        Shape(const Transform& transform = Transform(), const glm::vec4& color = glm::vec4(1.0f), const ShapeType type = ShapeType::Other)
            : transform(transform), color(color), type(type) {}
        virtual ~Shape() = default;

        Transform transform;
        glm::vec4 color;

        std::vector<glm::vec2> modelVertices;
        std::vector<float> worldVertices;
        std::vector<uint32_t> indices;

        uint32_t vertexOffset, indexOffset;

        virtual void GenerateModel() = 0;

        void UpdateWorldVertices() {
            worldVertices.clear();
            worldVertices.reserve(modelVertices.size() * 2);

            float angle = glm::radians(transform.rotation);
            float c = cos(angle);
            float s = sin(angle);

            for (const auto& v : modelVertices) {
                // Scale
                float x = v.x * transform.scale.x;
                float y = v.y * transform.scale.y;

                // Rotate
                float xr = x * c - y * s;
                float yr = x * s + y * c;

                // Translate
                xr += transform.position.x;
                yr += transform.position.y;

                worldVertices.push_back(xr);
                worldVertices.push_back(yr);
            }
        }
	protected:
        ShapeType type;
    };


    /// ===== Rectangle =====
    class Rectangle : public Shape {
    public:
        Rectangle(const Transform& transform = Transform(), const glm::vec4& color = glm::vec4(1.0f))
            : Shape(transform, color, ShapeType::Rectangle) {
            GenerateModel();
            UpdateWorldVertices();
        }

		void GenerateModel() override {
			modelVertices = {
				{ -0.5, -0.5 },
				{  0.5, -0.5 },
				{  0.5,  0.5 },
				{ -0.5,  0.5 }
			};
			indices = { 0, 1, 2, 2, 3, 0 };
		}
    };

    /// ===== Ellipse =====
    class Ellipse : public Shape {
    public:
        Ellipse(const Transform& transform = Transform(), const glm::vec4& color = glm::vec4(1.0f))
            : Shape(transform, color, ShapeType::Ellipse) {
            GenerateModel();
            UpdateWorldVertices();
        }

        void GenerateModel() override {
            modelVertices = { { 0.0f, 0.0f } }; // Ellipse only needs the center point
            indices = { 0 };
        }
    };


    /// ===== Polygon =====
    class Polygon : public Shape {
    public:
        int sides;
        Polygon(int sides, const Transform& transform = Transform(), const glm::vec4& color = glm::vec4(1.0f))
            : sides(sides), Shape(transform, color, ShapeType::Polygon) {
            GenerateModel();
            UpdateWorldVertices();
        }

        void GenerateModel() override {
            modelVertices.clear();
            modelVertices.reserve(sides);
            
            float angleStep = 2.0f * glm::pi<float>() / static_cast<float>(sides);
            constexpr float startAngle = glm::pi<float>() / 2.0f;
            float r = 0.5f;
            
            for (int i = 0; i < sides; ++i) {
				float x = r * cos(i * angleStep + startAngle);
				float y = r * sin(i * angleStep + startAngle);
				modelVertices.emplace_back(x, y);
			}
            
            indices.clear();
            indices.reserve(3 * (sides - 2));
            for (int i = 1; i < sides - 1; ++i) {
				indices.push_back(0);
				indices.push_back(i);
				indices.push_back(i + 1);
			}
        }
    };

    class Line : public Shape {
    public:
        Line(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color = glm::vec4(1.0f))
            : Shape(Transform(), color, ShapeType::Line), start(start), end(end)
        {
            GenerateModel();
            UpdateWorldVertices();
        }

        void GenerateModel() override {
            glm::vec2 center = (start + end) * 0.5f;

            float length = glm::distance(start, end);
            float angle = atan2(end.y - start.y, end.x - start.x);

            modelVertices = {
                {-0.5f, 0.0f},
                { 0.5f, 0.0f}
            };

            transform.position = center;
            transform.rotation = glm::degrees(angle);
            transform.scale = glm::vec2(length, 1.0f);

            indices = { 0, 1 };
        }

        void SetStart(const glm::vec2& start) {
            this->start = start;
            GenerateModel();
        }

        void SetEnd(const glm::vec2& end) {
            this->end = end;
            GenerateModel();
        }

    private:
        glm::vec2 start;
        glm::vec2 end;
    };
}

