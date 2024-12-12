#include "../../header/database/valve.hpp"

int Valve::_nextId = 0;

Valve::Valve()
{
    this->_state = false;
    this->_value = 10;
    this->_id = _nextId++;
    qDebug() << "defautl  valve constructor" << _id;
}

int Valve::getValue()
{
    return (this->_value);
}

PneuValve::PneuValve(QString name)
{
    this->_name = name;
}

