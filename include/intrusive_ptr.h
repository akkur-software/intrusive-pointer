#pragma once
#include <stdint.h>
#include <type_traits>
#include <concepts>

template<class Derived>
class RefCountObject;

/// <summary>
/// A function that increases the count of references to an object in memory
/// </summary>
/// <param name="ptr">
/// - A pointer to an object that implements <see cref="RefCountObject"/>
/// </param>
template<class Derived>
inline void intrusive_ptr_add_ref(RefCountObject<Derived>* ptr)
{
    ++(ptr->m_ref_count);
}

/// <summary>
/// A function that reduces the count of references to an object in memory.
/// When the count of references is reduced to zero, the object is destroyed.
/// </summary>
/// <param name="ptr">
/// - A pointer to an object that implements <see cref="RefCountObject"/>
/// </param>
template<class Derived>
inline void intrusive_ptr_release(RefCountObject<Derived>* ptr)
{
    if (--(ptr->m_ref_count) == 0)
    {
        delete static_cast<const Derived*>(ptr);
    }
}

/// <summary>
/// A base class containing a counter of references 
/// to objects derived from it in memory
/// </summary>
template<class Derived>
class RefCountObject
{
    template<class Derived>
    friend void intrusive_ptr_add_ref(RefCountObject<Derived>* ptr);

    template<class Derived>
    friend void intrusive_ptr_release(RefCountObject<Derived>* ptr);

public:
    /// <summary>
    /// Returns the current number of references to an object in memory
    /// </summary>
    /// <returns>
    /// Current number of references to an object
    /// </returns>
    inline uint32_t ReferenceCount() const
    {
        return m_ref_count;
    }

protected:
    /// <summary>
    /// Provides a new instance of the base class <see cref="RefCountObject"/>
    /// </summary>
    RefCountObject() = default;

    /// <summary>
    /// Destroys the instance <see cref="RefCountObject"/>. 
    /// Destruction is only available through a derived class.
    /// </summary>
    virtual ~RefCountObject() = default;

private:
    uint32_t m_ref_count { 0 };
}; 

/// <summary>
/// A type concept that allows you to create intrusive pointers 
/// only to derived classes from <see cref="RefCountObject"/>
/// </summary>
template<typename T>
concept intrusive_counter_type = std::is_base_of_v<RefCountObject<T>, T>;
    
/// <summary>
/// An intrusive pointer to an object of a class derived from <see cref="RefCountObject"/>
/// </summary>
/// <typeparam name="T">
/// The type derived from <see cref="RefCountObject"/>
/// </typeparam>
template<intrusive_counter_type T>
class intrusive_ptr final
{
public:
    /// <summary>
    /// Provides a new empty instance of intrusive pointer
    /// </summary>
    inline intrusive_ptr() noexcept : m_pointer(nullptr) { }

    /// <summary>
    /// Provides a new instance of <see cref="intrusive_ptr"/>
    /// </summary>
    /// <param name="ptr">
    /// - A raw pointer to an instance, that implements <see cref="RefCountObject"/>
    /// </param>
    /// <param name="add_ref">
    /// - Is it worth increasing the reference count per instance. 
    /// The default value is <see langword="true"/>.
    /// </param>
    inline intrusive_ptr(T* ptr, bool add_ref = true) noexcept
    {
        set(ptr, add_ref);
    }

    /// <summary>
    /// Provides a new instance of <see cref="intrusive_ptr"/> 
    /// based on the specified one whose data was copied
    /// </summary>
    /// <param name="other">
    /// - A reference to another intrusive pointer
    /// </param>
    inline intrusive_ptr(const intrusive_ptr& other) noexcept
    {
        set(other.get(), true);
    }

    /// <summary>
    /// Provides a new instance of <see cref="intrusive_ptr"/> 
    /// based on the specified one whose data was moved
    /// </summary>
    /// <param name="other">
    /// - A reference to another intrusive pointer
    /// </param>
    inline intrusive_ptr(intrusive_ptr&& other) noexcept : m_pointer(other.detach()) { }

    /// <summary>
    /// Destroys a current instance of <see cref="intrusive_ptr"/>
    /// </summary>
    inline ~intrusive_ptr() noexcept
    {
        if (m_pointer != nullptr)
        {
            intrusive_ptr_release(m_pointer);
        }
    }

    /// <summary>
    /// Assigns the specified instance to the current one by copying the object data in memory
    /// </summary>
    /// <param name="other">
    /// - A reference to another intrusive pointer
    /// </param>
    /// <returns>
    /// A reference to current intrusive pointer
    /// </returns>
    inline intrusive_ptr& operator=(const intrusive_ptr& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        m_pointer = other.get();
        return *this;
    }

    /// <summary>
    /// Assigns the specified instance to the current 
    /// one by moving the object data in memory
    /// </summary>
    /// <param name="other">
    /// - A reference to another intrusive pointer
    /// </param>
    /// <returns>
    /// A reference to current intrusive pointer
    /// </returns>
    inline intrusive_ptr& operator=(intrusive_ptr&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }
        
