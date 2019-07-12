#include <cmath>
#include <QDebug>
#include "include/ControlWidget.h"
#include "ui_ControlWidget.h"




// for Ui file constructor,
//    ui(new Ui::ControlWidget)

DoubleSpinBox::DoubleSpinBox(uint _row, uint _column, QWidget *parent)  :
    row(_row),
    column(_column),
    QDoubleSpinBox(parent)

{
    this->setRange(-1000,1000);
//    connect(this, SIGNAL(valueChanged(double)), this, SLOT(emitValueChangedRowColum(double)));
}


void DoubleSpinBox::emitValueChangedRowColum(double _v)
{
//    emit valueChangedRowColum(_v, row, column);
}


ValueSliderF::ValueSliderF(float defaultV, QWidget *parent, float _min, float _max, int decimalPlace) :
    m_conversion(pow(10,decimalPlace)),
    mParent(parent)
{
    m_min = _min * m_conversion;
    m_max = _max * m_conversion;
    m_default = defaultV * m_conversion;

    if(m_default < m_min) m_min = m_default;
    if(m_default > m_max) m_max = m_default;

    setupUi();
}

void ValueSliderF::setupUi()
{
//    layout = new QHBoxLayout();
//    layout = new QVBoxLayout();
    layout = new QGridLayout();
    m_slider = new QSlider(Qt::Horizontal);
    m_label = new QLabel();
    m_label->setNum(m_default / m_conversion);

    m_slider->setMaximum(m_max);
    m_slider->setMinimum(m_min);
    m_slider->setSliderPosition(m_default);

    auto s = m_slider->size();

//    layout->setHorizontalSpacing(200);

    QSize size(80,20);
    m_slider->setFixedSize(size);

    layout->addWidget(m_slider,0,0);
    layout->addWidget(m_label,0,1);

//    layout->setGeometry(QRect(QPoint(0,0),QPoint(200,100)));
    layout->setSpacing(0);

//    this->setMaximumHeight(30);
    this->setLayout(layout);

    // connect internal Signals
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(emitValueChanged(int)));

}

void ValueSliderF::emitValueChanged(int _v)
{
    float v  = float(_v) / m_conversion;
    m_label->setNum(v);
    emit valueChanged(v);
}

TransformUiWidget::TransformUiWidget(QWidget *parent) :
    QWidget(parent)
{
    // QDesigner
//    ui->setupUi(this);

    setupUi();
//    this->setMaximumWidth(200);

    connectWidgets();

    this->setMinimumHeight(240);
}

TransformUiWidget::~TransformUiWidget()
{
}

void TransformUiWidget::setupUi()
{
    createQObjects();

    transGridLayout->addWidget(translateLabel, 0,0);
    transGridLayout->addWidget(rotateLabel, 1,0);
    transGridLayout->addWidget(scaleLabel, 2,0);

    transGridLayout->addWidget(transform00Edit, 0,1);
    transGridLayout->addWidget(transform10Edit, 1,1);
    transGridLayout->addWidget(transform20Edit, 2,1);
    transGridLayout->addWidget(transform01Edit, 0,2);
    transGridLayout->addWidget(transform11Edit, 1,2);
    transGridLayout->addWidget(transform21Edit, 2,2);
    transGridLayout->addWidget(transform02Edit, 0,3);
    transGridLayout->addWidget(transform12Edit, 1,3);
    transGridLayout->addWidget(transform22Edit, 2,3);

    transGridLayout->setVerticalSpacing(1);
    transGridLayout->setHorizontalSpacing(1);

    matrixGridLayout->addWidget(mat00Edit,0,0);
    matrixGridLayout->addWidget(mat10Edit,1,0);
    matrixGridLayout->addWidget(mat20Edit,2,0);
    matrixGridLayout->addWidget(mat30Edit,3,0);
    matrixGridLayout->addWidget(mat01Edit,0,1);
    matrixGridLayout->addWidget(mat11Edit,1,1);
    matrixGridLayout->addWidget(mat21Edit,2,1);
    matrixGridLayout->addWidget(mat31Edit,3,1);
    matrixGridLayout->addWidget(mat02Edit,0,2);
    matrixGridLayout->addWidget(mat12Edit,1,2);
    matrixGridLayout->addWidget(mat22Edit,2,2);
    matrixGridLayout->addWidget(mat32Edit,3,2);
    matrixGridLayout->addWidget(mat03Edit,0,3);
    matrixGridLayout->addWidget(mat13Edit,1,3);
    matrixGridLayout->addWidget(mat23Edit,2,3);
    matrixGridLayout->addWidget(mat33Edit,3,3);

    layoutWidget->setGeometry(QRect(10, 10, 240, 661));
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addLayout(transGridLayout);
    layout->addWidget(matrixLabel);
    layout->addLayout(matrixGridLayout);
    layout->addSpacerItem(layoutSpacer);
}

