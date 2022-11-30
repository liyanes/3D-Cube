#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Windows.h>
#include "glext.h"
#include "cube.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#pragma comment(lib,"winmm.lib")

using namespace glExt;
using namespace std;
using namespace mycube;
using namespace mycube::cubeSolver;

float pitch = 0.0f, yaw = -90.0f;

double lastClickX, lastClickY;
double lastClickTime = 0.0;
double lastX, lastY ,fov = 45.0f;
bool leftkeyPress = false;
float distance = 10.0f;
const double sensitivity = 0.0005f;
const float spinRate = 4.0f;
int window_width = 1920, window_height = 1920;

int posView,posProj,posViewPos;
glm::mat4 view,projection;

float rotate_rate = 3.5f;
constexpr auto getRateTime = [](float rotate_rate) ->float {
    return glm::radians(90.0f) / rotate_rate;
};

singleCubeFace* _pickface = nullptr;
bool lastFramePicked = false;
// 指定是否允许三阶魔方的特殊按键
bool enableLevel3KeyMap = false;

//fpsCamera _camera(glm::vec3(0.0f,0.0f,10.0f));
QuatCamera _camera(glm::vec3(0.0f, 0.0f, 10.0f));

mycube::Cube* _tcube;
mycube::cubeSolver::solver* solver;
program* _tprogram;
window* _twindow;

