#include "ProcMeshGenerator.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace ProcMeshGenerator
{
	GLMeshData createQuad(float width, float height)
	{
		GLMeshData data;

		const float halfWidth = width * 0.5f;
		const float halfHeight = height * 0.5f;

		// XY平面上の四角形。正面は+Z方向。
		data.vertices = {
			{
				{ -halfWidth, -halfHeight, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 0.0f, 0.0f }
			},
			{
				{ halfWidth, -halfHeight, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f }
			},
			{
				{ halfWidth, halfHeight, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f }
			},
			{
				{ -halfWidth, halfHeight, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f }
			}
		};

		data.indices = {
			0, 1, 2,
			0, 2, 3
		};

		data.primitiveMode = GL_TRIANGLES;

		return data;
	}

	GLMeshData createCube(float size)
	{
		GLMeshData data;

		const float h = size * 0.5f;

		/*
		 * 頂点を8個で共有すると、各角の法線を1つに決められない。
		 * 面ごとに独立した法線とUVを持たせるため、6面×4頂点の24頂点。
		 */
		data.vertices = {
			// Front (+Z)
			{ { -h, -h,  h }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
			{ {  h, -h,  h }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
			{ {  h,  h,  h }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
			{ { -h,  h,  h }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },

			// Back (-Z)
			{ {  h, -h, -h }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
			{ { -h, -h, -h }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
			{ { -h,  h, -h }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
			{ {  h,  h, -h }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },

			// Right (+X)
			{ { h, -h,  h }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
			{ { h, -h, -h }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
			{ { h,  h, -h }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
			{ { h,  h,  h }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },

			// Left (-X)
			{ { -h, -h, -h }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
			{ { -h, -h,  h }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
			{ { -h,  h,  h }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
			{ { -h,  h, -h }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },

			// Top (+Y)
			{ { -h, h,  h }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
			{ {  h, h,  h }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
			{ {  h, h, -h }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
			{ { -h, h, -h }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },

			// Bottom (-Y)
			{ { -h, -h, -h }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
			{ {  h, -h, -h }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
			{ {  h, -h,  h }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
			{ { -h, -h,  h }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } }
		};

		data.indices.reserve(36);

		for (uint32_t face = 0; face < 6; ++face)
		{
			const uint32_t base = face * 4;

			data.indices.insert(
				data.indices.end(),
				{
					base + 0, base + 1, base + 2,
					base + 0, base + 2, base + 3
				}
			);
		}

		data.primitiveMode = GL_TRIANGLES;

		return data;
	}

	GLMeshData createSphere(
		float radius,
		uint32_t slices,
		uint32_t stacks
	)
	{
		GLMeshData data;

		slices = std::max(slices, 3u);
		stacks = std::max(stacks, 2u);
		radius = std::max(radius, 0.0f);

		const uint32_t verticesPerRow = slices + 1;

		data.vertices.reserve(
			static_cast<size_t>(stacks + 1) *
			static_cast<size_t>(verticesPerRow)
		);

		data.indices.reserve(
			static_cast<size_t>(stacks) *
			static_cast<size_t>(slices) *
			6
		);

		constexpr float pi = std::numbers::pi_v<float>;
		constexpr float twoPi = 2.0f * pi;

		for (uint32_t stack = 0; stack <= stacks; ++stack)
		{
			const float v =
				static_cast<float>(stack) /
				static_cast<float>(stacks);

			const float phi = v * pi;

			const float sinPhi = std::sin(phi);
			const float cosPhi = std::cos(phi);

			for (uint32_t slice = 0; slice <= slices; ++slice)
			{
				const float u =
					static_cast<float>(slice) /
					static_cast<float>(slices);

				const float theta = u * twoPi;

				const float sinTheta = std::sin(theta);
				const float cosTheta = std::cos(theta);

				const glm::vec3 normal = {
					sinPhi * cosTheta,
					cosPhi,
					sinPhi * sinTheta
				};

				Vertex vertex;
				vertex.position = normal * radius;
				vertex.normal = normal;
				vertex.uv = { u, 1.0f - v };

				data.vertices.push_back(vertex);
			}
		}

		for (uint32_t stack = 0; stack < stacks; ++stack)
		{
			for (uint32_t slice = 0; slice < slices; ++slice)
			{
				const uint32_t topLeft =
					stack * verticesPerRow + slice;

				const uint32_t bottomLeft =
					(stack + 1) * verticesPerRow + slice;

				const uint32_t topRight = topLeft + 1;
				const uint32_t bottomRight = bottomLeft + 1;

				data.indices.push_back(topLeft);
				data.indices.push_back(bottomRight);
				data.indices.push_back(bottomLeft);

				data.indices.push_back(topLeft);
				data.indices.push_back(topRight);
				data.indices.push_back(bottomRight);
			}
		}

		data.primitiveMode = GL_TRIANGLES;

		return data;
	}

	GLMeshData createPlane(
		float width,
		float depth,
		uint32_t xSegments,
		uint32_t zSegments
	)
	{
		GLMeshData data;

		xSegments = std::max(xSegments, 1u);
		zSegments = std::max(zSegments, 1u);

		const uint32_t verticesPerRow = xSegments + 1;

		data.vertices.reserve(
			static_cast<size_t>(xSegments + 1) *
			static_cast<size_t>(zSegments + 1)
		);

		data.indices.reserve(
			static_cast<size_t>(xSegments) *
			static_cast<size_t>(zSegments) *
			6
		);

		for (uint32_t z = 0; z <= zSegments; ++z)
		{
			const float v =
				static_cast<float>(z) /
				static_cast<float>(zSegments);

			const float positionZ = (v - 0.5f) * depth;

			for (uint32_t x = 0; x <= xSegments; ++x)
			{
				const float u =
					static_cast<float>(x) /
					static_cast<float>(xSegments);

				const float positionX = (u - 0.5f) * width;

				data.vertices.push_back({
					{ positionX, 0.0f, positionZ },
					{ 0.0f, 1.0f, 0.0f },
					{ u, 1.0f - v }
					});
			}
		}

		for (uint32_t z = 0; z < zSegments; ++z)
		{
			for (uint32_t x = 0; x < xSegments; ++x)
			{
				const uint32_t topLeft =
					z * verticesPerRow + x;

				const uint32_t bottomLeft =
					(z + 1) * verticesPerRow + x;

				const uint32_t topRight = topLeft + 1;
				const uint32_t bottomRight = bottomLeft + 1;

				// +Y方向から見て反時計回り。
				data.indices.push_back(topLeft);
				data.indices.push_back(bottomLeft);
				data.indices.push_back(bottomRight);

				data.indices.push_back(topLeft);
				data.indices.push_back(bottomRight);
				data.indices.push_back(topRight);
			}
		}

		data.primitiveMode = GL_TRIANGLES;

		return data;
	}
}