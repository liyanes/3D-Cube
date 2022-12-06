#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glext.h"
#include "cube.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#include "config.h"

using namespace glExt;
using namespace std;
using namespace mycube;
using namespace mycube::cubeSolver;

double lastClickX, lastClickY;
double lastClickTime = 0.0;
double lastX, lastY ,fov = 45.0f;
bool leftkeyPress = false;
float distance = 10.0f;
int window_width = 1920, window_height = 1920;

struct {
    unsigned long defaultLevel;
    float spinRate;
    bool useTexture;
    std::string textures[6];
    std::string backgroundMusic;
    bool useRGBA;
} thisConfig;
bool useTexture;

//int posView, posProj;
int posProAndView,posTInvModel;
int posViewPos;
glm::mat4 view,projection;

float rotate_rate = 3.5f;
constexpr auto getRateTime = []() ->float {
    return glm::radians(90.0f) / rotate_rate;
};

singleCubeFace* _pickface = nullptr;
bool lastFramePicked = false;
// Ö¸¶¨ÊÇ·ñÔÊÐíÈý½×Ä§·½µÄÌØÊâ°´¼ü
bool enableLevel3KeyMap = false;

//fpsCamera _camera(glm::vec3(0.0f,0.0f,10.0f));
QuatCamera _camera(glm::vec3(0.0f, 0.0f, 10.0f));

mycube::Cube* _tcube;
mycube::cubeSolver::solver* solver;
program* _tprogram;
window* _twindow;
texture_2D* texts;

void mouseCallback(window& _window, int button, int action, int mode) {
    auto _curpos = _window.getCursorPos();
    crood<int> curpos = { (int)_curpos.x,(int)_curpos.y };
    double posMove = (_window.getCursorPos() - glExt::crood<double>{lastClickX, lastClickY}).length2();
    double timeClickDiff = glfwGetTime() - lastClickTime;
    if (button == 0 && action == 0 && (posMove==0.0 || (timeClickDiff <= 0.2 && posMove / timeClickDiff >= 10.0))) {
        lastClickTime = 0;
        auto curpos = _twindow->getCursorPos();
        const singleCubeFace* _face = ::solver->getCurrentPickFace(*_twindow, (int)curpos.x, (int)(window_height - curpos.y), projection * view);
        if (_face && !lastFramePicked) {
            if (_pickface) {
                auto curpos = _twindow->getCursorPos();
                const singleCubeFace* _face = ::solver->getCurrentPickFace(*_twindow, curpos.x, window_height - curpos.y, projection * view);
                if (_face) {
                    singleCubeFace* temp = &const_cast<singleCubeFace*>(_pickface)->inSingleCube->findFace(_face->nowFace);
                    if (temp->outShow == true) {
                        if (_face->inSingleCube != _pickface->inSingleCube)
                            ::solver->rotateByFace(*temp, *const_cast<singleCubeFace*>(_face), rotate_rate);
                        _pickface = nullptr;
                        lastFramePicked = true;
                    }
                    else {
                        _pickface = const_cast<singleCubeFace*>(_face);
                    }
                }
            }
            else {
                _pickface = const_cast<singleCubeFace*>(_face);
            }
        }
        else {
            _pickface = nullptr;
        }
    }
    if (button == 0 && action == 0) {
        lastFramePicked = false;
    }
    leftkeyPress = (bool)action;
    if (action) {
        lastClickX = curpos.x;
        lastClickY = curpos.y;
    }
    if (button == 0 && action == 1) {
        lastClickTime = glfwGetTime();
    }
}

void cursorCallback(window&, double xpos,double ypos){
    if (leftkeyPress) {
        //_camera.sorround((float)(xpos - lastX) , (float)(ypos - lastY) );
        _camera.sorround((float)(xpos - lastX) , (float)(ypos - lastY) );

        //yaw += (float)(sensitivity * (xpos - lastX));
        //pitch += (float)(sensitivity * (ypos - lastY));
        //if (pitch > 89.0f) pitch = 89.0f;
        //else if (pitch < -89.0f) pitch = -89.0f;

        //_camera.setEulerAngle(- glm::radians(pitch), glm::radians(yaw), 10.0f, false);
    }
    lastX = xpos;
    lastY = ypos;
}

void scrollCallback(window&, double xoffset, double yoffset) {
    fov -= yoffset;
    if (fov > 45.0) fov = 45.0;
    else if (fov < 1.0) fov = 1.0;
}

void frameBufferSizeCallback(window&, int width, int height) {
    if (width && height) {
        window_width = width;
        window_height = height;
        glViewport(0, 0, width, height);
        ::solver->finializePickMode();
        ::solver->initPickMode(*_twindow);
    }
}

