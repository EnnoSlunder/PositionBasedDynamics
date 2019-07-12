#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <Qt>
#include <QPoint>

class inputManager
{
public:
    enum InputState
    {
      InputInvalid,
      InputRegistered,
      InputUnregistered,
      InputTriggered,
      InputPressed,
      InputReleased
    };

    // the InputState of the Qt::Key if it's in the container
    static InputState keyState(Qt::Key key);
    static bool keyTriggered(Qt::Key key);
    static bool keyPressed(Qt::Key key);
    static bool keyReleased(Qt::Key key);

    static InputState buttonState(Qt::MouseButton button);
    static bool buttonTriggered(Qt::MouseButton button);
    static bool buttonPressed(Qt::MouseButton button);
    static bool buttonReleased(Qt::MouseButton button);

//    to Do: add wheelEvents
//    static bool wheelTriggered();


    static QPoint mousePosition();
    static QPoint mouseTriggeredPosition();
    static QPoint mouseDelta();

    void setMouseTriggeredPosition();

private:
    static void update(const QPoint &_localMousePos);
    static void registerKeyPress(int key);
    static void registerKeyRelease(int key);
    static void registerMousePress(Qt::MouseButton button);
    static void registerMouseRelease(Qt::MouseButton button);
    static void reset();
    // friend Window so Window can call private members from inputMange
    // yields a friendship for scene and window !!
    friend class Window;
    friend class GLWidget;


};


inline bool inputManager::keyTriggered(Qt::Key key)
{
  return keyState(key) == InputTriggered;
}

inline bool inputManager::keyPressed(Qt::Key key)
{
  return keyState(key) == InputPressed;
}

inline bool inputManager::keyReleased(Qt::Key key)
{
  return keyState(key) == InputReleased;
}

inline bool inputManager::buttonTriggered(Qt::MouseButton button)
{
  return buttonState(button) == InputTriggered;
}

inline bool inputManager::buttonPressed(Qt::MouseButton button)
{
  return buttonState(button) == InputPressed;
}

inline bool inputManager::buttonReleased(Qt::MouseButton button)
{
  return buttonState(button) == InputReleased;
}

#endif // INPUTMANAGER_H
