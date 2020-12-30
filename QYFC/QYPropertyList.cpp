
#include "QYPropertyList.h"


QYPropertyList::~QYPropertyList()
{
    clearProperty();
}

void QYPropertyList::clearProperty()
{
    m_properties.clear();
}

//============================================================================
void
QYPropertyList::addProperty(
    const std::string &key,
    property_t value
)
//============================================================================
{
    m_properties.insert( std::pair<const std::string, property_t>( key, value ) );
}


void QYPropertyList::deleteProperty(const std::string &key)
{
    auto it = m_properties.find( key );

    if ( m_properties.end() == it )
    {
        return;
    }

    m_properties.erase( it );
}

//============================================================================
void
QYPropertyList::addProperty(
    const std::string &key,
    std::string value
)
//============================================================================
{
    property_t property;
    property.type = PROP_TYPE_STRING;
    property.s = value;
    addProperty( key, property );
}

//============================================================================
void
QYPropertyList::addProperty(
    const std::string &key,
    const char* value
)
//============================================================================
{
    addProperty( key, std::string( value ) );
}

//============================================================================
void
QYPropertyList::addProperty(
    const std::string &key,
    int value
)
//============================================================================
{
    property_t property;
    property.type = PROP_TYPE_INT;
    property.i = value;
    addProperty( key, property );
}

//============================================================================
void
QYPropertyList::addProperty(
    const std::string &key,
    double value
)
//============================================================================
{
    property_t property;
    property.type = PROP_TYPE_DOUBLE;
    property.d = value;
    addProperty( key, property );
}

//============================================================================
void
QYPropertyList::addProperty(
    const std::string &key,
    bool value
)
//============================================================================
{
    property_t property;
    property.type = PROP_TYPE_BOOL;
    property.b = value;
    addProperty( key, property );
}

//============================================================================
void
QYPropertyList::addProperty(
    const std::string &key,
    char value
)
//============================================================================
{
    property_t property;
    property.type = PROP_TYPE_CHAR;
    property.c = value;
    addProperty( key, property );
}

//============================================================================
void
QYPropertyList::addProperty(
    const std::string &key,
    void* value
)
//============================================================================
{
    property_t property;
    property.type = PROP_TYPE_POINTER;
    property.p = value;
    addProperty( key, property );
}

//============================================================================
bool
QYPropertyList::getExists(
    const std::string &key
)
//============================================================================
{
    return ( m_properties.find( key ) != m_properties.end() );
}

//============================================================================
#if 0


const std::string &
QYPropertyList::getValueString(
const char *key
)
//============================================================================
{
    if ( !getExists( key ) )
    {
        return "";
    }

    if ( m_properties.at( key ).type != PROP_TYPE_STRING )
    {
        return "";
    }

    return m_properties.at( key ).s;
}
#endif

const char* QYPropertyList::getValueString(const char *key)
{
	if (!getExists(key))
	{
		return nullptr;
	}

	if (m_properties.at(key).type != PROP_TYPE_STRING)
	{
		return nullptr;
	}

	return m_properties.at(key).s.c_str();
}

//============================================================================
const std::string &
QYPropertyList::getValueWithDefaultString(
    const std::string &key,
    const std::string &default_value
)
//============================================================================
{
	if (!getExists(key))
	{
		return default_value;
	}

	if (m_properties.at(key).type != PROP_TYPE_STRING)
	{
		return default_value;
	}

	return m_properties.at(key).s;
}

//============================================================================
const int
QYPropertyList::getValueInt(
    const std::string &key
)
//============================================================================
{
    if ( !getExists( key ) )
    {
        return 0;
    }

    if ( m_properties.at( key ).type != PROP_TYPE_INT )
    {
        return 0;
    }

    return m_properties.at( key ).i;
}

//============================================================================
const double
QYPropertyList::getValueDouble(
    const std::string &key
)
//============================================================================
{
    if ( !getExists( key ) )
    {
        return 0;
    }

    if ( m_properties.at( key ).type != PROP_TYPE_DOUBLE )
    {
        return 0;
    }

    return m_properties.at( key ).d;
}

//============================================================================
const bool
QYPropertyList::getValueBool(
    const std::string &key
)
//============================================================================
{
    if ( !getExists( key ) )
    {
        return false;
    }

    if ( m_properties.at( key ).type != PROP_TYPE_BOOL )
    {
        return false;
    }

    return m_properties.at( key ).b;
}

//============================================================================
const char
QYPropertyList::getValueChar(
    const std::string &key
)
//============================================================================
{
    if ( !getExists( key ) )
    {
        return '\0';
    }

    if ( m_properties.at( key ).type != PROP_TYPE_CHAR )
    {
        return '\0';
    }

    return m_properties.at( key ).c;
}

//============================================================================
const void*
QYPropertyList::getValuePtr(
    const std::string &key
)
//============================================================================
{
    if ( !getExists( key ) )
    {
        return NULL;
    }

    if ( m_properties.at( key ).type != PROP_TYPE_POINTER )
    {
        return NULL;
    }

    return m_properties.at( key ).p;
}

//============================================================================
const int
QYPropertyList::getValueWithDefaultInt(
    const std::string &key,
    int default_value
)
//============================================================================
{
    if ( !getExists( key ) )
    {
        return default_value;
    }

    if ( m_properties.at( key ).type != PROP_TYPE_INT )
    {
        return default_value;
    }

    return m_properties.at( key ).i;
}

//============================================================================
const double
QYPropertyList::getValueWithDefaultDouble(
    const std::string &key,
    double default_value
)
//============================================================================
{
    if ( !getExists( key ) )
    {
        return default_value;
    }

    if ( m_properties.at( key ).type != PROP_TYPE_DOUBLE )
    {
        return default_value;
    }

    return m_properties.at( key ).d;
}

//============================================================================
const bool
QYPropertyList::getValueWithDefaultBool(
    const std::string &key,
    bool default_value
)
//============================================================================
{
    if ( !getExists( key ) )
    {
        return default_value;
    }

    if ( m_properties.at( key ).type != PROP_TYPE_BOOL )
    {
        return default_value;
    }

    return m_properties.at( key ).b;
}

//============================================================================
const char
QYPropertyList::getValueWithDefaultChar(
    const std::string &key,
    char default_value
)
//============================================================================
{
    if ( !getExists( key ) )
    {
        return default_value;
    }

    if ( m_properties.at( key ).type != PROP_TYPE_CHAR )
    {
        return default_value;
    }

    return m_properties.at( key ).c;
}

const void*
QYPropertyList::getValueWithDefaultPtr(
    const std::string &key,
    void* default_value
)
{
    if ( !getExists( key ) )
    {
        return default_value;
    }

    if ( m_properties.at( key ).type != PROP_TYPE_POINTER )
    {
        return default_value;
    }

    return m_properties.at( key ).p;
}