void TransformUiWidget::connectWidgets()
{
    connect(transform00Edit, SIGNAL(valueChanged(double)), this, SLOT(fooB()));

    connect(transform00Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(transform01Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(transform02Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(transform10Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(transform11Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(transform12Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(transform20Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(transform21Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(transform22Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));

    connect(mat00Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat01Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat02Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat03Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat10Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat11Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat12Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat13Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat20Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat21Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat22Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat23Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat30Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat31Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat32Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
    connect(mat33Edit, SIGNAL(valueChanged(double)), this, SLOT(emitTransformChange()));
}

void TransformUiWidget::setTransform(const QMatrix4x4 _mat4, const QVector3D _t, const QVector3D _r, const QVector3D _s, int var)
{
    // loop over Units block signal set value allow signals
    transform00Edit->blockSignals(true);
    transform00Edit->setValue(_t.x());
    transform00Edit->blockSignals(false);

    transform01Edit->blockSignals(true);
    transform01Edit->setValue(_t.y());
    transform01Edit->blockSignals(false);

    transform02Edit->blockSignals(true);
    transform02Edit->setValue(_t.z());
    transform02Edit->blockSignals(false);

    transform10Edit->setValue(var);
}

void TransformUiWidget::emitTransformChange()
{
    // prepare data to send back to scene
    QVector3D translation, rotation, scale;
    QMatrix4x4 trs;

    translation = QVector3D(transform00Edit->value(), transform01Edit->value(), transform02Edit->value());
    rotation = QVector3D(transform10Edit->value(), transform11Edit->value(), transform12Edit->value());
    scale = QVector3D(transform20Edit->value(), transform21Edit->value(), transform22Edit->value());

    trs =  QMatrix4x4(
                mat00Edit->value(), mat01Edit->value(), mat02Edit->value(), mat03Edit->value(),
                mat10Edit->value(), mat11Edit->value(), mat12Edit->value(), mat13Edit->value(),
                mat20Edit->value(), mat21Edit->value(), mat22Edit->value(), mat23Edit->value(),
                mat30Edit->value(), mat31Edit->value(), mat32Edit->value(), mat33Edit->value()
                );

    emit matrixChanged(translation, rotation, scale);
}

void TransformUiWidget::createQObjects()
{
    layoutWidget = new QWidget(this);
    transGridLayoutWidget = new QWidget(this);

    layout = new QVBoxLayout(layoutWidget);
    transGridLayout = new QGridLayout;
    matrixGridLayout = new QGridLayout;

    ObjectIDLabel = new QLabel;
    translateLabel = new QLabel("translate");
    rotateLabel = new QLabel("rotate");
    scaleLabel = new QLabel("scale");
    matrixLabel = new QLabel("tMatrix");

    layoutSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

     transform00Edit = new DoubleSpinBox(0,0);
     transform01Edit = new DoubleSpinBox(0,0);
     transform02Edit = new DoubleSpinBox(0,0);
     transform10Edit = new DoubleSpinBox(0,0);
     transform11Edit = new DoubleSpinBox(0,0);
     transform12Edit = new DoubleSpinBox(0,0);
     transform20Edit = new DoubleSpinBox(0,0);
     transform21Edit = new DoubleSpinBox(0,0);
     transform22Edit = new DoubleSpinBox(0,0);

     mat00Edit = new DoubleSpinBox(0,0);
     mat01Edit = new DoubleSpinBox(0,0);
     mat02Edit = new DoubleSpinBox(0,0);
     mat03Edit = new DoubleSpinBox(0,0);
     mat10Edit = new DoubleSpinBox(0,0);
     mat11Edit = new DoubleSpinBox(0,0);
     mat12Edit = new DoubleSpinBox(0,0);
     mat13Edit = new DoubleSpinBox(0,0);
     mat20Edit = new DoubleSpinBox(0,0);
     mat21Edit = new DoubleSpinBox(0,0);
     mat22Edit = new DoubleSpinBox(0,0);
     mat23Edit = new DoubleSpinBox(0,0);
     mat30Edit = new DoubleSpinBox(0,0);
     mat31Edit = new DoubleSpinBox(0,0);
     mat32Edit = new DoubleSpinBox(0,0);
     mat33Edit = new DoubleSpinBox(0,0);
}



ControlWidget::ControlWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ControlWidget)
{
    setupUi();
}

void ControlWidget::setupUi()
{
    this->setMaximumWidth(270);
    layout = new QVBoxLayout;
    transformWidget = new TransformUiWidget(this);
    dynamicsWidget =  new DynamicsUiWidget;

    layout->addWidget(transformWidget);
    layout->addWidget(dynamicsWidget);
    layout->addStretch(1);

    this->setLayout(layout);
}

DynamicsUiWidget::DynamicsUiWidget(QWidget *parent)
{
    setupUi();
}

void DynamicsUiWidget::setupUi()
{
    startSim = new QPushButton("stop");
    stepSim = new QPushButton("step");
    resetSim = new QPushButton("reset");

    stepSizeLabel = new QLabel("step size");
    stepSizeEdit = new ValueSliderF(timeStepSize,this, 0.001, 1, 3);

    gravityLabel = new QLabel("gravity (f) ");
//    gravityLabelEditX = new ValueSliderF(0, this, -10 , 10);
    gravityLabelEditY = new ValueSliderF(gravityDir.y(), this, -10 , 10);
//    gravityLabelEditZ = new ValueSliderF(0, this, -10 , 10);

    particleMassLabel = new QLabel("particle m ");
    particleMassEdit = new ValueSliderF(particleMass, this, 0.01, 5);

    preConditionsIterLabel = new QLabel("pre iter:");
    preConditionsIterEdit = new ValueSliderI(preConditionIterations, this, 0, 20);
    constraintIterLabel = new QLabel("constraint iter");
    constraintIterEdit = new ValueSliderI(constraintIterations, this, 0, 50);

    pbdDampingLabel = new QLabel("PBD Damping");
    pbdDampingEdit = new ValueSliderF(pbd_Damping, this, 0, 1);

    constraintHeadline = new QLabel("Constraints:");

//    distanceConstraintStretchLabel = new QLabel("Stretch:");
//    distanceConstraintStretchEdit = new ValueSliderF(distanceConstraintStrechR, this, 0, 1);

//    distanceConstraintCompressLabel = new QLabel("Compress:");
//    distanceConstraintCompressEdit = new ValueSliderF(distanceConstraintCompressR, this, 0, 1);

//    shapeMatchingConstraintAttractLabel = new QLabel("SM Attract:");
//    shapeMatchingConstraintAttractEdit = new ValueSliderF(shapeMatchingGoalAttract, this, 0, 1);


    layout.setVerticalSpacing(0);
    layout.setHorizontalSpacing(10);
//    spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout.addWidget(startSim,0,0);
    layout.addWidget(stepSim,0,1);
    layout.addWidget(resetSim,0,2);

    layout.addWidget(stepSizeLabel,1,0);
    layout.addWidget(stepSizeEdit,1,1,1,6);

    layout.addWidget(gravityLabel,2,0);
//    layout.addWidget(gravityLabelEditX,2,1);
    layout.addWidget(gravityLabelEditY,2,1,1,6);

    layout.addWidget(particleMassLabel,3,0);
    layout.addWidget(particleMassEdit,3,1,1,3);

    layout.addWidget(constraintIterLabel,5,0);
    layout.addWidget(constraintIterEdit,5,1,1,3);

    layout.addWidget(preConditionsIterLabel,6,0);
    layout.addWidget(preConditionsIterEdit,6,1,1,3);

    layout.addWidget(pbdDampingLabel,7,0);
    layout.addWidget(pbdDampingEdit,7,1,1,3);

//    layout.addWidget(constraintHeadline,8,0);

//    layout.addWidget(distanceConstraintStretchLabel,9,0);
//    layout.addWidget(distanceConstraintStretchEdit,9,1,1,3);

//    layout.addWidget(distanceConstraintCompressLabel,10,0);
//    layout.addWidget(distanceConstraintCompressEdit,10,1,1,3);

//    layout.addWidget(shapeMatchingConstraintAttractLabel,11,0);
//    layout.addWidget(shapeMatchingConstraintAttractEdit,11,1,1,3);

      this->setLayout(&layout);
}


ValueSliderI::ValueSliderI(int defaultV, QWidget *parent, int _min, int _max):
    mParent(parent),
    m_min(_min ),
    m_max(_max ),
    m_default(defaultV)
{
    if(m_default < m_min) m_min = m_default;
    if(m_default > m_max) m_max = m_default;
    setupUi();
}

void ValueSliderI::setupUi()
{
    layout = new QHBoxLayout();
    m_slider = new QSlider(Qt::Horizontal);
    m_label = new QLabel();
    m_label->setNum(m_default);

    m_slider->setMaximum(m_max);
    m_slider->setMinimum(m_min);
    m_slider->setSliderPosition(m_default);

    QSize size(80,20);
    m_slider->setFixedSize(size);

    layout->addWidget(m_slider);
    layout->addWidget(m_label);

    layout->setSpacing(0);

    this->setLayout(layout);

    // connect internal Signals
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(emitValueChanged(int)));

}

void ValueSliderI::emitValueChanged(int _v)
{
    m_label->setNum(_v);
    emit valueChanged(_v);
}
