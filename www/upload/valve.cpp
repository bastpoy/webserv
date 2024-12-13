#include "../../header/database/valve.hpp"

Valve::Valve(QString name, QString type, int value, bool state, QPixmap image, QString valveType, ComponentBook &components): Component(name, type, value, state, image)
{
    _valveType = valveType;
    components.addComponentBook1(_id, this);
    qDebug() << "defautl  valve constructor" << _id;
}

//getter
QString const &Valve::getValveType() const
{
    return (this->_valveType);
}

//setter
void Valve::setValveType(QString valveType)
{
    _valveType = valveType;
}


--
