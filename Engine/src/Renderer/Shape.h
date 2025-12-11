#pragma once

#include "Core/Core.h"
#include "Core/Log.h"
#include "Resources/AssetManager.h"

#include "Renderer/Texture.h"

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

    enum class ShapeType { Rectangle, Ellipse, Triangle, Polygon, Line, Other, TextureQuad };

    /// ===== Shape =====
    class Shape {
    public:
        Shape(const Transform& transform = Transform(), const glm::vec4& color = glm::vec4(1.0f), const ShapeType type = ShapeType::Other)
            : transform(transform), color(color), m_Type(type), m_ID(0) {}
        virtual ~Shape() = default;

        Transform transform;
        glm::vec4 color;

        std::vector<glm::vec2> modelVertices;
        std::vector<glm::vec2> worldVertices;
        std::vector<uint32_t> indices;

        uint32_t vertexOffset = 0, vertexByteOffset = 0, indexOffset = 0;

        virtual void GenerateModel() {};

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

                worldVertices.emplace_back(xr, yr);
            }

            m_Dirty = true;
        }

        uint32_t GetVertexByteSize() const { return sizeof(float) * worldVertices.size() * 2; }
        uint32_t GetVertexCount() const { return modelVertices.size(); }
        uint32_t GetIndexByteSize() const { return sizeof(uint32_t) * indices.size(); }
        ShapeType GetType() const { return m_Type; }
        uint32_t GetID() const { return m_ID; }

        void SetID(uint32_t id) {
            if (m_ID)
                FT_ENGINE_WARN("ID of shape {} already set.", m_ID);
            else
                m_ID = id;
        }
        bool IsDirty() const { return m_Dirty; }
        void ResetDirty() { m_Dirty = false; }
        void MarkDirty() { m_Dirty = true; }
	protected:
        ShapeType m_Type;
    private:
        bool m_Dirty = false;
        uint32_t m_ID;
    };


    /// ===== Rectangle =====
    class Rectangle : public Shape {
    public:
        Rectangle(const Transform& transform = Transform(), const glm::vec4& color = glm::vec4(1.0f))
            : Shape(transform, color, ShapeType::Rectangle){
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
			indices = { 0, 1, 2, 0, 2, 3 };
		}
    };

    /// ===== Ellipse =====
    class Ellipse : public Shape {
    public:
        Ellipse(float thickness = 0, float AA = 0.01, const Transform& transform = Transform(), const glm::vec4& color = glm::vec4(1.0f))
            : Shape(transform, color, ShapeType::Ellipse), thickness(thickness), AA(AA) {
            GenerateModel();
            UpdateWorldVertices();
        }

        float thickness, AA;

        void GenerateModel() override {
            modelVertices = {
                { -0.5, -0.5 },
                {  0.5, -0.5 },
                {  0.5,  0.5 },
                { -0.5,  0.5 }
            };
            indices = { 0, 1, 2, 0, 2, 3 };
        }
    };


    /// ===== Polygon =====
    class Polygon : public Shape {
    public:
        Polygon(int sides, const Transform& transform = Transform(), const glm::vec4& color = glm::vec4(1.0f))
            : m_Sides(sides), Shape(transform, color, ShapeType::Polygon) {
            GenerateModel();
            UpdateWorldVertices();
        }

        void GenerateModel() override {
            modelVertices.clear();
            modelVertices.reserve(m_Sides);
            
            float angleStep = 2.0f * glm::pi<float>() / static_cast<float>(m_Sides);
            constexpr float startAngle = glm::pi<float>() / 2.0f;
            float r = 0.5f;
            
            for (int i = 0; i < m_Sides; ++i) {
				float x = r * cos(i * angleStep + startAngle);
				float y = r * sin(i * angleStep + startAngle);
				modelVertices.emplace_back(x, y);
			}
            
            indices.clear();
            indices.reserve(3 * (m_Sides - 2));
            for (int i = 1; i < m_Sides - 1; ++i) {
				indices.push_back(0);
				indices.push_back(i);
				indices.push_back(i + 1);
			}
        }

        int GetSides() const { return m_Sides; }
    private:
        int m_Sides;
    };

    /// ===== Line =====
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

    /// ===== Texture Quad =====
    class TextureQuad : public Rectangle {
    public:
        TextureQuad(std::shared_ptr<Texture> texture, const Transform& transform = Transform(), const glm::vec4& color = glm::vec4(1.0f))
            : Rectangle(transform, color) {
            this->m_Type = ShapeType::TextureQuad;
            m_Texture = texture;
            GenerateModel();
            UpdateWorldVertices();
        }

        TextureQuad(const std::string& imagePath, const Transform& transform = Transform(), const glm::vec4& color = glm::vec4(1.0f))
            : TextureQuad(AssetManager::LoadTexture(imagePath), transform, color) {}

        Texture* GetTexture() const { return m_Texture.get(); }
        void const SetTexture(std::shared_ptr<Texture> texture) { m_Texture = texture; }
    private:
        std::shared_ptr<Texture> m_Texture;
    };
}

