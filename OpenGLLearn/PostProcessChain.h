#pragma once
#include<vector>
#include"PostProcessPass.h"
class PostProcessChain
{
private:
	std::vector<PostProcessPass*> m_passes;
public:
	PostProcessChain() = default;
	~PostProcessChain() = default;
	PostProcessChain(PostProcessChain&) = delete;
	PostProcessChain& operator=(PostProcessChain&) = delete;
	PostProcessChain(PostProcessChain&& other) noexcept;
	PostProcessChain& operator=(PostProcessChain&& other) noexcept;
	void allocate(size_t size);
	void add(PostProcessPass& pass);
	void resize(int width, int height);
	void execute(const GLTexture2D& texture, const GLMesh& screen);

	[[nodiscard]] const GLTexture2D& output() const {
		return m_passes.back()->output();
	}
};

