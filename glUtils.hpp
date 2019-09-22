#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <vector>

namespace Needle {
    namespace glUtils {
        using glID = GLuint;

        struct glObject {
            virtual ~glObject() = default;
            glObject(const glObject&) = default;
            glID id() const noexcept { return mID; }
        protected:
            explicit glObject() : mID(0) {}
            glID mID;
        };

        namespace Shader {
            const char* const vertShader =
                "#version 330 core\n"
                "layout (location = 0) in vec3 in_pos;"
                "layout (location = 1) in vec3 in_color;"
                "out vec3 v_color;"
                "uniform mat4 model;"
                "uniform mat4 view;"
                "uniform mat4 projection;"
                "void main() {"
                "    gl_Position = projection * view * model * vec4(in_pos, 1);"
                "    v_color = in_color;"
                "}";

            const char* const fragShader =
                "#version 330 core\n"
                "in vec3 v_color;"
                "out vec4 out_color;"
                "void main() {"
                "    out_color = vec4(v_color, 1);"
                "}";

            class Source final : public glObject {
            public:
                Source(const char* _code, const GLenum _type) {
                    mID = glCreateShader(_type);
                    glShaderSource(mID, 1, &_code, nullptr);
                }

                ~Source() {
                    glDeleteShader(mID);
                }

                void compile() const {
                    glCompileShader(mID);
                }
            };

            class Program final : public glObject {
            public:
                Program() {
                    mID = glCreateProgram();
                }

                void attachShader(const Source& _shader) const {
                    glAttachShader(mID, _shader.id());
                }

                void link() const {
                    glLinkProgram(mID);
                }

                void useSelf() const {
                    glUseProgram(mID);
                }

                void setUniform(const char* _uniform, const glm::mat4x4& _value) const {
                    glUniformMatrix4fv(uniformLocation(_uniform), 1, GL_FALSE, &_value[0][0]);
                }

            private:
                glID uniformLocation(const char* _uniform) const {
                    return glGetUniformLocation(mID, _uniform);
                }
            };
        }

        class VertexArray final : public glObject {
        public:
            VertexArray() {
                glGenVertexArrays(1, &mID);
            }

            ~VertexArray() {
                glDeleteVertexArrays(1, &mID);
            }

            void bind() const {
                glBindVertexArray(mID);
            }

            void unbind() const {
                glBindVertexArray(0);
            }
        };

        template<typename _Ty, GLenum _Target>
        class BufferObject final : public glObject {
        public:
            BufferObject() {
                glGenBuffers(1, &mID);
            }

            ~BufferObject() {
                glDeleteBuffers(1, &mID);
            }

            constexpr GLenum bindTarget() const noexcept {
                return _Target;
            }

            void bind() const {
                glBindBuffer(_Target, mID);
            }

            void unbind() const {
                glBindBuffer(_Target, 0);
            }

            void fillData(const std::vector<_Ty>& _data, const GLenum _usage = GL_STATIC_DRAW) const {
                bind();
                glBufferData(_Target, _data.size() * sizeof(_Ty), _data.data(), _usage);
            }

            void fillData(std::vector<_Ty>&& _data, const GLenum _usage = GL_STATIC_DRAW) const {
                bind();
                glBufferData(_Target, _data.size() * sizeof(_Ty), _data.data(), _usage);
            }
        };

        using VertexBuffer = BufferObject<float, GL_ARRAY_BUFFER>;

        inline void linkVertexAttributes(const unsigned _layoutLocation,
                                         const unsigned _numComponents,
                                         const unsigned _stride,
                                         const unsigned _offset) {
            glVertexAttribPointer(_layoutLocation,
                                  _numComponents,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  _stride * sizeof(float),
                                  reinterpret_cast<void*>(_offset * sizeof(float)));
            glEnableVertexAttribArray(_layoutLocation);
        }
    }
}
