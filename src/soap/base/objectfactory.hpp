#ifndef _objectfactory_H
#define	_objectfactory_H

#include <map>
#include <list>
#include <iostream>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace soap { namespace base {

/**
    The REGISTER_OBJECT macro allows to easily register an object in an object factory.
 */
#define REGISTER_OBJECT(factory, object, key) \
    namespace { \
        ObjectFactoryRegister<object> \
            _register_##object(factory, key); \
    }

/**
    \brief template class for object factory

    This class is a template for an object factory. The factory creates an instance of an derived class,
    be giving a key (e.g. a string) for the object which identifies it uniquely. This allows the implementation
    of new features (e.g. new file formats, new mapping algorithms) without touching or recompiling existing bits of code.

    If you don't understand this, read the book by Alexandresku (Modern C++ design)
    everything explained there in detail!
*/
template<typename key_t, typename T>
class ObjectFactory
{
private:
    typedef T* (*creator_t)();
public:

    typedef T abstract_type;
    typedef std::map<key_t, creator_t> assoc_map;
    
    ObjectFactory() {}
    ~ObjectFactory() {};
    
    /**
     * \brief register an object
     * \param key identifier
     * \param creator create policy
     *
     * This function is called to register an object in the factory. After an object is registered,
     * an instance of it can be created by calling Create specifying the corresponding key.
     */
    void Register(const key_t &key, creator_t creator );

    template< typename obj_t >
    void Register(const key_t &key);

    /**
       Create an instance of the object identified by key.
    */
    T *Create(const key_t &key);
    bool IsRegistered(const key_t & _id) const;

    static ObjectFactory<key_t, T>& Instance()
    {
        static ObjectFactory<key_t, T> _this;
        return _this;
    }

    const assoc_map &getObjects() { return _objects; }
private:
    assoc_map _objects;
};

template<class parent, class T> parent* create_policy_new()
{
    return new T();
}

template<typename key_t, typename T>
inline void ObjectFactory<key_t, T>::Register(const key_t &key, creator_t creator)
{
    (void)_objects.insert(typename assoc_map::value_type(key, creator)).second;
}

template<typename key_t, typename T>
template< typename obj_t >
inline void ObjectFactory<key_t, T>::Register(const key_t &key)
{
    Register(key, create_policy_new<abstract_type, obj_t>);
}


template<typename key_t, typename T>
inline T* ObjectFactory<key_t, T>::Create(const key_t &key)
{
    typename assoc_map::const_iterator it(_objects.find(key));
    if (it != _objects.end())
        return (it->second)();
    else
        throw std::runtime_error("factory key " + boost::lexical_cast<std::string>(key) + " not found.");
}

/*template<typename key_t, typename T>
inline static ObjectFactory<key_t, T>& ObjectFactory<key_t, T>::Instance()
{
    static ObjectFactory<key_t, T> _this;
    return _this;
}*/

template<typename key_t, typename T>
inline bool ObjectFactory<key_t, T>::IsRegistered(const key_t & _id) const
{
	return ( _objects.find(_id)!= _objects.end() );
}

/*std::string list_keys() const {
    std::stringstream _str;
    for (typename assoc_map::const_iterator it(map.begin()); it != map.end(); it++) {
        _str << (*it).first << "\n";
    }
    return _str.str();
}
*/

template<typename object_type >
class ObjectFactoryRegister {
public:
    template<typename factory_type, typename key_type>
    ObjectFactoryRegister(factory_type &factory, key_type &key) {
        factory.Register(key, &create_policy_new<typename factory_type::abstract_type, object_type>);
    }
};

}}

#endif	/* _objectfactory_H */