        m_pointer = std::move(other);
        return *this;
    }

    /// <summary>
    /// Implements indirect access to the instance referenced 
    /// by the current instance of <see cref="intrusive_ptr"/> 
    /// </summary>
    /// <returns>
    /// A raw pointer to an instance, that implements <see cref="RefCountObject"/>
    /// </returns>
    inline T* operator->() const noexcept
    {
        return m_pointer;
    }

    /// <summary>
    /// Dereferences a pointer to an instance
    /// </summary>
    /// <returns>
    /// Reference to an instance that implements <see cref="RefCountObject"/>
    /// </returns>
    inline T& operator *() const
    {
        return *m_pointer;
    }

    /// <summary>
    /// Converts a current instance of <see cref="intrusive_ptr"/> 
    /// to a logical type <see langword="bool"/>
    /// </summary>
    inline explicit operator bool() const
    {
        return m_pointer != nullptr;
    }

    /// <summary>
    /// Checks whether the current and specified instances of 
    /// <see cref="intrusive_ptr"/> are equal
    /// </summary>
    /// <param name="other">
    /// - A reference to another intrusive pointer
    /// </param>
    /// <returns>
    /// Returns <see langword="true"/>, if the instances are equal to each other, 
    /// otherwise it returns <see langword="false"/>.
    /// </returns>
    inline bool operator==(const intrusive_ptr& other) const noexcept
    {
        return m_pointer == other.get();
    }

    /// <summary>
    /// Checks whether the current and specified instances of 
    /// <see cref="intrusive_ptr"/> are not equal 
    /// </summary>
    /// <param name="other">
    /// - A reference to another intrusive pointer
    /// </param>
    /// <returns>
    /// Returns <see langword="true"/>, if the instances are not equal to each other, 
    /// otherwise it returns <see langword="false"/>.
    /// </returns>
    inline bool operator!=(const intrusive_ptr& other) const noexcept
    {
        return !(*this == other);
    }
        
    /// <summary>
    /// Provides a raw pointer to an instance
    /// </summary>
    /// <returns>
    /// A raw pointer to an instance, that implements <see cref="RefCountObject"/>
    /// </returns>
    inline T* get() const
    {
        return m_pointer;
    }

    /// <summary>
    /// Detach the instance from the intrusive pointer. 
    /// The reference counter does not change.
    /// </summary>
    /// <returns>
    /// A raw pointer to an instance, that implements <see cref="RefCountObject"/>
    /// </returns>
    inline T* detach()
    {
        auto ptr = m_pointer;
        if (ptr)
        {
            m_pointer = nullptr;
        }
        return ptr;
    }

    /// <summary>
    /// Sets a pointer to a new instance in memory
    /// </summary>
    /// <param name="ptr">
    /// - A raw pointer to an instance, that implements <see cref="RefCountObject"/>
    /// </param>
    /// <param name="add_ref">
    /// - Is it worth increasing the reference count per instance. 
    /// The default value is <see langword="true"/>
    /// </param>
    inline void reset(T* ptr, bool add_ref = true)
    {
        auto old_ptr = m_pointer;
        set(ptr, add_ref);

        if (old_ptr)
        {
            intrusive_ptr_release(old_ptr);
        }
    }

    /// <summary>
    /// Exchanges the current and specified pointers with references to instances
    /// </summary>
    /// <param name="other">
    /// - A reference to another intrusive pointer
    /// </param>
    inline void swap(intrusive_ptr& other)
    {
        std::swap(*m_pointer, *other.get());
    }

    /// <summary>
    /// Returns the current number of references to an object in memory
    /// </summary>
    /// <returns>
    /// Current number of references to an object
    /// </returns>
    inline uint32_t use_count() const noexcept
    {
        return m_pointer != nullptr
            ? m_pointer->ReferenceCount() 
            : 0;
    }

private:
    inline void set(T* ptr, bool add_ref = true)
    {
        m_pointer = ptr;
        if (ptr && add_ref)
        {
            intrusive_ptr_add_ref(m_pointer);
        }
    }

private:
    T* m_pointer;
};

/// <summary>
/// Creates a new pointer to the instance 
/// that implements <see cref="RefCountObject"/>"/>
/// </summary>
/// <typeparam name="T">
/// The type that implements <see cref="RefCountObject"/>
/// </typeparam>
/// <typeparam name="...Args">
/// Package of constructor argument types
/// </typeparam>
/// <param name="...args">
/// - Arguments of the constructor of type that implements <see cref="RefCountObject"/>
/// </param>
/// <returns>
/// A new instance of <see cref="intrusive_ptr"/>
/// </returns>
template<intrusive_counter_type T, typename... Args>
inline intrusive_ptr<T> make_intrusive(Args... args)
{
    auto raw_ptr = new T(args...);        
    return intrusive_ptr<T>(raw_ptr);
}