void mouseCallback(window& _window, int button, int action, int mode) {
    auto curpos = _window.getCursorPos();
    if (button == 0 && action == 1) {
        lastClickTime = glfwGetTime();
    }
    if (button == 0 && action == 0 && glfwGetTime() - lastClickTime < 0.1) {
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
        //delete _tcube;
        //_tcube = new mycube::Cube(key - GLFW_KEY_0, 2.0f, *_tprogram);
        new (_tcube)mycube::Cube(key - GLFW_KEY_0, 2.0f, *_tprogram); 
        _tcube->frameCallback = [](Cube::animateFrame& _frame)->bool {
            if (!PlaySound(L"resources\\rotate.wav", 0, SND_ASYNC | SND_FILENAME)) {
                wstringstream ss(L"错误");
                ss << GetLastError();
                MessageBox(NULL, ss.str().c_str(), L"错误", MB_OK);
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
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        _twindow->setFullScreen(!_twindow->isFullScreen());
    }
    using namespace mycube::cubeSolver;
    
    if (!(enableLevel3KeyMap && _tcube->getLevel() == 3)) {
        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getRight(),glm::radians(-90.f) }, getRateTime(rotate_rate));
        }
        else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getRight(),glm::radians(90.f) }, getRateTime(rotate_rate));
        }
        else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getUp(),glm::radians(-90.f) }, getRateTime(rotate_rate));
        }
        else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getUp(),glm::radians(90.f) }, getRateTime(rotate_rate));
        }
        else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getFront(),glm::radians(-90.f) }, getRateTime(rotate_rate));
        }
        else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
            _camera.requireRotate({ _camera.getFront(),glm::radians(90.f) }, getRateTime(rotate_rate));
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
                    ::solver->rotateToFace(faceRotate::down, getRateTime(rotate_rate));
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::left, 2, rotate_rate);
                    ::solver->rotateToFace(faceRotate::up, getRateTime(rotate_rate));
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
                    ::solver->rotateToFace(faceRotate::up, getRateTime(rotate_rate));
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::right, 2, rotate_rate);
                    ::solver->rotateToFace(faceRotate::down, getRateTime(rotate_rate));
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
                    ::solver->rotateToFace(faceRotate::right, getRateTime(rotate_rate));
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::down, 2, rotate_rate);
                    ::solver->rotateToFace(faceRotate::left, getRateTime(rotate_rate));
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
                    ::solver->rotateToFace(faceRotate::left, getRateTime(rotate_rate));
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::up, 2, rotate_rate);
                    ::solver->rotateToFace(faceRotate::right, getRateTime(rotate_rate));
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
                    _camera.requireRotate({camera::getMajorFaceVec(_camera.getFront()),glm::radians(-90.f)}, getRateTime(rotate_rate));
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::front, 2, rotate_rate);
                    _camera.requireRotate({ camera::getMajorFaceVec(_camera.getFront()),glm::radians(90.f) }, getRateTime(rotate_rate));
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
                    _camera.requireRotate({ camera::getMajorFaceVec(_camera.getFront()),glm::radians(90.f) }, getRateTime(rotate_rate));
                }
                else {
                    ::solver->frameRequire(solver::relativeAxis::back, 2, rotate_rate);
                    _camera.requireRotate({ camera::getMajorFaceVec(_camera.getFront()),glm::radians(-90.f) }, getRateTime(rotate_rate));
                }
            }
            break;
        case GLFW_KEY_M:
            if (!shifted) ::solver->frameRequire(solver::relativeAxis::right, 1, rotate_rate);
            else ::solver->frameRequire(solver::relativeAxis::left, 1, rotate_rate);
            break;
        case GLFW_KEY_X:
            if (!shifted) ::solver->rotateToFace(faceRotate::down, getRateTime(rotate_rate));
            else ::solver->rotateToFace(faceRotate::up, getRateTime(rotate_rate));
            break;
        case GLFW_KEY_Y:
            if (!shifted) ::solver->rotateToFace(faceRotate::right, getRateTime(rotate_rate));
            else ::solver->rotateToFace(faceRotate::left, getRateTime(rotate_rate));
            break;
        case GLFW_KEY_Z:
            if (!shifted) _camera.requireRotate({ camera::getMajorFaceVec(_camera.getFront()),glm::radians(-90.f) }, getRateTime(rotate_rate));
            else _camera.requireRotate({ camera::getMajorFaceVec(_camera.getFront()),glm::radians(90.f) }, getRateTime(rotate_rate));
            break;
        }
    }

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
    SetUnhandledExceptionFilter([](EXCEPTION_POINTERS* pExptInfo)->long {
        wstringstream ss(L"发生未捕获的错误:");
        ss << "错误号:" << pExptInfo->ExceptionRecord->ExceptionCode << "\n";
        ss << "错误位置:" << pExptInfo->ExceptionRecord->ExceptionAddress << "\n";
        ss << "错误信息" << pExptInfo->ExceptionRecord->ExceptionInformation << "\n";
        MessageBox(NULL,ss.str().c_str(), L"错误", MB_OK | MB_ICONERROR);
        return EXCEPTION_EXECUTE_HANDLER;
        });
    srand((unsigned)time(NULL));

    initializer init;

	window _window = window(window_width, window_height, "Test"/*,true,glfwGetMonitors(&count)[0]*/);
    _window.setCursorCallback(cursorCallback);
    _window.setMouseCallback(mouseCallback);
    _window.setScrollCallback(scrollCallback);
    _window.setKeyCallback(keyCallback);
    _window.setFramebufferSizeCallback(frameBufferSizeCallback);

    _twindow = &_window;

    program _program;

    _tcube = new mycube::Cube(3, 2.0f,_program);
    _tprogram = &_program;

    std::array<texture_2D,6> _texts;
    std::array<texture*, 6> _textindexs;
    for (unsigned i = 0; i < 6; i++) {
        _texts[i].setSurrounding(texture::wrap_s, texture::surr_border);
        _texts[i].setSurrounding(texture::wrap_t, texture::surr_border);
        _texts[i].setFilter(texture::ope_min, texture::fil_linear);
        _texts[i].setFilter(texture::ope_mag, texture::fil_linear);
        stringstream ss;
        ss << "resources\\" << i + 6 << ".jpg\0";
        _texts[i].loadFromImage(image(ss.str().c_str(), true),GL_RGB,GL_RGB,0,GL_UNSIGNED_BYTE,true);
        _textindexs[i] = &_texts[i];
    }

    _tprogram->use();

    mycube::cubeSolver::solver _solver(*_tcube,_camera);
    ::solver = &_solver;

    ::solver->initPickMode(_window);

    _tcube->frameCallback = [](Cube::animateFrame& _frame)->bool {
        if (!PlaySound(L"resources\\rotate.wav", 0, SND_ASYNC | SND_FILENAME)) {
            wstringstream ss(L"错误");
            ss << GetLastError();
            MessageBox(NULL, ss.str().c_str(), L"错误", MB_OK);
        }
        return true;
    };

    _tprogram->use();
    //posModel = _tprogram->getUniformPos("model");
    posView = _tprogram->getUniformPos("view");
    posProj = _tprogram->getUniformPos("projection");
    posViewPos = _tprogram->getUniformPos("viewPos");

    //model = glm::rotate(glm::mat4(1), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, -10.0f));
    projection = glm::perspective(glm::radians((float)fov), 1.0f, 0.1f, 100.0f);
    _window.fitViewport();

    //_tprogram->uniform(posModel, model);
    _tprogram->uniform(posView, view);
    _tprogram->uniform(posProj, projection);
    _tprogram->uniform(_tprogram->getUniformPos("lightColor"), 1.0f, 1.0f, 1.0f);

    glfwSwapInterval(1);
    enableDepthTest();

    while (_window.running()) {
        if (_window.getKey(GLFW_KEY_ESCAPE)) {
            _window.setNoRunning(true);
            break;

        }
        
        glExt::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
        depthTest();

        _tprogram->use();
        _solver.changeWorkFace();
        _camera.checkFrame();
        _tcube->frameCheck();
        //view = _camera.getView();
        view = _camera.getLookAt();
        projection = glm::perspective(glm::radians((float)fov), (float)window_width / window_height, 0.1f, 100.0f);
        _tprogram->uniform(posView, view);
        _tprogram->uniform(posProj, projection);
        //_tprogram->uniform(posViewPos, _camera.pos());
        _tprogram->uniform(posViewPos, _camera.getPos());

        _tprogram->uniform(_tprogram->getUniformPos("lightPos"), 6.0f * (float)sin(glfwGetTime()), 3.0f, 6.0f * (float)cos(glfwGetTime()));

        if (!_pickface)
            _tcube->paint(/*, _textindexs*/);
        else _tcube->paintEach([](program& _pro, std::vector<singleCube*>::const_iterator i, unsigned faceIndex, glExt::vertexArray* _arr) {
            if (*i == _pickface->inSingleCube) {
                int posModel = _pro.getUniformPos("model");
                _pro.uniform(posModel, ((*i)->useTimelyRotateMat ? (*i)->timelyRotateMat : (*i)->rotateMat) * (*i)->initTranslate * glm::scale(glm::mat4(1.0f),glm::vec3(1.05f)));
                _arr[faceIndex].draw(GL_TRIANGLES);
            }
            else {
                _arr[faceIndex].draw(GL_TRIANGLES);
            }
            });

        _window.handleSysEvent();
    }
};


