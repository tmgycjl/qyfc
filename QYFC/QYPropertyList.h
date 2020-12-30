
#pragma  once


#include "qydefine.h"
#include <map>
#include <string>

enum property_type_t
{
    PROP_TYPE_NONE,
    PROP_TYPE_STRING,
    PROP_TYPE_INT,
    PROP_TYPE_DOUBLE,
    PROP_TYPE_BOOL,
    PROP_TYPE_CHAR,
    PROP_TYPE_POINTER
};

struct property_t
{
	property_type_t type;
    std::string s;
    int i;
    double d;
    bool b;
    char c;
    void* p;
};


class QYUI_EXPORT_CLASS QYPropertyList
{
    public:
		~QYPropertyList();
        void addProperty( const std::string &key, property_t );
        void addProperty( const std::string &key, std::string value );
        void addProperty( const std::string &key, const char* value );
        void addProperty( const std::string &key, int value );
        void addProperty( const std::string &key, double value );
        void addProperty( const std::string &key, bool value );
        void addProperty( const std::string &key, char value );
        void addProperty( const std::string &key, void* value );
        bool getExists( const std::string &key );
		//const std::string &getValueString(const char *key);
		const char* getValueString(const char *key);
        const std::string &getValueWithDefaultString( const std::string &key, const std::string &default_value );
		//const char* getValueWithDefaultString(const char *key, const char *default_value);
        const int getValueInt( const std::string &key );
        const int getValueWithDefaultInt( const std::string &key, int default_value );
        const double getValueDouble( const std::string &key );
        const double getValueWithDefaultDouble( const std::string &key, double default_value );
        const bool getValueBool( const std::string &key );
        const bool getValueWithDefaultBool( const std::string &key, bool default_value );
        const char getValueChar( const std::string &key );
        const char getValueWithDefaultChar( const std::string &key, char default_value );
        const void* getValuePtr( const std::string &key );
        const void* getValueWithDefaultPtr( const std::string &key, void* default_value );
        void deleteProperty( const std::string &key );
        void clearProperty();
		int size(){ return m_properties.size(); }
    private:
        std::map<const std::string, property_t> m_properties;
};