void keyCallback(window& _win, int key, int scancode, int action, int mods) {
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9 && (mods & GLFW_MOD_CONTROL && !(mods & GLFW_MOD_ALT) && !(mods & GLFW_MOD_SHIFT))) {
        _tprogram->use();
        _tcube->~Cube();
        new (_tcube)mycube::Cube(key - GLFW_KEY_0, 2.0f, *_tprogram); 
        _pickface = nullptr;
        _tcube->frameCallback = [](Cube::animateFrame& _frame)->bool {
            if (!PlaySound(L"resources\\rotate.wav", 0, SND_ASYNC | SND_FILENAME)) {
                wstringstream ss(L"´íÎó");
                ss << GetLastError();
                MessageBox(NULL, ss.str().c_str(), L"´íÎó", MB_OK);
            }
            return true;
        };
    }

    const auto numPressed = [key, action, mods]()->int {
        if (action != GLFW_PRESS) return -1;
        if (mods & GLFW_MOD_CONTROL) return -1;
        if (GLFW_KEY_0 <= key && key <= GLFW_KEY_9) return key - GLFW_KEY_0;
        return -1;
    };
    const bool shifted = mods & (GLFW_MOD_SHIFT);
    const bool ctrled = mods & GLFW_MOD_CONTROL;

    if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
        enableLevel3KeyMap = !enableLevel3KeyMap;
        return;
    }
    if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
        ::solver->distribute(30, rotate_rate);
        return;
    }
    if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
        _tcube->frameClear();
        return;
    }
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS){
        if (thisConfig.useTexture) useTexture = !useTexture;
    }
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        _twindow->setFullScreen(!_twindow->isFullScreen());
    }
    using namespace mycube::cubeSolver;
    
    if (!(enableLevel3KeyMap && _tcube->getLevel() == 3)) {
        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getRight(),glm::radians(-90.f) }, getRateTime());
        }
        else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getRight(),glm::radians(90.f) }, getRateTime());
        }
        else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getUp(),glm::radians(-90.f) }, getRateTime());
        }
        else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getUp(),glm::radians(90.f) }, getRateTime());
        }
        else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getFront(),glm::radians(-90.f) }, getRateTime());
        }
        else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getFront(),glm::radians(90.f) }, getRateTime());
        }

        int presssdNum = numPressed();
        if (presssdNum > (int)_tcube->getLevel()) { return; }
        if (presssdNum == 0 || presssdNum == -1) return;
        if (_win.getKey(GLFW_KEY_U)) {
            ::solver->frameRequire((mods & GLFW_MOD_SHIFT) ? solver::relativeAxis::down : solver::relativeAxis::up, presssdNum - 1,rotate_rate);
        }
        else if (_win.getKey(GLFW_KEY_I)) {
            ::solver->frameRequire((mods & GLFW_MOD_SHIFT) ? solver::relativeAxis::left : solver::relativeAxis::right, presssdNum - 1, rotate_rate);
        }
        else if (_win.getKey(GLFW_KEY_O)) {
            ::solver->frameRequire((mods & GLFW_MOD_SHIFT) ? solver::relativeAxis::back : solver::relativeAxis::front, presssdNum - 1, rotate_rate);
        }
    }
    else {
        if (action != GLFW_PRESS) return;
        switch (key) {
        case GLFW_KEY_R:
            if (!ctrled) {
                if (!shifted) ::solver->frameRequire(solver::relativeAxis::left, 0,rotate_rate);
                else ::solver->frameRequire(solver::relativeAxis::right, 2, rotate_rate);
            }
            else {
                if (!shifted) {
                    ::solver->frameRequire(solver::relativeAxis::right, 0, rotate_rate);
                    ::solver->rotateToFace(faceRotate::down, getRateTime());
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::left, 2, rotate_rate);
                    ::solver->rotateToFace(faceRotate::up, getRateTime());
                }
            }
            break;
        case GLFW_KEY_L:
            if (!ctrled) {
                if (!shifted) ::solver->frameRequire(solver::relativeAxis::right, 0, rotate_rate);
                else ::solver->frameRequire(solver::relativeAxis::left, 2, rotate_rate);
            }
            else {
                if (!shifted) {
                    ::solver->frameRequire(solver::relativeAxis::left, 0, rotate_rate);
                    ::solver->rotateToFace(faceRotate::up, getRateTime());
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::right, 2, rotate_rate);
                    ::solver->rotateToFace(faceRotate::down, getRateTime());
                }
            }
            break;
        case GLFW_KEY_U:
            if (!ctrled) {
                if (!shifted) ::solver->frameRequire(solver::relativeAxis::down, 0, rotate_rate);
                else ::solver->frameRequire(solver::relativeAxis::up, 2, rotate_rate);
            }
            else {
                if (!shifted) {
                    ::solver->frameRequire(solver::relativeAxis::up, 0, rotate_rate);
                    ::solver->rotateToFace(faceRotate::right, getRateTime());
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::down, 2, rotate_rate);
                    ::solver->rotateToFace(faceRotate::left, getRateTime());
                }
            }
            break;
        case GLFW_KEY_D:
            if (!ctrled) {
                if (!shifted) ::solver->frameRequire(solver::relativeAxis::up, 0, rotate_rate);
                else ::solver->frameRequire(solver::relativeAxis::down, 2, rotate_rate);
            }
            else {
                if (!shifted) {
                    ::solver->frameRequire(solver::relativeAxis::down, 0, rotate_rate);
                    ::solver->rotateToFace(faceRotate::left, getRateTime());
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::up, 2, rotate_rate);
                    ::solver->rotateToFace(faceRotate::right, getRateTime());
                }
            }
            break;
        case GLFW_KEY_F:
            if (!ctrled) {
                if (!shifted) ::solver->frameRequire(solver::relativeAxis::front, 0, rotate_rate);
                else ::solver->frameRequire(solver::relativeAxis::back, 2, rotate_rate);
            }
            else {
                if (!shifted) {
                    ::solver->frameRequire(solver::relativeAxis::back, 0, rotate_rate);
                    _camera.requireRotate({camera::getMajorFaceVec(_camera.getFront()),glm::radians(-90.f)}, getRateTime());
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::front, 2, rotate_rate);
                    _camera.requireRotate({ camera::getMajorFaceVec(_camera.getFront()),glm::radians(90.f) }, getRateTime());
                }
            }
            break;
        case GLFW_KEY_B:
            if (!ctrled) {
                if (!shifted) ::solver->frameRequire(solver::relativeAxis::back, 0, rotate_rate);
                else ::solver->frameRequire(solver::relativeAxis::front, 2, rotate_rate);
            }
            else {
                if (!shifted) {
                    ::solver->frameRequire(solver::relativeAxis::front, 0, rotate_rate);
                    _camera.requireRotate({ camera::getMajorFaceVec(_camera.getFront()),glm::radians(90.f) }, getRateTime());
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::back, 2, rotate_rate);
                    _camera.requireRotate({ camera::getMajorFaceVec(_camera.getFront()),glm::radians(-90.f) }, getRateTime());
                }
            }
            break;
        case GLFW_KEY_M:
            if (!shifted) ::solver->frameRequire(solver::relativeAxis::right, 1, rotate_rate);
            else ::solver->frameRequire(solver::relativeAxis::left, 1, rotate_rate);
            break;
        case GLFW_KEY_X:
            if (!shifted) ::solver->rotateToFace(faceRotate::down, getRateTime());
            else ::solver->rotateToFace(faceRotate::up, getRateTime());
            break;
        case GLFW_KEY_Y:
            if (!shifted) ::solver->rotateToFace(faceRotate::right, getRateTime());
            else ::solver->rotateToFace(faceRotate::left, getRateTime());
            break;
        case GLFW_KEY_Z:
            if (!shifted) _camera.requireRotate({ camera::getMajorFaceVec(_camera.getFront()),glm::radians(-90.f) }, getRateTime());
            else _camera.requireRotate({ camera::getMajorFaceVec(_camera.getFront()),glm::radians(90.f) }, getRateTime());
            break;
        }
    }

}

