#ifndef _SHADERMANAGER_
#define _SHADERMANAGER_

#ifndef _GL3W_
#define _GL3W_
  #include <GL\gl3w.h>
  #include <GLFW\glfw3.h>
#endif

#include <iostream>
#include <array>
#include <string>
#include <fstream>

#include "ShaderBaseModel.h"

template<class T, class...>
struct are_same : std::true_type {};

template<class T, class U, class... TT>
struct are_same<T, U, TT...> 
  : std::integral_constant < bool, std::is_same<T, U>::value && are_same<T, TT...>::value > 
{};

/* Usually used as Singleton. But for this small project it suffices to use it 
*  as a plain object in ParticleSystem, since it's the only instance in the whole
*  project.
* 
*  Note that not all the functionalities of this class are used in this project.
*/
class ShaderManager{
private:
  ShaderBaseModel _shaderData;
  std::array<bool, GL_MAX_UNIFORM_BUFFER_BINDINGS> _ubo_points;

  // static ShaderManager* _instance;

  // ShaderManager();

  std::string   getFileContents(const std::string&) const;

  template<typename T, typename... Args>
  void checkForHomogenousTypes(){
    static_assert(are_same<T, Args...>::value, "The arguments in the uniform loading method are not of the same type!");
  }

  template<typename... Args>
  void uploadUniforms(const GLuint uniID, Args... args);

  const std::string errVal(GLenum error);

public:

  ShaderManager();
  ShaderManager(ShaderManager&) = delete;
  ShaderManager(ShaderManager&&) = delete;
  ShaderManager& operator=(ShaderManager&) = delete;
  ShaderManager& operator=(ShaderManager&&) = delete;

  void  loadShader(const std::string& filename, const std::string& shaderKey, const GLenum type);
  void  attachShader(const std::string& shaderKey, const std::string& shaderProgramKey);
  void  detachShader(const std::string& shaderKey, const std::string& shaderProgramKey);

  GLuint  createProgram(const std::string& shaderProgramKey);
  GLuint  createProgram();

  void    useProgram(const std::string& shaderProgramKey);
  void    useProgram(const GLuint shaderProgramID);

  void    linkProgram(const std::string& shaderProgramKey);
  void    linkProgram(const GLuint& shaderProgramID);

  GLuint  getShaderID(const std::string& shaderKey);
  GLuint  getShaderProgramID(const std::string& shaderProgramKey);

  GLuint  occupyUBOBindingPoint();
  void    unoccupyUBOBindingPoint(GLuint point);

  void  deleteProgram(const std::string& shaderProgramKey);
  void  deleteProgram(const GLuint& shaderProgramID);

  void  deleteShader(const std::string& shaderKey);
  void  deleteShader(const GLuint& shaderID);

  void  resetProgram();

  void  loadMatrix4(const std::string& shaderProgram, const std::string& name, const GLfloat* value);
  void  loadMatrix4(const GLuint shaderProgramID, const std::string& name, const GLfloat* value);

  void  getBufferVariableIndices(const std::string& shaderProgram, const int length, const GLchar** names, GLint* indices);


  template<typename... Args>
  void loadUniform(const GLuint shaderProgramID, const std::string& name, Args... args){
    checkForHomogenousTypes<Args...>();

    glGetError();

    int uniID = glGetUniformLocation(shaderProgramID, name.c_str());

    if (glGetError() != GL_NO_ERROR || uniID == -1){
      std::cerr << "ERROR: Could not get " + name + "-uniform-location!" << std::endl;
      return;
    }

    uploadUniforms(uniID, args...);

    if (glGetError() != GL_NO_ERROR){
      std::cerr << "ERROR: Could not update " + name + "-uniform-location!" << std::endl;
      return;
    }
  }

  template<typename... Args>
  void loadUniform(const std::string& shaderProgram, const std::string& name, Args... args){
    checkForHomogenousTypes<Args...>();

    glGetError();

    int uniID = glGetUniformLocation(getShaderProgramID(shaderProgram), name.c_str());

    if (glGetError() != GL_NO_ERROR){
      std::cerr << "ERROR: Could not get " + name + "-uniform-location!" << std::endl;
    }

    uploadUniforms(uniID, args...);

    if (glGetError() != GL_NO_ERROR){
      std::cerr << "ERROR: Could not update " + name + "-uniform-location!" << std::endl;
    }
  }

  // static  ShaderManager* Inst();
};


#endif