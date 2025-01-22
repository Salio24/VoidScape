#include "BatchRenderer.hpp"
#include "App.hpp"
#include <cstddef>

BatchRenderer::BatchRenderer() : app_(nullptr) {

}

BatchRenderer::~BatchRenderer() {

}

App& BatchRenderer::app()  {
	if (app_ == nullptr) {
		app_ = &App::getInstance();
	}
	return *app_;
}
void BatchRenderer::StartUp(ShaderProgram* program, GLuint& PipelineProgramID) {
	if (QuadBuffer != nullptr) {
		std::cout << "BatchRenderer has been initialized twice. ERROR!" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	currentProgram = program;
	currentPipelineProgramID = PipelineProgramID;

	QuadBuffer = new Box[maxVertexCount];

	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glCreateBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, maxVertexCount * sizeof(Box), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, Position));

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, Color));

	glEnableVertexArrayAttrib(VAO, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, TexturePosition));

	glEnableVertexArrayAttrib(VAO, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, TextureIndex));

	glEnableVertexArrayAttrib(VAO, 4);
	glVertexAttribPointer(4, 1, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, FlyAngle));

	glEnableVertexArrayAttrib(VAO, 5);
	glVertexAttribPointer(5, 1, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, FlyOrientation));

	glEnableVertexArrayAttrib(VAO, 6);
	glVertexAttribPointer(6, 2, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, Center));

	uint32_t indices[maxIndexCount];
	uint32_t offset = 0;
	for (int i = 0; i < maxIndexCount; i += 6) {
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}
	glCreateBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void BatchRenderer::ShutDown() {
	glDeleteVertexArrays(1, &VAO); 
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

	delete[] QuadBuffer;
}

void BatchRenderer::BeginBatch(const glm::mat4& ProjectionMatrix) {
	QuadBufferPtr = QuadBuffer;
	currentProjectionMatrix = ProjectionMatrix;
}

void BatchRenderer::EndBatch() {
	GLsizeiptr size = (uint8_t*)QuadBufferPtr - (uint8_t*)QuadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, QuadBuffer);
}

void BatchRenderer::DrawInBatch(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	if (indexCount >= maxIndexCount) {
		EndBatch();
		Flush();
		BeginBatch(currentProjectionMatrix);
	}

	QuadBufferPtr->Position = {position.x, position.y};
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 0.0f, 0.0f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x + size.x, position.y };
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 0.25f, 0.0f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x + size.x, position.y + size.y};
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 0.25f, 0.25f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x, position.y + size.y };
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 0.0f, 0.25f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr++;

	indexCount += 6;
}

void BatchRenderer::DrawInBatch(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec2& textureSize, const glm::vec2& texturePosition, const bool& drawFliped, const glm::vec4& color) {
	if (indexCount >= maxIndexCount) {
		EndBatch();
		Flush();
		BeginBatch(currentProjectionMatrix);
	}

	if (drawFliped == true) {
		QuadBufferPtr->Position = { position.x, position.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x + size.x, position.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y + textureSize.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x, position.y + size.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y + textureSize.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;
	}
	else if (drawFliped == false) {
		QuadBufferPtr->Position = { position.x, position.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x + size.x, position.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y + textureSize.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x, position.y + size.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y + textureSize.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;
	}

	indexCount += 6;
}

void BatchRenderer::DrawInBatch(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec2& textureSize, const glm::vec2& texturePosition, float flyAngle, float flyOrientation) {
	if (indexCount >= maxIndexCount) {
		EndBatch();
		FlushFly();
		BeginBatch(currentProjectionMatrix);
	}

	QuadBufferPtr->Position = { position.x, position.y };
	QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y };
	QuadBufferPtr->TextureIndex = textureID;
	QuadBufferPtr->FlyAngle = flyAngle;
	QuadBufferPtr->FlyOrientation = flyOrientation;
	QuadBufferPtr->Center = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x + size.x, position.y };
	QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y };
	QuadBufferPtr->TextureIndex = textureID;
	QuadBufferPtr->FlyAngle = flyAngle;
	QuadBufferPtr->FlyOrientation = flyOrientation;
	QuadBufferPtr->Center = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
	QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y + textureSize.y };
	QuadBufferPtr->TextureIndex = textureID;
	QuadBufferPtr->FlyAngle = flyAngle;
	QuadBufferPtr->FlyOrientation = flyOrientation;
	QuadBufferPtr->Center = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x, position.y + size.y };
	QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y + textureSize.y };
	QuadBufferPtr->TextureIndex = textureID;
	QuadBufferPtr->FlyAngle = flyAngle;
	QuadBufferPtr->FlyOrientation = flyOrientation;
	QuadBufferPtr->Center = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
	QuadBufferPtr++;
	

	indexCount += 6;
}


void BatchRenderer::DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::vec4& color, const glm::mat4& ProjectionMatrix, const glm::mat4& ModelMatrix) {
	BeginBatch(ProjectionMatrix);
	DrawInBatch(position, size, color);
	EndBatch();
	Flush(ModelMatrix);
}

void BatchRenderer::DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::mat4& ProjectionMatrix, uint32_t textureID, const glm::vec2& textureSize, const glm::vec2& texturePosition, const glm::mat4& ModelMatrix, const bool& drawFliped) {
	BeginBatch(ProjectionMatrix);
	DrawInBatch(position, size, textureID, textureSize, texturePosition, drawFliped);
	EndBatch();
	Flush(ModelMatrix);
}

void BatchRenderer::Flush(const glm::mat4& ModelMatrix) {
	currentProgram->UseInPipeline(currentPipelineProgramID);
	currentProgram->SetMat4("uModelMatrix", ModelMatrix);
	currentProgram->SetMat4("uProjectionMatrix", currentProjectionMatrix);
	currentProgram->SetFloat("flyBlockBool", 0.0f);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	indexCount = 0;
}

void BatchRenderer::FlushFly() {
	currentProgram->UseInPipeline(currentPipelineProgramID);
	currentProgram->SetMat4("uModelMatrix", glm::mat4(1.0f));
	currentProgram->SetMat4("uProjectionMatrix", currentProjectionMatrix);
	currentProgram->SetFloat("flyBlockBool", 1.0f);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	indexCount = 0;
}

void BatchRenderer::UniformVariableLinkageAndPopulatingWithMatrix(const GLchar* uniformLocation, glm::mat4 matrix, const GLuint& PipelineProgram) {
	GLint MatrixLocation = glGetUniformLocation(PipelineProgram, uniformLocation);
	if (MatrixLocation >= 0) {
		glUniformMatrix4fv(MatrixLocation, 1, GL_FALSE, &matrix[0][0]);
	}
	else {
		std::cerr << "Failed to get uniform location of: " << uniformLocation << std::endl << "Exiting now" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void BatchRenderer::Test() {

	BeginBatch(app().mCamera.GetProjectionMatrix());
	DrawInBatch(glm::vec2(100.0f, 100.0f), glm::vec2(100.0f, 100.0f), glm::vec4(1.0f));
	EndBatch();
	
	currentProgram->UseInPipeline(currentPipelineProgramID);
	currentProgram->SetMat4("uModelMatrix", glm::mat4(1.0f));
	currentProgram->SetMat4("uProjectionMatrix", currentProjectionMatrix);

	currentProgram->SetVec2("center", glm::vec2(150.0f, 150.0f));
	currentProgram->SetFloat("angle", 45.0f);
	currentProgram->SetFloat("ad", 1.0f);


	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	indexCount = 0;

}