//
// Created by Harrand on 04/04/2019.
//

#ifndef TOPAZ_UNIFORM_HPP
#define TOPAZ_UNIFORM_HPP
#include "graphics/light.hpp"
#include "core/topaz.hpp"

#ifdef TOPAZ_OPENGL
/**
* Abstract. Not available for non-polymorphic use.
* I highly doubt you'll need to inherit from this anyway.
* Consider providing template specialisation to Uniform<T> if you need custom uniforms, which you may need in a later version of OpenGL which supports more primitives, perhaps.
*/
class UniformImplicit
{
public:
    UniformImplicit(GLuint shader_handle, std::string uniform_location);
    virtual GLuint get_shader_handle() const;
    virtual std::string_view get_uniform_location() const;
    /**
     * Pure Virtual
     */
    virtual void push() const = 0;
protected:
    /// Underlying OpenGL shader-handle.
    GLuint shader_handle;
    /// OpenGL uniform-location (name of the variable in GLSL).
    std::string uniform_location;
};

/**
 * Represent an OpenGL uniform in C++. Supports the following Topaz/C++ primitives:
 * bool, int, unsigned int, float, double, Vector2F, Vector3F, Vector4F, Matrix2x2, Matrix3x3, and Matrix4x4.
 * If the template argument is not any of these types, a static assertation will fail in Uniform<T>::push and emit a compiler error.
 * @tparam T - Type of the Uniform value
 */
template<class T>
class Uniform : public UniformImplicit
{
public:
    /**
     * Construct a Uniform from all specifications.
     * @param shader_handle - The OpenGL shader-handle referring to the Shader which this Uniform should belong to
     * @param uniform_location - OpenGL uniform-location (i.e name of the Uniform)
     * @param value - Value of the Uniform
     */
    Uniform<T>(GLuint shader_handle, std::string uniform_location, T value);
    /**
     * Uniforms are not copyable.
     * @param copy - N/A
     */
    Uniform<T>(const Uniform<T>& copy) = delete;
    /**
     * Construct a Uniform from an existing Uniform.
     * @param move - The existing Uniform to move from.
     */
    Uniform<T>(Uniform<T>&& move) = default;
    /**
     * Read-only access to the underlying Uniform's value.
     * @return - Value of the Uniform
     */
    const T& get_value() const;
    /**
     * Assign the value of the Uniform.
     * @param value - Desired new value of the Uniform
     */
    void set_value(T value);
    /**
     * Update all Uniform changes and have them affect all subsequent render-passes.
     */
    virtual void push() const final;
private:
    /// Underlying value.
    T value;
    /// Underlying OpenGL handle for this Uniform.
    GLint uniform_handle;
};

template<>
class Uniform<DirectionalLight> : public UniformImplicit
{
public:
    /**
     * Construct a DirectionalLight Uniform from all specifications.
     * @param shader_handle - The OpenGL shader-handle referring to the Shader which this Uniform should belong to
     * @param uniform_location - OpenGL uniform-location (i.e name of the Uniform)
     * @param value - Value of the Uniform
     */
    Uniform<DirectionalLight>(GLuint shader_handle, std::string uniform_location, DirectionalLight value);
    /**
     * Uniforms are not copyable.
     * @param copy - N/A
     */
    Uniform<DirectionalLight>(const Uniform<DirectionalLight>& copy) = delete;
    /**
     * Construct a Uniform from an existing Uniform.
     * @param move - The existing Uniform to move from.
     */
    Uniform<DirectionalLight>(Uniform<DirectionalLight>&& move) = default;
    /**
     * Read-only access to the underlying Uniform's value.
     * @return - Value of the Uniform
     */
    const DirectionalLight& get_value() const;
    /**
     * Assign the value of the Uniform.
     * @param value - Desired new value of the Uniform
     */
    void set_value(DirectionalLight value);
    /**
     * Update all Uniform changes and have them affect all subsequent render-passes.
     */
    virtual void push() const final;
private:
    /// Underlying value.
    DirectionalLight value;
    /// DirectionalLight is (direction, colour, power) so each needs their own uniform.
    GLint direction_uniform_handle;
    GLint colour_uniform_handle;
    GLint power_uniform_handle;
};

template<>
class Uniform<PointLight> : public UniformImplicit
{
public:
    /**
     * Construct a PointLight Uniform from all specifications.
     * @param shader_handle - The OpenGL shader-handle referring to the Shader which this Uniform should belong to
     * @param uniform_location - OpenGL uniform-location (i.e name of the Uniform)
     * @param value - Value of the Uniform
     */
    Uniform<PointLight>(GLuint shader_handle, std::string uniform_location, PointLight value);
    /**
     * Uniforms are not copyable.
     * @param copy - N/A
     */
    Uniform<PointLight>(const Uniform<PointLight>& copy) = delete;
    /**
     * Construct a Uniform from an existing Uniform.
     * @param move - The existing Uniform to move from.
     */
    Uniform<PointLight>(Uniform<PointLight>&& move) = default;
    /**
     * Read-only access to the underlying Uniform's value.
     * @return - Value of the Uniform
     */
    const PointLight& get_value() const;
    /**
     * Assign the value of the Uniform.
     * @param value - Desired new value of the Uniform
     */
    void set_value(PointLight value);
    /**
     * Update all Uniform changes and have them affect all subsequent render-passes.
     */
    virtual void push() const final;
private:
    /// Underlying value.
    PointLight value;
    /// DirectionalLight is (direction, colour, power) so each needs their own uniform.
    GLint position_uniform_handle;
    GLint colour_uniform_handle;
    GLint power_uniform_handle;
};
#endif

#include "platform_specific/uniform.inl"

#endif //TOPAZ_UNIFORM_HPP