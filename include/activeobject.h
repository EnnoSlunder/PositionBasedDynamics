#ifndef ACTIVEOBJECT_H
#define ACTIVEOBJECT_H

#include <QObject>
#include <QMatrix4x4>
#include <QVector3D>

#include "utils.h"
#include "manipulator.h"
#include "sceneobject.h"
#include "dynamics/constraint.h"

class GLWidget;
class Manipulator;

class ActiveObject : public QObject
{
    Q_OBJECT

public:
    enum AOInput{
        LM_CLICKED,
        LM_PRESSED,
        LM_RELEASED,
        B_P_PRESSED,
        B_T_PRESSED,
        B_D_PRESSED
    };
    enum AOState{
        NONE_SELECTED,
        SELECTED ,
        PINNED,
        PICKED,
    };

    ActiveObject();
    ActiveObject(GLWidget *_widget);
    void notify(int _id);
    void notify(pSceneOb _sender);
    void notify(const Transform &_t);
    void notify(MouseState _mouseState);

    pSceneOb currentObject();
    bool isActive();

    void select();
    void deselect();

    void setTransform(const QVector3D _t, const QVector3D _r, const QVector3D _s);
    void setManipulator(Manipulator *_manipulator);
    void setState(AOState _state);

    void pinConstraintActive();
    void updatePinConstraintActive();
    void unpinConstraintActive();
    void processInput(AOInput _input);
    void addParticleToSelection(const ParticlePtr _p);
    void addPinTogetherConstraintToSelection();
    void deletePinTogetherConstraintFromSelection();

signals:
    void transformChanged(const QMatrix4x4 _modelMat, const QVector3D num, const QVector3D num1, const QVector3D num2, int var=0);
    void testSignal(int p);

private:
    bool m_isActive;
    bool m_pickedDynamic = false;

    AOInput m_userInput;
    AOState m_state;

    pSceneOb activeSceneObject = nullptr;
    GLWidget *m_GLWidget;
    Manipulator *m_manipulator;
    std::shared_ptr<PinConstraint>  m_pinConstraint;
    std::vector<ParticlePtr>        m_selection;
};

#endif // ACTIVEOBJECT_H
