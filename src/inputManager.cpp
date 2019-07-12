#include "inputManager.h"


#include <vector>
#include <algorithm>
#include <QDebug>
#include <QCursor>


template <typename T>
struct InputInstance : std::pair<T, inputManager::InputState>
{
  typedef std::pair<T, inputManager::InputState> base_class;
  inline InputInstance(T value) : base_class(value, inputManager::InputInvalid) {}
  inline InputInstance(T value, inputManager::InputState state) : base_class(value, state) {}
  inline bool operator==(const InputInstance &rhs) const
  {
    return this->first == rhs.first;
  }
};

// Instance types
typedef InputInstance<Qt::Key> KeyInstance;
typedef InputInstance<Qt::MouseButton> ButtonInstance;

// Container types
typedef std::vector<KeyInstance> KeyContainer;
typedef std::vector<ButtonInstance> ButtonContainer;

// Globals
static KeyContainer sg_keyInstances;
static ButtonContainer sg_buttonInstances;

static QPoint sg_mouseCurrPosition;
static QPoint sg_mousePrevPosition;
static QPoint sg_mouseTriggeredPosition;
static QPoint sg_mouseDelta;
static QPoint sg_mouseTriggeredDelta;


static inline KeyContainer::iterator FindKey(Qt::Key value)
{
  return std::find(sg_keyInstances.begin(), sg_keyInstances.end(), value);
}

static inline ButtonContainer::iterator FindButton(Qt::MouseButton value)
{
  return std::find(sg_buttonInstances.begin(), sg_buttonInstances.end(), value);
}

template <typename TPair>
static inline void UpdateStates(TPair &instance)
{
  switch (instance.second)
  {
  case inputManager::InputRegistered:
    instance.second = inputManager::InputTriggered;
    break;
  case inputManager::InputTriggered:
    instance.second = inputManager::InputPressed;
    break;
  case inputManager::InputUnregistered:
    instance.second = inputManager::InputReleased;
    break;
  default:
    break;
  }
}

//
template <typename TPair>
static inline bool CheckReleased(const TPair &instance)
{
  return instance.second == inputManager::InputReleased;
}

template <typename Container>
static inline void Update(Container &container)
{

  // container is actual a
  // std::vector < InputInstance<Qt::Key> >
  // std::vector< std::pair< Qt::Key, Input::InputState>>

  typedef typename Container::iterator Iter;
  typedef typename Container::value_type TPair;

  // Remove old data
  // remove if the key is not pressed anymore
  Iter remove = std::remove_if(container.begin(), container.end(), &CheckReleased<TPair>);
  container.erase(remove, container.end());

  // Update existing data
  std::for_each(container.begin(), container.end(), &UpdateStates<TPair>);
}

inputManager::InputState inputManager::keyState(Qt::Key key)
{
    KeyContainer::iterator it = FindKey(key);
    return (it != sg_keyInstances.end()) ? it->second : InputInvalid;
}

inputManager::InputState inputManager::buttonState(Qt::MouseButton button)
{
    ButtonContainer::iterator it = FindButton(button);
    return (it != sg_buttonInstances.end()) ? it->second : InputInvalid;
}

QPoint inputManager::mousePosition()
{
    return sg_mouseCurrPosition;
}

QPoint inputManager::mouseTriggeredPosition()
{
    return sg_mouseTriggeredPosition;
}

QPoint inputManager::mouseDelta()
{
    return sg_mouseDelta;
}

void inputManager::setMouseTriggeredPosition()
{
    sg_mouseTriggeredPosition = sg_mouseCurrPosition;
}

void inputManager::update(const QPoint &_localMousePos)
{
    // Update Mouse Delta
    sg_mousePrevPosition = sg_mouseCurrPosition;
    sg_mouseCurrPosition = _localMousePos;

    sg_mouseDelta = sg_mouseCurrPosition - sg_mousePrevPosition;
    sg_mouseTriggeredDelta = sg_mouseCurrPosition - sg_mouseTriggeredPosition;

    Update(sg_buttonInstances);
    Update(sg_keyInstances);
}

void inputManager::registerKeyPress(int key)
{
    // get interator
  KeyContainer::iterator it = FindKey((Qt::Key)key);
  // if Qt:key doenst exist yet add it to the container
  if (it == sg_keyInstances.end())
  {
    sg_keyInstances.push_back(KeyInstance((Qt::Key)key, InputRegistered));
  }
}

void inputManager::registerKeyRelease(int key)
{
    KeyContainer::iterator it = FindKey((Qt::Key)key);
    if (it != sg_keyInstances.end())
    {
      it->second = InputUnregistered;
    }
}

void inputManager::registerMousePress(Qt::MouseButton button)
{
    ButtonContainer::iterator it = FindButton(button);
    if (it == sg_buttonInstances.end())
    {
      sg_buttonInstances.push_back(ButtonInstance(button, InputRegistered));
    }
}

void inputManager::registerMouseRelease(Qt::MouseButton button)
{
    ButtonContainer::iterator it = FindButton(button);
    if (it != sg_buttonInstances.end())
    {
      it->second = InputUnregistered;
    }
}

void inputManager::reset()
{
    sg_keyInstances.clear();
    sg_buttonInstances.clear();
}