bool loadConfig() {
    ct_ST_config* config = NULL;
    if (ct_config_load("config.conf", &config)) {
        return false;
    }
    auto cubeSect = ct_config_section_find(config, "cube");
    thisConfig.defaultLevel = std::strtoul(ct_config_variable_find(cubeSect, "defaultLevel")->value,NULL,10);
    thisConfig.spinRate = std::strtof(ct_config_variable_find(cubeSect, "spinRate")->value, NULL);
    useTexture = thisConfig.useTexture = std::strtol(ct_config_variable_find(cubeSect, "useTexture")->value, NULL, 10);
    if (thisConfig.useTexture) {
        for (unsigned i = 0; i < 6; i++) {
            stringstream ss;
            ss << "textures" << i;
            thisConfig.textures[i] = string(ct_config_variable_find(cubeSect, ss.str().c_str())->value);
        }
        thisConfig.useRGBA = std::strtol(ct_config_variable_find(cubeSect, "useRGBA")->value, NULL, 10);
    }
    thisConfig.backgroundMusic = ct_config_variable_find(cubeSect, "bgm")->value;
    return true;
}

#if defined(_CONSOLE) && _CONSOLE
int main(int argc, char *argv[]){
#else
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR pCmdLine,
    _In_ int nCmdShow) {
#endif
    srand((unsigned)time(NULL));

    if (!loadConfig()) {
        MessageBox(NULL, L"ÅäÖÃÎÄ¼þ¶ÁÐ´´íÎó", L"´íÎó", MB_ICONERROR | MB_OK);
        return -1;
    };
    rotate_rate = thisConfig.spinRate;

    initializer init;

	window _window = window(window_width, window_height, "3D-Cube"/*,true,glfwGetMonitors(&count)[0]*/);
    _window.setCursorCallback(cursorCallback);
    _window.setMouseCallback(mouseCallback);
    _window.setScrollCallback(scrollCallback);
    _window.setKeyCallback(keyCallback);
    _window.setFramebufferSizeCallback(frameBufferSizeCallback);

    _twindow = &_window;

    program _program;

    _tcube = new mycube::Cube(thisConfig.defaultLevel, 2.0f,_program);
    _tprogram = &_program;


    program _sprogram("#version 330 core\n"
        "layout(location = 0) in vec3 aPos;"
        "out vec3 TexCoords;"
        "uniform mat4 stdmat;"

        "void main()"
        "{"
        "TexCoords = aPos;"
        "gl_Position = stdmat * vec4(aPos, 1.0);"
        "}",
        "#version 330 core\n"
        "out vec4 FragColor;"
        "in vec3 TexCoords;"
        "uniform samplerCube skybox;"
        "void main()"
        "{"
        "FragColor = texture(skybox, TexCoords);"
        "}");
    _sprogram.use();
    _sprogram.uniform(_sprogram.getUniformPos("skybox"), 0);
    texture_Cube _world;
    texts = thisConfig.useTexture ? new texture_2D[6] : NULL;
    std::array<texture*, 6> _textsmap;
    for (unsigned i = 0; i < 6; i++) {
        _textsmap[i] = texts + i;
    }
    _world.bind();
    _world.loadFromImage(camera::axis::x_positive, image("resources\\skybox\\right.jpg", false), GL_RGB, GL_RGB, 0, GL_UNSIGNED_BYTE, 1);
    _world.loadFromImage(camera::axis::x_negative, image("resources\\skybox\\left.jpg"), GL_RGB, GL_RGB, 0, GL_UNSIGNED_BYTE, 1);
    _world.loadFromImage(camera::axis::y_positive, image("resources\\skybox\\top.jpg"), GL_RGB, GL_RGB, 0, GL_UNSIGNED_BYTE, 1);
    _world.loadFromImage(camera::axis::y_negative, image("resources\\skybox\\bottom.jpg"), GL_RGB, GL_RGB, 0, GL_UNSIGNED_BYTE, 1);
    _world.loadFromImage(camera::axis::z_positive, image("resources\\skybox\\front.jpg"), GL_RGB, GL_RGB, 0, GL_UNSIGNED_BYTE, 1);
    _world.loadFromImage(camera::axis::z_negative, image("resources\\skybox\\back.jpg"), GL_RGB, GL_RGB, 0, GL_UNSIGNED_BYTE, 1);
    if (texts) {
        for (unsigned i = 0; i < 6; i++) {
            texts[i].loadFromImage(image(thisConfig.textures[i].c_str()),thisConfig.useRGBA ? GL_RGBA : GL_RGB, thisConfig.useRGBA ? GL_RGBA : GL_RGB,0,GL_UNSIGNED_BYTE);
            texts[i].setFilter(texture::ope_mag, texture::fil_linear);
            texts[i].setFilter(texture::ope_min, texture::fil_linear);
            texts[i].setSurrounding(texture::wrap_r, texture::surr_edge);
            texts[i].setSurrounding(texture::wrap_s, texture::surr_edge);
            texts[i].setSurrounding(texture::wrap_t, texture::surr_edge);
        }
    }
    _world.setFilter(texture::ope_mag, texture::fil_linear);
    _world.setFilter(texture::ope_min, texture::fil_linear);
    _world.setSurrounding(texture::wrap_r, texture::surr_edge);
    _world.setSurrounding(texture::wrap_s, texture::surr_edge);
    _world.setSurrounding(texture::wrap_t, texture::surr_edge);
    vertexArray _arr;
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    _arr.setData(sizeof(skyboxVertices),skyboxVertices,GL_STATIC_DRAW,3*sizeof(float));
    _arr.setVertexAttributes({
            {0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),0}
        });

    _tprogram->use();

    mycube::cubeSolver::solver _solver(*_tcube,_camera);
    ::solver = &_solver;

    ::solver->initPickMode(_window);

    _tcube->frameCallback = [](Cube::animateFrame& _frame)->bool {
        if (!PlaySound(L"resources\\rotate.wav", 0, SND_ASYNC | SND_FILENAME)) {
            wstringstream ss(L"´íÎó");
            ss << GetLastError();
            MessageBox(NULL, ss.str().c_str(), L"´íÎó", MB_OK);
        }
        return true;
    };

    _tprogram->use();

    posProAndView = _tprogram->getUniformPos("proAndView");
    posViewPos = _tprogram->getUniformPos("viewPos");
    posTInvModel = _tprogram->getUniformPos("tinvModel");

    view = glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, -10.0f));
    projection = glm::perspective(glm::radians((float)fov), 1.0f, 0.1f, 100.0f);
    _window.fitViewport();

    _tprogram->uniform(_tprogram->getUniformPos("lightColor"), 1.0f, 1.0f, 1.0f);

    glfwSwapInterval(1);
    enableDepthTest();
    stringstream bgmope;
    bgmope << "open " << thisConfig.backgroundMusic;
    mciSendStringA(bgmope.str().c_str(), NULL, 0, 0);
    bgmope.str("");
    bgmope << "play ";
    bgmope << thisConfig.backgroundMusic << " repeat";
    mciSendStringA(bgmope.str().c_str(), NULL, 0, 0);

    while (_window.running()) {
        if (_window.getKey(GLFW_KEY_ESCAPE)) {
            _window.setNoRunning(true);
            break;

        }
        
        glExt::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        depthTest();

        _solver.changeWorkFace();
        _camera.checkFrame();
        _tcube->frameCheck();
        view = _camera.getLookAt();
        projection = glm::perspective(glm::radians((float)fov), (float)window_width / window_height, 0.1f, 100.0f);

        glDepthMask(GL_FALSE);
        _sprogram.use();
        _world.bind();
        textureUnit::active(0);
        glm::quat tmpquat =  glm::quat(view);
        tmpquat.x = 0.0f;
        tmpquat.z = 0.0f;
        tmpquat = glm::normalize(tmpquat);
        _sprogram.uniform(_sprogram.getUniformPos("stdmat"), projection* glm::toMat4(tmpquat));
        _arr.draw(GL_TRIANGLES);
        glDepthMask(GL_TRUE);

        _tprogram->use();
        _tprogram->uniform(posProAndView, projection * view);
        //_tprogram->uniform(posViewPos, _camera.pos());
        _tprogram->uniform(posViewPos, _camera.getPos());

        _tprogram->uniform(_tprogram->getUniformPos("lightPos"), 6.0f * (float)sin(glfwGetTime()), 3.0f, 6.0f * (float)cos(glfwGetTime()));

        if (!_pickface)
            if (useTexture) _tcube->paint(_textsmap);
            else _tcube->paint();
        else _tcube->paintEach([](program& _pro, std::vector<singleCube*>::const_iterator i, unsigned faceIndex, glExt::vertexArray* _arr) {
            int posTexture = _pro.getUniformPos("texture0");
            if (*i == _pickface->inSingleCube) {
                int posModel = _pro.getUniformPos("model"),postinvModel = _pro.getUniformPos("tinvModel");
                glm::mat4 model = ((*i)->useTimelyRotateMat ? (*i)->timelyRotateMat : (*i)->rotateMat) * (*i)->initTranslate * glm::scale(glm::mat4(1.0f), glm::vec3(1.05f));
                _pro.uniform(posModel, model);
                _pro.uniform(postinvModel, glm::transpose(glm::inverse(model)));
            }
            if (useTexture) {
                if ((*i)->faces[faceIndex].outShow) {
                    _pro.uniform(_pro.getUniformPos("useTexture"), 1);
                    _pro.uniform(posTexture, 0);
                    glExt::textureUnit::active(0);
                    texts[faceIndex].bind();
                }
                else {
                    _pro.uniform(_pro.getUniformPos("useTexture"), 0);
                }
            }
            else _pro.uniform(_pro.getUniformPos("useTexture"), 0);;
            _arr[faceIndex].draw(GL_TRIANGLES);
            },(bool)texts);

        _window.handleSysEvent();
    }
    delete _tcube;
    if (texts) {
        delete[] texts;
    }
    return 0;
};


