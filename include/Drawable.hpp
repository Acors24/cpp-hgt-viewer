#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include <epoxy/gl.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

class Drawable {
public:
    std::vector<glm::vec3> vertices;

    virtual ~Drawable() = default;

    virtual void draw(const glm::mat4 &view,
                      const glm::mat4 &projection) const = 0;

    void initialize();

    void recompileShaders() const;

protected:
    virtual void initializeMesh() = 0;
    virtual std::filesystem::path getShaderPath() const = 0;
    virtual bool &isInitialized() = 0;

    virtual std::vector<unsigned> &getIndices() = 0;

    virtual GLuint &getVao() const = 0;
    virtual GLuint &getVbo() const = 0;
    virtual GLuint &getEbo() const = 0;
    virtual GLuint &getShaderProgram() const = 0;

    void bind() const;

private:
    void setBuffers();
    void compileShaders() const;
};

#endif